#include <stdio.h>
#include <stdlib.h>

#define PACKET_COUNT 1000000

typedef struct {
    char src_ip[16];
    char dst_ip[16];
    char src_port[4];
    char dst_port[4];
    char packet_length[4];
} Packet; // 44 bytes in this demo

int main(void) {
    Packet *packets = malloc(PACKET_COUNT * sizeof(*packets));
    if (!packets) return 1;

    // Pointer arithmetic understands the pointed-to type:
    // packets + 1 advances by sizeof(Packet), not by 1 byte.
    Packet *first  = packets;
    Packet *second = packets + 1;
    Packet *third  = packets + 2;

    first->packet_length[0]  = '1';
    second->packet_length[0] = '2';
    third->packet_length[0]  = '3';

    free(packets);
    return 0;
}
