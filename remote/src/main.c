#include <gbdk/platform.h>
#include <stdint.h>

#include "remote.h"
#include "remote_down.h"

inline uint8_t send_data(uint8_t b) {
    SB_REG = b;
    SC_REG = 0x81;
    while (SC_REG & 0x80);
    // Pulled high when link cable disconnected
    return (SB_REG != 0xff);
}

#define ID_DPAD 0
#define ID_BUTTONS 0x20

#define PARITY(A) (((A) ^ ((A) >> 1) ^ ((A) >> 2) ^ ((A) >> 3)) & 0x01)

uint8_t send_joypad(uint8_t j, uint8_t id) {
    return send_data((j) | ((PARITY(j)) ? (uint8_t) 0x90 : (uint8_t) 0x80) | id);
}

#define MS_PULSE_LENGTH 10
#define MS_PACKET_LENGTH 250

uint8_t send_ir_shutter(void) CRITICAL {
    for (uint8_t i = 0; i != MS_PACKET_LENGTH / MS_PULSE_LENGTH; i++) {
        RP_REG ^= 0x1;
        delay(MS_PULSE_LENGTH);
    }
    RP_REG = 0;
    return TRUE;
}

typedef struct submap_t {
    uint8_t x, y, w, h;
    const struct submap_t * next;
} submap_t;

const submap_t pad[] = {
        {0x04, 0x0d, 2, 3, pad + 1},     // RIGHT
        {0x01, 0x0d, 2, 3, pad + 2},     // LEFT
        {0x02, 0x0c, 3, 2, pad + 3},     // UP
        {0x02, 0x0f, 3, 2, NULL}         // DOWN
};

const submap_t btn[] = {
        {0x10, 0x0d, 3, 3, btn + 1},     // A
        {0x0d, 0x0e, 3, 3, btn + 2},     // B
        {0x06, 0x0f, 3, 2, btn + 3},     // SELECT
        {0x09, 0x0f, 3, 2, NULL}         // START
};

void update_joy(uint8_t joy, const submap_t * coords) {
    for (const submap_t * b = coords; (b); b = b->next, joy >>= 1) {
        if (joy & 0x01) {
            set_bkg_submap(DEVICE_SCREEN_X_OFFSET + b->x, DEVICE_SCREEN_Y_OFFSET + b->y, b->w, b->h, remote_down_map, (remote_down_WIDTH / remote_down_TILE_W));
        } else {
            set_bkg_submap(DEVICE_SCREEN_X_OFFSET + b->x, DEVICE_SCREEN_Y_OFFSET + b->y, b->w, b->h, remote_map, (remote_WIDTH / remote_TILE_W));
        }
    }
}

void main(void) {
    static uint8_t has_ir = FALSE;

    DISPLAY_OFF;
    if (_cpu == CGB_TYPE) {
        cgb_compatibility();
        has_ir = TRUE;
    }
    set_bkg_data(0, remote_TILE_COUNT, remote_tiles);
    set_bkg_data(remote_TILE_COUNT, remote_down_TILE_COUNT, remote_down_tiles);
    set_bkg_tiles(DEVICE_SCREEN_X_OFFSET, DEVICE_SCREEN_Y_OFFSET, remote_WIDTH / remote_TILE_W, remote_HEIGHT / remote_TILE_H, remote_map);
    SHOW_BKG;
    DISPLAY_ON;

    // delay a bit for aesthetics
    for (uint8_t i = 30; i != 0; i--) vsync();

    // refresh link state
    static uint8_t connected, old_connected;
    connected = send_data(0), old_connected = ~connected;

    static uint8_t joy = 0, old_joy;
    while (TRUE) {
        old_joy = joy, joy = joypad();

        if ((old_joy ^ joy) & 0x0f) {
            update_joy(joy & 0x0f, pad);
            connected = send_joypad(joy & 0x0f, ID_DPAD);
        }
        if ((old_joy ^ joy) & 0xf0) {
            update_joy(joy >> 4, btn);
            connected = send_joypad(joy >> 4, ID_BUTTONS);

            if (has_ir && !connected && (joy & J_A)) {
                send_ir_shutter();
            }
        }
        if (old_connected != connected) {
            set_bkg_submap(DEVICE_SCREEN_X_OFFSET + 2, DEVICE_SCREEN_Y_OFFSET + 4, 3, 2,
                        connected ? remote_down_map : remote_map,
                        (remote_WIDTH / remote_TILE_W));
            old_connected = connected;
        }
        vsync();
    }

}
