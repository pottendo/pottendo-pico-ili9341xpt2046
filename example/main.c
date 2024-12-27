/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdbool.h>
#include "pico-ili9341xpt2046/ili9341.h"
#include "pico-ili9341xpt2046/ili9341_framebuffer.h"
#include "pico-ili9341xpt2046/ili9341_draw.h"
#include "pico-ili9341xpt2046/xpt2046.h"
#include "pico-ili9341xpt2046/ugui.h"

ili9341_config_t ili9341_config = {
	.port = spi1,
	.pin_miso = 11,
	.pin_cs = 13,
	.pin_sck = 10,
	.pin_mosi = 12,
	.pin_reset = 15,
	.pin_dc = 14,
    .pin_led = 8
};

#define LED_PIN 25

bool timer_ts_poll_callback(struct repeating_timer *t);

void pixel_set(UG_S16 x, UG_S16 y, UG_COLOR rgb);
UG_RESULT _HW_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR rgb);
UG_RESULT _HW_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR rgb);

void window_1_callback( UG_MESSAGE* msg );
void button_green_click(void);
void button_red_click(void);

static UG_GUI gui;
static UG_WINDOW window_1;
static UG_BUTTON button1_1;
static UG_BUTTON button1_2;

#define MAX_OBJECTS 10
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];

void pixel_set(UG_S16 x, UG_S16 y, UG_COLOR rgb)
{
    uint16_t R = (rgb >> 16) & 0x0000FF;
    uint16_t G = (rgb >> 8) & 0x0000FF;
    uint16_t B = rgb & 0x0000FF;
    UG_COLOR RGB16 = RGBConv(R,G,B);
    draw_pixel(x,y,RGB16);
}

UG_RESULT _HW_DrawLine(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR rgb)
{
    uint16_t R = (rgb >> 16) & 0x0000FF;
    uint16_t G = (rgb >> 8) & 0x0000FF;
    uint16_t B = rgb & 0x0000FF;
    UG_COLOR RGB16 = RGBConv(R,G,B);
    if (x1 == x2) {
        draw_vertical_line(x1,y1,y2-y1,RGB16);
    } else if (y1 == y2) {
        draw_horizontal_line(x1,y1,x2-x1,RGB16);
    } else {
        //drawLine(x1,y1,x2,y2,RGB16);
    }
    return UG_RESULT_OK;
}

UG_RESULT _HW_FillFrame(UG_S16 x1, UG_S16 y1, UG_S16 x2, UG_S16 y2, UG_COLOR rgb)
{
    uint16_t R = (rgb >> 16) & 0x0000FF;
    uint16_t G = (rgb >> 8) & 0x0000FF;
    uint16_t B = rgb & 0x0000FF;
    UG_COLOR RGB16 = RGBConv(R,G,B);
    fill_rectangle_alt(x1,x2,y1,y2,RGB16);
    return UG_RESULT_OK;
}

/* Pico W devices use a GPIO on the WIFI chip for the LED,
   so when building for Pico W, CYW43_WL_GPIO_LED_PIN will be defined */
#ifdef CYW43_WL_GPIO_LED_PIN
#include "pico/cyw43_arch.h"
#endif

#ifndef LED_DELAY_MS
#define LED_DELAY_MS 250
#endif

/* Perform initialisation */
int pico_led_init(void) {
#if defined(PICO_DEFAULT_LED_PIN)
    // A device like Pico that uses a GPIO for the LED will define PICO_DEFAULT_LED_PIN
    // so we can use normal GPIO functionality to turn the led on and off
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    return PICO_OK;
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // For Pico W devices we need to initialise the driver etc
    return cyw43_arch_init();
#endif
}

// Turn the led on or off
void pico_set_led(bool led_on)
{
#if defined(PICO_DEFAULT_LED_PIN)
    // Just set the GPIO on or off
    gpio_put(PICO_DEFAULT_LED_PIN, led_on);
#elif defined(CYW43_WL_GPIO_LED_PIN)
    // Ask the wifi "driver" to set the GPIO on or off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, led_on);
#endif
}

