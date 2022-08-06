#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <queue>
#include <semaphore.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "datatypes.h"
#include "encoder.h"
#include "log.h"
#include "socketHandler.h"

pthread_mutex_t count_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t full, empty;
std::queue<packet_t *> buffer;

volatile bool flag = false;
int socket_i = 0;

int socket_desc;
// Functions prototype:
void sigintHandler(int);
FILE *fptr;

packet_t *data;
packet_t *old_data;
packet_t *wait_data;
pthread_t read_data, process_data;

void *read_task(void *params) {

  while (1) {
    puts("Waiting for incoming connections...");

    int new_socket = waitForConnection(socket_desc);
    if (new_socket == -1) {
      continue;
    }
    while (true) {

      memcpy(old_data, wait_data, sizeof(packet_t) + wait_data->len);

      int err = readData(new_socket, data);
      if (err != -1) {
        memcpy(wait_data, data, sizeof(packet_t) + data->len);
        encodeData(data, old_data);
        std::cout<< data->data;

        // packet_t *temp_data = (packet_t *)malloc(sizeof(packet_t) + data->len);
        // memcpy((void *)temp_data, (void *)data, sizeof(packet_t) + data->len);

        // sem_wait(&empty);
        // pthread_mutex_lock(&count_mutex);

        // buffer.push(temp_data);

        // pthread_mutex_unlock(&count_mutex);
        // sem_post(&full);

      } else {
        std::cout << "error in read task \r\n";
      }
    }
  }
}

// void *process_task(void *params) {

//   int connection_closed = 0;
//   int socket_desc = initSocket();
//   if (socket_desc < 0) {
//     log_error("Error: Could not create socket");
//     perror("socket");
//     // unable to create socket
//     pthread_exit(NULL);
//   }

//   if (connectToServer(socket_desc, true) < 0) {

//     log_error("Error: Could not connect to server");
//     perror("socket");
//     pthread_exit(NULL);
//   }
//   while (true) {

//     sem_wait(&full);

//     pthread_mutex_lock(&count_mutex);

//     packet_t *temp_data = buffer.front();
//     buffer.pop();
//     pthread_mutex_unlock(&count_mutex);

//     std::cout << " data len " << temp_data->len << std::endl;
//     connection_closed = simpleSendpacket(socket_desc, temp_data);
//     free(temp_data);
//     if (connection_closed != 0) {
//       log_error("connection closed!");
//     }

//     sem_post(&empty);
//   }
// }

int main(char argc, char *argv[]) {

  data = (packet_t *)malloc(sizeof(packet_t) + MAXIMUM_SIZE);
  old_data = (packet_t *)malloc(sizeof(packet_t) + MAXIMUM_SIZE);
  wait_data = (packet_t *)malloc(sizeof(packet_t) + MAXIMUM_SIZE);

  old_data->len = MAXIMUM_SIZE / 2;
  memset(old_data->data, 0, MAXIMUM_SIZE);

  data->len = MAXIMUM_SIZE / 2;
  memset(data->data, 0, MAXIMUM_SIZE);

  wait_data->len = MAXIMUM_SIZE / 2;
  memset(wait_data->data, 0, MAXIMUM_SIZE);

  // int err = sem_init(&empty, 0, 10);
  // if (err < 0) {
  //   std::cout << "error" << std::endl;
  // }
  // err = sem_init(&full, 0, 0);
  // if (err < 0) {
  //   std::cout << "error" << std::endl;
  // }

  socket_desc = initSocket();
  if (socket_desc < 0) {
    printf("Error: Could not create socket");
    // unable to create socket
    return 1;
  }

  // handle SIGINT
  signal(SIGINT, sigintHandler);
  if (bindSocket(socket_desc) < 0) {
    printf("Error: Could not bind socket to port");
    return 1;
  } else
    puts("bind done!");

  // listening for input requests
  listen(socket_desc, 1);

  pthread_create(&read_data, NULL, read_task, NULL);
  // pthread_create(&process_data, NULL, process_task, NULL);

  pthread_join(read_data, NULL);
  // pthread_join(process_data, NULL);

  return 0;
}

void sigintHandler(int socket_desc) {
  close(socket_desc);
  pthread_cancel(read_data);
  // pthread_cancel(process_data);
  // sem_destroy(&full);
  // sem_destroy(&empty);
  pthread_mutex_destroy(&count_mutex);
  printf("\nServer closed\n");
  exit(0);
}
