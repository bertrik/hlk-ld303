#include "Arduino.h"
#include <SoftwareSerial.h>

#include "ld303-protocol.h"

#include "editline.h"
#include "cmdproc.h"

#define PIN_TX  D2
#define PIN_RX  D3

#define printf Serial.printf

static LD303Protocol protocol;
static SoftwareSerial radar(PIN_RX, PIN_TX);
static char cmdline[128];

static void printhex(const char *prefix, const uint8_t * buf, size_t len)
{
    printf(prefix);
    for (size_t i = 0; i < len; i++) {
        printf(" %02X", buf[i]);
    }
    printf("\n");
}

static int show_help(const cmd_t * cmds)
{
    for (const cmd_t * cmd = cmds; cmd->cmd != NULL; cmd++) {
        printf("%10s: %s\n", cmd->name, cmd->help);
    }
    return CMD_OK;
}

static int do_help(int argc, char *argv[]);

static int do_cmd(int argc, char *argv[])
{
    uint8_t buf[32];

    if (argc < 3) {
        return CMD_ARG;
    }
    uint8_t cmd = strtoul(argv[1], NULL, 0);
    uint16_t param = strtoul(argv[2], NULL, 0);

    size_t len = protocol.build_command(buf, cmd, param);
    printf("Sending cmd 0x%02X with param 0x%04X, len %d\n", cmd, param, len);
    printhex("- data: ", buf, len);
    radar.write(buf, len);

    return CMD_OK;
}

static int do_query(int argc, char *argv[])
{
    uint8_t buf[32];

    uint8_t param = argc < 2 ? 0xD3 : strtoul(argv[1], NULL, 0);
    printf("Querying with parameter 0x%02X\n", param);
    size_t len = protocol.build_query(buf, param);
    printhex("- data: ", buf, len);
    radar.write(buf, len);

    return CMD_OK;
}

static int do_mode(int argc, char *argv[])
{
    uint8_t buf[32];

    if (argc < 2) {
        return CMD_ARG;
    }
    uint16_t mode = atoi(argv[1]);
    printf("Setting mode %d\n", mode);
    size_t len = protocol.build_command(buf, CMD_PROTOCOL_TYPE, mode);
    printhex("- data: ", buf, len);
    radar.write(buf, len);

    return CMD_OK;
}

static int do_baud(int argc, char *argv[])
{
    uint8_t buf[32];

    if (argc < 2) {
        return CMD_ARG;
    }
    int baud = atoi(argv[1]);
    printf("Setting baud %d\n", baud);
    size_t len = protocol.build_command(buf, CMD_BAUD_RATE, baud / 100);
    printhex("- data: ", buf, len);
    radar.write(buf, len);

    delay(100);
    radar.begin(baud);

    return CMD_OK;
}

const cmd_t commands[] = {
    { "help", do_help, "Show help" },
    { "cmd", do_cmd, "<cmd> <param> Set a parameter" },
    { "q", do_query, "[param] Query the radar" },
    { "mode", do_mode, "<0|1|6|7> Set protocol mode" },
    { "baud", do_baud, "<baudrate> Set baud rate" },
    { NULL, NULL, NULL }
};

static int do_help(int argc, char *argv[])
{
    return show_help(commands);
}

void setup(void)
{
    Serial.begin(115200);
    radar.begin(115200);

    EditInit(cmdline, sizeof(cmdline));
}

void loop(void)
{
    uint8_t buf[256];

    // parse command line
    if (Serial.available()) {
        char c;
        bool haveLine = EditLine(Serial.read(), &c);
        Serial.write(c);
        if (haveLine) {
            int result = cmd_process(commands, cmdline);
            switch (result) {
            case CMD_OK:
                printf("OK\n");
                break;
            case CMD_NO_CMD:
                break;
            case CMD_ARG:
                printf("Invalid arguments\n");
                break;
            case CMD_UNKNOWN:
                printf("Unknown command, available commands:\n");
                show_help(commands);
                break;
            default:
                printf("%d\n", result);
                break;
            }
            printf(">");
        }
    }

    // print incoming serial bytes as HEX
    while (radar.available()) {
        uint8_t c = radar.read();
        printf(" %02X", c);

        // run receive state machine
        bool done = protocol.process_rx(c);
        if (done) {
            int len = protocol.get_data(buf);
            printhex("<", buf, len);
            uint8_t cmd = buf[0];
            switch (cmd) {
            case 0xD3:
                int dist = (buf[1] << 8) + buf[2];
                int pres = buf[4];
                int k = (buf[5] << 8) + buf[6];
                int micro = buf[7];
                int off = buf[8];
                printf("distance=%3d,present=%d,strength=%5d,micro=%d,off=%d\n", dist, pres, k, micro, off);
                break;
            }
        }
    }
}