void window_1_callback(UG_MESSAGE *msg)
{
    pico_set_led(true);
    sleep_ms(50);
    pico_set_led(false);
    if (msg->type == MSG_TYPE_OBJECT)
    {
        if (msg->id == OBJ_TYPE_BUTTON)
        {
            switch (msg->sub_id)
            {
            case BTN_ID_0: // Toggle green LED
            {
                button_green_click();
                break;
            }
            case BTN_ID_1: // Toggle red LED
            {
                button_red_click();
                break;
            }
            default:
                break;
            }
        }
    }
}

void button_green_click(void)
{
    static bool state = false;
    state = !state;
    if (state) {
        pico_set_led(true);
        UG_ButtonSetText(&window_1, BTN_ID_0, "An");
    } else {
        pico_set_led(false);
        UG_ButtonSetText(&window_1, BTN_ID_0, "Aus");
    }
}

void button_red_click(void)
{
    static bool state = false;
    state = !state;
    if (state) {
        pico_set_led(true);
        UG_ButtonSetText(&window_1, BTN_ID_1, "ON");
    } else {
        pico_set_led(false);
        UG_ButtonSetText(&window_1, BTN_ID_1, "OFF");
    }
}

bool timer_ts_poll_callback(struct repeating_timer *t)
{
    if (gpio_get(TS_IRQ_PIN) == 0) {
        uint16_t x = ts_get_x();
        uint16_t y = ts_get_y();
        if ((x > 0 && x < 239) && (y > 0 && y < 319)) {
            UG_TouchUpdate(240 - x, 320 - y, TOUCH_STATE_PRESSED);  /* Fixme for rotation */
        }
    } else {
        UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
    }
    UG_Update();
    return true;
}


int main() 
{
    stdio_init_all();
    int rc = pico_led_init();
    hard_assert(rc == PICO_OK);
    pico_set_led(true);
    sleep_ms(LED_DELAY_MS);
    pico_set_led(false);

    ili9341_init();
    ts_spi_setup();
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN,GPIO_OUT);

    fill_screen(0x0000);
    struct repeating_timer timer;
    add_repeating_timer_ms(30, timer_ts_poll_callback, NULL, &timer);

    UG_Init(&gui, pixel_set, 240, 320);

    UG_DriverRegister( DRIVER_DRAW_LINE, (void*)_HW_DrawLine );
    UG_DriverRegister( DRIVER_FILL_FRAME, (void*)_HW_FillFrame );
    UG_DriverEnable( DRIVER_DRAW_LINE );
    UG_DriverEnable( DRIVER_FILL_FRAME );

    UG_FillScreen(C_BLUE);

    UG_WindowCreate( &window_1, obj_buff_wnd_1, MAX_OBJECTS, window_1_callback );
    UG_WindowSetTitleText( &window_1, "uGUI @ RPi PICO" );
    UG_WindowSetTitleTextFont( &window_1, &FONT_12X20 );

    UG_ButtonCreate( &window_1, &button1_1, BTN_ID_0, 10, 10, 110, 60 );
    UG_ButtonSetFont( &window_1, BTN_ID_0, &FONT_12X20 );
    UG_ButtonSetBackColor( &window_1, BTN_ID_0, C_LIME );
    UG_ButtonSetText( &window_1, BTN_ID_0, "Aus" );

    UG_ButtonCreate( &window_1, &button1_2, BTN_ID_1, 10, 80, 110, 130 );
    UG_ButtonSetFont( &window_1, BTN_ID_1, &FONT_12X20 );
    UG_ButtonSetBackColor( &window_1, BTN_ID_1, C_RED );
    UG_ButtonSetText( &window_1, BTN_ID_1, "OFF" );


    UG_WindowShow( &window_1 );
    UG_WaitForUpdate();

	while (1) {

    }
}
