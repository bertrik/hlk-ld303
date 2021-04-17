#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


// command definitions
typedef enum {
    CMD_OPERATING_MODE = 0xB1,      // 0 = sensitive, 1 = stable
    CMD_FITTING_COEFFICIENT = 0xB2, // unit 0.001
    CMD_OFFSET_CORRECTION = 0xB3,   // unit 0.01 cm
    CMD_CLOSE_TREATMENT = 0xD2,     // 0 = keep last measured distance, 1 = clear distance result
    CMD_BAUD_RATE = 0xD4,           // unit 100 bps, e.g. 96 for 9600 bps
    CMD_TRIGGER_THRESHOLD = 0xD5,   // unit 'k'
    CMD_OUTPUT_TARGET = 0xD9,       // 0 = nearest, 1 = maximum goal
    CMD_SIGNAL_INTERVAL = 0xDA,     // range 5-20, unit 40 ms
    CMD_RESET = 0xDE,               // reset, 0
    CMD_DATA_RESPONSE_TIME = 0xE6   // range 0-20, unit 40 ms
    // etc
} ld303_cmd_t;

// parsing state
typedef enum {
} state_t;

class Protocol {

private:
    state_t _state;

private:

public:
    size_t build_tx(uint8_t *buf, uint8_t cmd, uint16_t data);
    bool process_rx(uint8_t c, uint8_t cmd);

};

