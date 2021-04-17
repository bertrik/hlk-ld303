#include "ld303-protocol.h"


LD303Protocol::LD303Protocol(void)
{
    _state = STATE_HEADER_55;
    _len = 0;
}

// protocol 6
// 55 5A 02 D3 84  open/close, "fixed query command"
//
// BA AB .... set parameter
// 
// change query mode: BAAB 00 F6 0007 00 55BB = send cmd F6 with parameter 0007
// -> reports data continuously?

size_t LD303Protocol::build_query(uint8_t *buf, uint8_t param)
{
    int idx = 0;
    uint8_t sum = 0;

    buf[idx++] = 0x55;  // header
    buf[idx++] = 0x5A;
    buf[idx++] = 0x02;  // length
    buf[idx++] = param;  // query parameter
    for (int i = 0; i < idx; i++) {
        sum += buf[idx];
    }
    buf[idx++] = sum;

    return idx;
}

size_t LD303Protocol::build_command(uint8_t *buf, uint8_t cmd, uint16_t data)
{
    int idx = 0;
    buf[idx++] = 0xBA;  // header
    buf[idx++] = 0xAB;
    buf[idx++] = 0x00;  // address
    buf[idx++] = cmd;
    buf[idx++] = (data >> 8) & 0xFF;
    buf[idx++] = data & 0xFF;
    buf[idx++] = 0;     // checksum
    buf[idx++] = 0x55;
    buf[idx++] = 0xBB;

    return idx;
}

// message from radar: "uplink data, (sent by module)" 
// 55       A5      0A      XX      XX      XX      XX      XX      XX      XX      XX      XX      XX
// header           len     adress  distance        rsvd    state   signal strength micro   closed  check

bool LD303Protocol::process_rx(uint8_t c, uint8_t cmd)
{
    switch (_state) {
    case STATE_HEADER_55:
        if (c == 0x55) {
            _state = STATE_HEADER_A5;
            _sum = c;
        }
        break;
    case STATE_HEADER_A5:
        _sum += c;
        if (c == 0xA5) {
            _state = STATE_LEN;
        } else {
            _state = STATE_HEADER_55;
        }
        break;
    case STATE_LEN:
        _sum += c;
        if (c < sizeof(_buf)) {
            _len = c;
            _idx = 0;
            _state = STATE_DATA;
        } else {
            _state = STATE_HEADER_55;
        }
        break;
    case STATE_DATA:
        _sum += c;
        if (_idx < _len) {
            _buf[_idx++] = c;
        } else {
            _state = STATE_CHECK;
        }
        break;
    case STATE_CHECK:
        _state = STATE_HEADER_55;
        return (c == _sum);

    default:
        _state = STATE_HEADER_55;
        break;
    }
    return false;
}

