#include <stdbool.h>
#include <string.h>
#include <iostream>
#include "datatypes.h"
#include "encoder.h"
#include "log.h"
#include "socketHandler.h"
#include "stdint.h"

packet_t *temp_packet = (packet_t *)malloc(sizeof(packet_t) + MAXIMUM_SIZE);

void encodeData(packet_t *pckt, packet_t *key) {
  bool swap = false;
  int max_len = sizeof(packet_t) + MAXIMUM_SIZE;
  int len;
  // log_trace("asb %d,%d", pckt->len, key->len);
  if (key->len > pckt->len) {
    swap = true;
    memcpy(temp_packet, key, max_len);
    memcpy(key, pckt, max_len);
    memcpy(pckt, temp_packet, max_len);
  }
  // log_trace("asb2 %d,%d", pckt->len, key->len);

  // log_warn("pckt len %d, key ( old ) len %d , %d", pckt->len, key->len,
  //          sizeof(packet_t) + pckt->len + key->len - (pckt->len % key->len));

  if (pckt->len % key->len != 0) {
    log_warn("pad added");
    std::cout << pckt->len << ", " << "key len: " << key->len << std::endl;
    int padded_len = pckt->len + key->len - (pckt->len % key->len);
    if (padded_len > MAXIMUM_SIZE) {
      log_error("error in size %d", padded_len);
    }
    std::cout << "padded_len: " << padded_len << ", packet-len: " << pckt->len << std::endl;
    memset(pckt->data + pckt->len, 0, padded_len - pckt->len);
    pckt->len = padded_len;
  }

  log_warn("ohy: %d, %d, %d", pckt->len, key->len, swap);

  for (int j = 0; j < pckt->len / key->len; j++) {
    for (int i = 0; i < key->len; i++)
      pckt->data[j * key->len + i] ^= key->data[i];
  }

  if (swap) {
    // memcpy(temp_packet, key, max_len);
    // memcpy(key, pckt, max_len);
    // memcpy(pckt, temp_packet, max_len);
    pckt->len = key->len;
  }

  log_debug("address in %p ,%p", (pckt), (key));
}
