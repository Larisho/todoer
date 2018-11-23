#if !defined(QUEUE_H)

typedef struct node_t {
  char *path;
  struct node_t *next;
} Node;

void enqueue(char *);
char *dequeue();
void destroy_queue();


#define QUEUE_H
#endif
