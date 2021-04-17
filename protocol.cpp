

#include "protocol.h"


size_t Protocol::build_tx(uint8_t *buf, uint8_t cmd, uint16_t data)
{
    int idx = 0;
    buf[idx++] = 0xBA;
    buf[idx++] = 0xAB;
    buf[idx++] = cmd;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = data & 0xFF;
    buf[idx++] = 0;
    buf[idx++] = 0x55;
    buf[idx++] = 0xBB;

    return idx;
}


bool Protocol::process_rx(uint8_t c, uint8_t cmd)
{
    return false;
}


