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
    Packet **packets = malloc(PACKET_COUNT * sizeof(*packets));
    if (!packets) return 1;

    for (size_t i = 0; i < PACKET_COUNT; i++) {
        packets[i] = malloc(sizeof(Packet));
        if (!packets[i]) return 1;
    }

    // Pretend we use the packets here.
    packets[0]->packet_length[0] = '1';

    for (size_t i = 0; i < PACKET_COUNT; i++) {
        free(packets[i]);
    }
    free(packets);
    return 0;
}
