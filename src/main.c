#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lvgl.h>

// Thread must have bigger size
#define DRAW_TRHEAD_STACK_SIZE 2048 
#define DRAW_TRHEAD_PRIORITY 0

struct display_buffer_descriptor descriptor;

extern void draw_loop(void *, void *, void *);

static const struct device *display = DEVICE_DT_GET(DT_NODELABEL(st7735s));

K_THREAD_DEFINE(draw_loop_tid, DRAW_TRHEAD_STACK_SIZE,
                draw_loop, NULL, NULL, NULL,
                DRAW_TRHEAD_PRIORITY, 0, 0);

static lv_obj_t *test_label;
static lv_disp_drv_t disp_drv;

int main(void)
{
    if(display == NULL) {
        printk("Device is null");
        return 1;
    }

    if(!device_is_ready(display)) {
        printk("Device not ready");
        return 1;
    }

	
	lv_disp_drv_init(&disp_drv);

	test_label = lv_label_create(lv_scr_act());

	static lv_style_t style;
	lv_color16_t redColor = lv_color_make(255,0,0);
	
    lv_style_init(&style);
    lv_style_set_text_font(&style, &lv_font_montserrat_32);
	lv_style_set_text_color(&style, redColor);
    lv_obj_add_style(test_label, &style, 0); 

	lv_obj_align(test_label, LV_ALIGN_CENTER, 0, 0);

	display_blanking_off(display);	
	
	// Event loop
	while (true) {
		lv_task_handler();
		k_sleep(K_MSEC(10));
	}

	return 0;
}

extern void draw_loop(void *, void *, void *) {
	int count = 0;
    while(true) {
	
		lv_label_set_text_fmt(test_label, "%d", count);

		count++;
		count = count % 100;
	
	    k_usleep(1000000);
	}
}

