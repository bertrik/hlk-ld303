#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// command definitions
typedef enum {
    CMD_OPERATING_MODE = 0xB1,      // 0 = sensitive, 1 = stable
    CMD_FITTING_COEFFICIENT = 0xB2, // unit 0.001
    CMD_OFFSET_CORRECTION = 0xB3,   // unit 0.01 cm
    CMD_DELAY_TIME = 0xD1,          // 
    CMD_CLOSE_TREATMENT = 0xD2,     // 0 = keep last measured distance, 1 = clear distance result
    CMD_MEASUREMENT = 0xD3,         // read measurement?
    CMD_BAUD_RATE = 0xD4,           // unit 100 bps, e.g. 96 for 9600 bps
    CMD_TRIGGER_THRESHOLD = 0xD5,   // unit 'k'
    CMD_OUTPUT_TARGET = 0xD9,       // 0 = nearest, 1 = maximum goal
    CMD_SIGNAL_INTERVAL = 0xDA,     // range 5-20, unit 40 ms
    CMD_RESET = 0xDE,               // reset, 0
    CMD_MIN_DETECTION_DIST = 0xE0,  // unit 'cm'
    CMD_SENSITIVITY = 0xE1,         // unit 'k', 60-2000 (default 300)
    CMD_MAX_DETECTION_DIST = 0xE5,  // unit 'cm'
    CMD_REPORT_INTERVAL = 0xE6,     // range 0-20, unit 40 ms
    CMD_EXTREME_VALUE_STATS = 0xE7, // unit 'times'
    CMD_EXTREME_FILTER_TIMES = 0xE8,// unit 'times'
    CMD_NUMBER_OF_SWIPES = 0xE9,    // unit 'times'
    CMD_PROTOCOL_TYPE = 0xF6,       // 0 = ASCII, 1 = hex protocol 1, 6 = standard protocol (query), 7 = automatic
    CMD_PROPORTION_STATISTIC = 0xF9,// 0-100
    CMD_INVALID_DISTANCE = 0xFA,    // unit 'cm'
    CMD_PERCENTAGE = 0xFB,          // unit '%'
    CMD_FIRMWARE_UPGRADE = 0xDF,    // 1
    CMD_QUERY_PARAMETERS = 0xFE     // 0, query all parameters
} ld303_cmd_t;

// parsing state
typedef enum {
    STATE_HEADER_55,
    STATE_HEADER_A5,
    STATE_LEN,
    STATE_DATA,
    STATE_CHECK
} state_t;

class LD303Protocol {

private:
    state_t _state;
    uint8_t _sum;
    uint8_t _buf[32];
    uint8_t _len;
    uint8_t _idx;

public:
    LD303Protocol();

    // builds a parameter setting command
    size_t build_command(uint8_t *buf, uint8_t cmd, uint16_t data);

    // queries the module for measurement data (param usually 0xD3)
    size_t build_query(uint8_t *buf, uint8_t *data, size_t len);

    // processes received data, returns true if measurement data was found
    bool process_rx(uint8_t c);

    // call this when process_rx returns true, copies received data into buffer, returns length
    size_t get_data(uint8_t *data);

};

