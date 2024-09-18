#ifndef __PACKET_HASH_TABLE_H__
#define __PACKET_HASH_TABLE_H__

#include "hash-table.h"

extern HashTable_t *packet_hash_table;

void init_packet_hash_table();
void *key_from_ip(ip_addr_t *src, ip_addr_t *dest);

#endif /* __PACKET_HASH_TABLE_H__ */