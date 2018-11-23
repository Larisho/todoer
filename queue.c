#include <stdlib.h>
#include <string.h>
#include "queue.h"

Node *head = NULL;
Node *tail = NULL;

void destroy_queue() {
  while (head != NULL) {
    Node *const temp = head;
    head = head->next;
    free(temp->path);
    free(temp);
  }
}

void enqueue(char *path) {
  Node *node = malloc(sizeof *node);
  if (!node) {
    destroy_queue();
    exit(-1);
  }

  node->path = malloc(strlen(path) + 1);
  if (!node->path) {
    destroy_queue();
    exit(-1);
  }

  strcpy(node->path, path);
  node->next = NULL;
  
  if (head == NULL) {
    head = tail = node;
  }
  else {
    tail->next = node;
    tail = node;
  }
}

char *dequeue() {
  char *path;

  if (head == NULL) {
    return NULL;
  }

  path = head->path;
  
  if (head == tail) {
    free(head);
    head = tail = NULL;
  }
  else {
    Node *temp = head;
    head = head->next;
    free(temp);
  }

  return path;
}
