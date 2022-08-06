#include "socketHandler.h"
#include "datatypes.h"
#include "log.h"

#include <algorithm> // std::rotate
#include <arpa/inet.h>

#include <netinet/in.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

packet_t *temp_data = (packet_t *)malloc(sizeof(packet_t) + MAXIMUM_SIZE);
uint8_t *temp_send = (uint8_t *)malloc(BUFF_SIZE);

int initSocket() { return socket(AF_INET, SOCK_STREAM, 0); }

int bindSocket(int socket_desc) {
  // optional options for sockets
  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(PORT_NUM);

  // Bind socket to port
  return bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
}

int waitForConnection(int socket_desc) {
  int c = sizeof(struct sockaddr_in);
  struct sockaddr_in client;
  int new_socket =
      accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c);

  if (new_socket < 0) {
    perror("Connection failed");
    return -1;
  }

  puts("Connection accepted!");
  char *client_ip = inet_ntoa(client.sin_addr);
  int client_port = ntohs(client.sin_port);
  printf("Connection from %s:%d\n", client_ip, client_port);

  return new_socket;
}

// int updateData(int new_socket, packet_t *data, packet_t *old_data) {
//   //   char socket_data[BUFF_SIZE + 1];
//   memcpy(old_data->data, data->data, data->len);
//   old_data->len = data->len;

//   int recv_data_length;
//   int offset = 0;
//   while (true) {
//     recv_data_length =
//         recv(new_socket, socket_data + offset, BUFF_SIZE + 1 - offset, 0);
//     if (recv_data_length <= 0)
//       return -1;
//     else
//       offset += recv_data_length;
//   }
//   strncpy(data, socket_data, BUFF_SIZE);
//   data[BUFF_SIZE] = '\0';
//   return 1;
// }

void initData(char *data) {
  for (int i = 0; i < BUFF_SIZE + 1; i++)
    data[i] = '\0';
}

int connectToServer(int socket_desc, bool local) {
  struct sockaddr_in server;
  char temp_addr[60];
  int temp_port = 0;
  server.sin_family = AF_INET;
  if (local) {
    server.sin_addr.s_addr = inet_addr(LOCAL_SERVER_IP_ADDR);
    server.sin_port = htons(LOCAL_PORT_NUM);
    strcpy(temp_addr, LOCAL_SERVER_IP_ADDR);
    temp_port = LOCAL_PORT_NUM;

  } else {
    server.sin_addr.s_addr = inet_addr(SERVER_IP_ADDR);
    server.sin_port = htons(PORT_NUM);
    strcpy(temp_addr, SERVER_IP_ADDR);
    temp_port = PORT_NUM;
  }
  log_debug("ip and port : %s, %d", temp_addr, temp_port);
  return connect(socket_desc, (struct sockaddr *)&server, sizeof(server));
}
int simpleSendpacket(int fd, packet_t *payload) {
  int connection_closed = 0;
  int send_data_length;
  int offset = 0;

  while (offset < payload->len) {
    send_data_length =
        send(fd, payload->data + offset, payload->len - offset, 0);
    if (send_data_length <= 0) { // error while sending data
      connection_closed = 1;
      perror("Send failed!");
      break;
    } else {
      offset += send_data_length;
    }
  }
  if (connection_closed) {
    return -1;
  }
  return 0;
}
int sendPacket(int fd, packet_t *payload) {
  int len = payload->len;
  int connection_closed = 0;
  int send_data_length;
  int offset = 0;
  int total_send = 0;
  int send_size = 0;
  while (total_send < len && !connection_closed) {
    send_size = (len - total_send) > (BUFF_SIZE - 1) ? BUFF_SIZE - 1
                                                     : (len - total_send);

    memset(temp_send, 0, BUFF_SIZE);
    if (send_size < BUFF_SIZE - 1) {
      temp_send[0] = 1;
    } else {
      temp_send[0] = 0;
    }
    memcpy(temp_send + 1, payload->data + total_send, send_size);
    while (offset < BUFF_SIZE) {
      // send_data_length = send(fd, &send_size, sizeof(int), 0);
      send_data_length = send(fd, temp_send + offset, BUFF_SIZE - offset, 0);
      if (send_data_length <= 0) { // error while sending data
        connection_closed = 1;
        perror("Send failed!");
        break;
      } else {
        offset += send_data_length;
      }
    }
    total_send += send_size;
    offset = 0;
  }
  if (connection_closed) {
    return -1;
  }
  return 0;
}

int closeConnection(int fd) {
  int send_data_length = send(fd, "##", 2, 0);
  if (send_data_length != 2) {
    puts("sending close signal failed");
    return -1;
  }
  return 0;
}

int readData(int socket, packet_t *data) {

  int recv_data_length = 0;
  int total_read_length = 0;
  int whole_frame_recieved = 0;
  int offset = 0;
  while (!whole_frame_recieved) {
    while (offset < BUFF_SIZE) {
      recv_data_length =
          recv(socket, temp_data->data + total_read_length + offset,
               BUFF_SIZE - offset, 0);
      if (recv_data_length <= 0)
        return -1;
      else {

        offset += recv_data_length;
      }
    }
    if (temp_data->data[total_read_length] == 1) { // this is the last frame
      printf("YYYYYYYYYYYY! , %d, %d\n", offset,
             total_read_length + (offset - 1));
      whole_frame_recieved = 1;
    }
    memmove(temp_data->data + total_read_length,
            temp_data->data + total_read_length + 1, BUFF_SIZE - 1);
    total_read_length += (offset - 1);
    offset = 0;
  }
  int counter = 0;
  int index = total_read_length - 1;
  while (index) {
    if (temp_data->data[index] == 0) {
      counter++;
    } else {
      break;
    }
    index--;
  }
  memcpy(data->data, temp_data->data, total_read_length - counter);
  data->len = total_read_length - counter;
  return 0;
}