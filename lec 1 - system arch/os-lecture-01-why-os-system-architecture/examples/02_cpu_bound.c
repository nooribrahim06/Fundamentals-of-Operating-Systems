#include <stdint.h>
#include <stdio.h>

int main(void) {
    volatile uint64_t sum = 0;

    for (uint64_t i = 0; i < 400000000ULL; ++i) {
        sum += (i ^ (i >> 3));
    }

    printf("sum=%llu\n", (unsigned long long)sum);
    return 0;
}
