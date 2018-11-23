#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <stdbool.h>
#include "queue.h"

#define PATH_MAX 4096

typedef struct {
  char base_dir[PATH_MAX];
  char *exclude_dirs;
  bool output_count;
} Options;

int searchFile(const char *filename, bool count_only) {
  FILE *file = fopen(filename, "r");
  if (file == NULL)
    return 0;

  int line_number = 0;
  int matches = 0;
  bool found_match = false;

  int chars_read = 0;
  size_t n = 0;

  char *line = NULL;
  while ((chars_read = getline(&line, &n, file)) > 0) {
    line_number++;

    char *pch = strstr(line, "// TODO");

    if (pch) {
      matches++;
      
      if (!count_only) {
	if (!found_match) {
	  printf("%s:\n", filename);
	  found_match = true;
	}

	char line_col[11];
	snprintf(line_col, 11, "(%d:%td)", line_number, pch - line + 1);

	printf("%-4s%11s %s%s", "", line_col, pch, (line[chars_read - 1] == '\n' ? "" : "\n"));
      }
    }
  }
  
  if (found_match) {
    printf("\n");
  }

  free(line);
  fclose(file);

  return matches;
}

int isValidDirectory(const char *dir) {
  if (dir[0] == '.') {
    if (dir[1] == '\0' || (dir[1] == '.' && dir[2] == '\0')) {
      return 0;
    }
  }
  
  return 1;
}

bool is_excluded(char d_name[256], char *excluded) {
  if (excluded == NULL)
    return false;

  char buffer[256] = {0};
  char *i = excluded;
  int buf = 0;

  for (; *i; buf++) {
    if (*i != ';' && buf < 255) {
      buffer[buf] = *i;
      i++;
      continue;
    }

    buffer[buf] = '\0';
    if (strncmp(buffer, d_name, 256) == 0) {
      return true;
    }

    memset(buffer, 0, sizeof buffer);
    buf = -1;
    i++;
  }

  if (buf > -1 && strncmp(buffer, d_name, 256) == 0) {
    return true;
  }
  
  return false;
}

void walkDirectoryTree(Options *opts) {
  
  struct dirent *dir; /* Directory object */
  struct stat file_stat; /* Stats object */
  char *base; /* Absolute value of current directory */
  char *path = malloc(PATH_MAX * sizeof(char)); /* Allocate space for path */
  int total_matches = 0;
  
  /* While items are still left in the queue... */
  while ((base = dequeue())) {

    /* Open directory stream */
    DIR *d = opendir(base);  /* Directory stream */

    if (errno != 0) {
      /* If user doesn't have read access, skip it */
      if (errno == EACCES) {
	errno = 0;
	free(base);
	continue;
      }

      if (errno == ENOTDIR || errno == ENOENT) {
	perror(base);
	free(base);
	free(path);
	destroy_queue();
	exit(-1);
      }
    }

    if (d) { /* If stream isn't null... */
      while ((dir = readdir(d))) { /* For each file in the current directory... */

        /* compose path, adding separator if needed */
        sprintf(path, "%s%s%s", base, (base[strlen(base)-1] == '/' ? "" : "/"), dir->d_name);

	lstat(path, &file_stat); /* Get stats for path */
	if (errno != 0) {
          if (errno == EIO || errno == ENOENT || errno == ELOOP) {
            perror(path);
            free(path);
            free(base);
            closedir(d);
            destroy_queue();
            exit(-1);
          }
	  else {
            errno = 0;
            continue;
          }
        }
	
	if (S_ISDIR(file_stat.st_mode) && isValidDirectory(dir->d_name)) { /* If path points to directory and it isn't '.' or '..'... */
	  if (!is_excluded(dir->d_name, opts->exclude_dirs))
	    enqueue(path); /* Add the directory to the queue */
	}
	else if (S_ISREG(file_stat.st_mode)) { // TODO: Only search if file && not binary file
	  total_matches += searchFile(path, opts->output_count); /* Search file for pattern */
	}
      }

      free(base);
    }

    closedir(d); /* Be nice and close the directory stream :) */
  }

  if (opts->output_count)
    printf("TODOs: %d\n", total_matches);

  free(path);
}

