// https://docs.platformio.org/en/latest/projectconf/sections/env/index.html#options
#include "reg51.h"

#define LED P2_0

int main() {
    LED = 0;
    return 0;
}
