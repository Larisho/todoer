# TODOER

## Building
To build, just run `make`. This project uses `clang` by default, but it can also be compiled on `gcc`.

## `man todoer`
```
DESCRIPTION:
  This program recursively searches the files in the CWD (current working directory) for TODO comments
  and prints them to STDOUT with file and line/column number information.

Usage: todoer [OPTIONS]

OPTIONS:
  -c		Just print the number of TODOs found
  
  -d <PATH>	Start searching for TODOs in the directory specified by path.
     		Path must point to a _directory_.

  -e <DNAMES>	Exclude directories whose name matches of the names provided.
     		Ex: -e 'build_output;dist' to exclude files in the 'build_output' and 'dist' directories.

EXAMPLES:
  todoer -d ~/source/my-proj -e 'dist;node_modules'	Get all the TODOs in the my-proj directory while
  	    		     				excluding files in the 'dist' and 'node_modules'
							directories.

  todoer -cd ~/source/my-proj				Print the number of TODOs found in the my-proj
  	     						directory.

LICENSE:
  MIT

AUTHOR:
  Gab David <https://github.com/Larisho>
```

## The point
The point of this is to have a utility that tracks all my TODOs in my software projects. This would only ever be a backend to the tracking of TODOs.

## Improvements
- [ ] Find TODOs in more than just C-based languages (currently searching for `// TODO`).
- [ ] Exclude directory path as opposed to by directory name
- [ ] Exclude directories based on regex
- [ ] Exclude files by name and regex
- [ ] Abbreviate lines that are longer than X characters (or something like this...)

## Alterior motives
This project has also been created as a way to compare C with Rust in performance and ease of development. Eventually, I will write a Rust version of this tool and will measure some benchmarks between both programs.

## License
Standard MIT license. 
