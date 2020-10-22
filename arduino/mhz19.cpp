// MHZ 19 sensor
// implement only with MHZ19 sensor

#include "sensors.h"

#if (SENSOR_TYPE == SENS_MHZ19)

#include <stdint.h>
#include <stdbool.h>

#define CMD_SIZE 9
#define CO2_RX_PIN  2
#define CO2_TX_PIN  5

#define CO2RAW    // debug co2 communication
#define CO2WARMUP (2*1000)  // (2*60*1000)
#define SENSCYCLE (20*1000) // (60*60*1000)

typedef enum {
    START_BYTE,
    COMMAND,
    DATA,
    CHECK
} state_t;

/**
    Prepares a command buffer to send to an mhz19.
    @param data tx data
    @param buffer the tx buffer to fill
    @param size the size of the tx buffer
    @return number of bytes in buffer
*/
int prepare_tx(uint8_t cmd, const uint8_t *data, uint8_t buffer[], int size)
{
    if (size < CMD_SIZE) {
        return 0;
    }

    // create command buffer
    buffer[0] = 0xFF;
    buffer[1] = 0x01;
    buffer[2] = cmd;
    for (int i = 3; i < 8; i++) {
        buffer[i] = *data++;
    }

    // calculate checksum
    uint8_t check = 0;
    for (int i = 0; i < 8; i++) {
        check += buffer[i];
    }
    buffer[8] = 255 - check;

    return CMD_SIZE;
}

/**
    Processes one received byte.
    @param b the byte
    @param cmd the command code
    @param data the buffer to contain a received message
    @return true if a full message was received, false otherwise
 */
bool process_rx(uint8_t b, uint8_t cmd, uint8_t data[])
{
    static uint8_t check = 0;
    static int idx = 0;
    static int len = 0;
    static state_t state = START_BYTE;

    // update checksum
    check += b;

    switch (state) {
    case START_BYTE:
        if (b == 0xFF) {
            check = 0;
            state = COMMAND;
        }
        break;
    case COMMAND:
        if (b == cmd) {
            idx = 0;
            len = 6;
            state = DATA;
        } else {
            state = START_BYTE;
            process_rx(b, cmd, data);
        }
        break;
    case DATA:
        data[idx++] = b;
        if (idx == len) {
            state = CHECK;
        }
        break;
    case CHECK:
        state = START_BYTE;
        return (check == 0);
    default:
        state = START_BYTE;
        break;
    }

    return false;
}


static bool exchange_command(uint8_t cmd, uint8_t data[], unsigned int timeout)
{
    // create command buffer
    uint8_t buf[9];
    int len = prepare_tx(cmd, data, buf, sizeof(buf));

    // send the command
    co2sensor.write(buf, len);

    // wait for response
    unsigned long start = millis();
    while ((millis() - start) < timeout) {
        if (co2sensor.available() > 0) {
            uint8_t b = co2sensor.read();
            if (process_rx(b, cmd, data)) {
                return true;
            }
        }
        yield();
    }

    return false;
}

static void read_co2(uint16_t *co2)
{
    uint8_t data[] = { 0, 0, 0, 0, 0, 0 };
    bool result = exchange_command(0x86, data, 3000);
    if (result) {
        *co2 = (data[0] << 8) + data[1];
#ifdef CO2RAW
        char raw[32];
        sprintf(raw, "RAW: %02X %02X %02X %02X %02X %02X", data[0], data[1], data[2], data[3],
                data[4], data[5]);
        Serial.println(raw);
#endif
    }
}

#endif
