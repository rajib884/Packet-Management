#ifndef __PACKET_HASH_TABLE_H__
#define __PACKET_HASH_TABLE_H__

#include "hash-table.h"
#include "ipv4-packet.h"

extern HashTable_t *packet_hash_table;

HashTable_t *packet_hash_table_create(uint64_t capacity);
void *key_from_ip(ipv4_datagram_t *datagram);

#endif /* __PACKET_HASH_TABLE_H__ */