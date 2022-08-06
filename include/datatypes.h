#ifndef _DATATYPES_H_
#define _DATATYPES_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif /**< _cplusplus */

#define BUFF_SIZE 30
#define PORT_NUM 11222
#define SERVER_IP_ADDR "127.0.0.1"
#define LOCAL_SERVER_IP_ADDR "127.0.0.1"
#define LOCAL_PORT_NUM 11220

static char SHARED_KEY[BUFF_SIZE + 1] = "aErT";

typedef struct {
  int len;
  uint8_t data[0];
} __attribute__((packed)) packet_t;

#ifdef __cplusplus
}
#endif /**< _cplusplus */

#endif // _DATATYPES_H_