void print_help() {
  printf("DESCRIPTION:\n"
	 "  This program recursively searches the files in the CWD (current working directory) for TODO comments\n"
	 "  and prints them to STDOUT with file and line/column number information.\n"
	 "\n"
	 "Usage: todoer [OPTIONS]\n"
	 "\n"
	 "OPTIONS:\n"
	 "  -c           Just print the number of TODOs found\n"
	 "  -d <PATH>    Start searching for TODOs in the directory specified by path.\n"
	 "               Path must point to a directory.\n"
	 "  -e <DNAMES>  Exclude directories whose name matches of the names provided.\n"
	 "               Ex: -e 'build_output;dist' to exclude files in the 'build_output' and 'dist' directories.\n"
	 "\n"
	 "EXAMPLES:\n"
	 "  todoer -d ~/source/my-proj -e 'dist;node_modules'     Get all the TODOs in the my-proj directory while\n"
	 "                                                        excluding files in the 'dist' and 'node_modules'\n"
	 "                                                        directories.\n"
	 "  todoer -cd ~/source/my-proj                           Print the number of TODOs found in the my-proj\n"
	 "                                                        directory.\n"
	 "\n"
	 "LICENSE:\n"
	 "  MIT\n\n");
}

void parse(int argc, char **argv, Options *opts) {
  int match;
  bool directory_assigned = false;
  bool output_count_assigned = false;
  bool exclude_dirs_assigned = false;
  
  while ((match = getopt(argc, argv, "d:e:ch")) != -1) {
    switch (match) {
    case 'd':
      strncpy(opts->base_dir, optarg, PATH_MAX);
      directory_assigned = true;
      break;
    case 'c':
      opts->output_count = true;
      output_count_assigned = true;
      break;
    case 'e':
      opts->exclude_dirs = strdup(optarg);
      exclude_dirs_assigned = true;
      break;
    case 'h':
      print_help();
      exit(0);
      break;
    case '?':
      // TODO(gab): Maybe consider removing this logic since the getopt library prints stuff out
      if (optopt == 'd' || optopt == 'e')
	fprintf (stderr, "Option -%c requires an argument.\n", optopt);
      else if (isprint (optopt))
	fprintf (stderr, "Unknown option `-%c'.\n", optopt);
      else
	fprintf (stderr, "Unknown option character `\\x%x'.\n", optopt);
      
      exit(-1);
    }
  }

  if (!directory_assigned) {
    getcwd(opts->base_dir, PATH_MAX);
  }

  if (!output_count_assigned) {
    opts->output_count = false;
  }

  if (!exclude_dirs_assigned) {
    opts->exclude_dirs = NULL;
  }
}

int main(int argc, char **argv) {
  Options opts;

  parse(argc, argv, &opts);

  enqueue(opts.base_dir);

  walkDirectoryTree(&opts);

  free(opts.exclude_dirs); /* Free pointer here (allocated using strdup) */
  destroy_queue();
  
  return 0;
}

/* Test code for the is_excluded method */
/* struct Test { */
/*   char buffer[256]; */
/*   char *exclusion_list; */
/* }; */

/* int main() { */

/*   struct Test arr[7] = { {"node_modules", "src;node;node_modules"}, */
/* 			 {"src", "node;nol;hold;"}, */
/* 			 {"weee", "weee"}, */
/* 			 {"holdthephone!", "hold;the;phone;!"}, */
/* 			 {"", "w00t"}, */
/* 			 {"w00t", ""}, */
/* 			 {"", ""} }; */
  
/*   for (int i = 0; i < 7; i++) { */
/*     printf("d_name: '%s', exclusion list: '%s', is_excluded?: %s\n", */
/* 	   arr[i].buffer, arr[i].exclusion_list, */
/* 	   is_excluded(arr[i].buffer, arr[i].exclusion_list) ? "excluded": "not excluded"); */
/*   } */
/* } */
