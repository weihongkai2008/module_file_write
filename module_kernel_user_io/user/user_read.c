#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <memory.h>

#define BLOCK_SIZE 2048

typedef struct LinkedNode{
	char *name_file;
  void *data;
  int flag;
  int file_block_size;
  struct LinkedNode *next_node;
}node_task;

void node_tail_add(node_task *list, node_task *node_add){
  node_task *tmp = list;

  while (tmp->next_node != NULL){
    tmp = tmp->next_node;
  }

  tmp->next_node = node_add;
}

void list_destroy(struct LinkedNode *list){

  node_task *del_node = list->next_node;
  node_task *cur = list;

  while((list->next_node != NULL) && (list->next_node->next_node != NULL)){
    del_node = list->next_node;

    list->next_node = list->next_node->next_node;
    free(del_node);
  }

  del_node = list->next_node;

  free(del_node);
  free(list);

}

void node_print(node_task *list){
  node_task *cur = list->next_node;
  while (cur != NULL){
    printf("%d\n", cur->file_block_size);
    cur = cur->next_node;
  }
}

int node_write(node_task *list){
  int fd_output;
  node_task *cur = list->next_node;

  if ((fd_output = open("/Users/weihongkai/Desktop/test.c", O_CREAT | O_RDWR | O_APPEND)) < 0) {
    printf("error creat file\n");
    exit(101);
  }
  printf("fd_output %d\n", fd_output);

  while (cur) {
    if ((write(fd_output, cur->data, cur->file_block_size)) != cur->file_block_size) {
      printf("error write file\n");
      exit(102);
    }
    cur = cur->next_node;
  }

  close(fd_output);
}

int main() {
  int fd_input;
  int block_once_size;
  int i = 1;

  node_task *list, *temp;
  void *buf;

  list = malloc(sizeof(node_task));
  list->next_node = NULL;
  buf = malloc(BLOCK_SIZE);

  if ((fd_input = open("/home/weihongkai/Desktop/helloworld.c", O_RDONLY)) < 0) {
    printf("error fetch file\n");
    exit(100);
  }
  printf("fd_input %d\n", fd_input);

  while (block_once_size = read(fd_input, buf, BLOCK_SIZE)) {
    temp = malloc(sizeof(node_task));

    temp->data = malloc(block_once_size);
		//temp->name = "helloworld.c";
    memmove(temp->data, buf, block_once_size);
    temp->flag = i++;
    temp->file_block_size = block_once_size;
    temp->next_node = NULL;

    node_tail_add(list, temp);

  }

  //node_write(list);
	printf("list->next address:%x\n", list->next_node);
	printf("%d\n", list->next_node->file_block_size);
  syscall(223, list->next_node);
	while(1){}
  list_destroy(list);

  close(fd_input);
  free(buf);
  return 0;
}

