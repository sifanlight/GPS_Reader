#ifndef _SOCKET_HANDLER_H_
#define _SOCKET_HANDLER_H_

#include "datatypes.h"

#define MAXIMUM_SIZE 450

int initSocket();
int bindSocket(int);
int waitForConnection(int);
int updateData(int, char *, char *);
void initData(char *);
int connectToServer(int socket_desc, bool local = false);
int simpleSendpacket(int fd, packet_t *payload);
int readData(int socket, packet_t *data);
int sendPacket(int fd, packet_t *payload);

#endif // _SOCKET_HANDLER_H_
