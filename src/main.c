#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/display.h>
#include <zephyr/device.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DRAW_TRHEAD_STACK_SIZE 512
#define DRAW_TRHEAD_PRIORITY 0

struct display_buffer_descriptor descriptor;

int count = 0;
int increment = 1;
uint16_t* buf;
extern void draw_loop(void *, void *, void *);
void DrawLine(uint16_t* buffer, int x0, int y0, int x1, int y1, uint16_t color, int display_pitch);

static const struct device *display = DEVICE_DT_GET(DT_NODELABEL(st7735s));

K_THREAD_DEFINE(draw_loop_tid, DRAW_TRHEAD_STACK_SIZE,
                draw_loop, NULL, NULL, NULL,
                DRAW_TRHEAD_PRIORITY, 0, 0);

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

    struct display_capabilities capabilites;
    display_get_capabilities(display, &capabilites);

    printk("Width: %d\n", capabilites.x_resolution);
    printk("Height: %d\n", capabilites.y_resolution);
    printk("Screen info: %d\n", capabilites.screen_info);
    printk("Current pixel format: %d\n", capabilites.current_pixel_format);
    printk("Supporrted pixel formats: %d\n", capabilites.supported_pixel_formats);

    descriptor.width = capabilites.x_resolution;
    descriptor.height = capabilites.y_resolution;
    descriptor.pitch = capabilites.x_resolution;
    descriptor.buf_size = capabilites.x_resolution * capabilites.y_resolution * sizeof(uint16_t);

    printk("Buf size: %d\n", descriptor.buf_size);

    buf = (uint16_t*)malloc(descriptor.buf_size);

	return 0;
}

extern void draw_loop(void *, void *, void *) {
   
    while(true) {

    memset(buf, 0x1F00, descriptor.buf_size);
   
    DrawLine(buf, 0 + count, 0 ,80 - count, 160,
     0x001F, descriptor.pitch);
    
    display_write(display, 0, 0, &descriptor, buf);
    
    count += increment;
    if(count == 80) {
        increment = -1;
    }
    if(count == 0) {
        increment = 1;
    }

    // 1000000 = 1s
    k_usleep(33333); // 30 fps
    }
}

void DrawLine(uint16_t* buffer, int x0, int y0, int x1, int y1, uint16_t color, int display_pitch) {
    
    int dx, dy, x, y, incr1, incr2, xend,yend, d;
	int incx, incy;
	dx = abs(x1 - x0);
	dy = abs(y1 - y0);

	if (dx != 0 && (float)dy / (float)dx < 1.f) {
		d = 2 * dy - dx;
		incr1 = 2 * dy;
		incr2 = 2 * (dy - dx);

		x = x0 > x1 ? x1 : x0;
		y = x0 > x1 ? y1 : y0;
		xend = x0 > x1 ? x0 : x1;
		yend = x0 > x1 ? y0 : y1;

		incy = y - yend > 0 ? -1 : 1;

		while (x <= xend) {
			buffer[x * display_pitch + y] = color;
			x = x + 1;
			if (d < 0) 
				d += incr1;
			else {
				y = y + incy;
				d += incr2;
			}
		}
	}
	else {
		d = 2 * dx - dy;
		incr1 = 2 * dx;
		incr2 = 2 * (dx - dy);


		y = y0 > y1 ? y1 : y0;
		yend = y0 > y1 ? y0 : y1;
		x = y0 > y1 ? x1 : x0;
		xend = y0 > y1 ? x0 : x1;

		incx = x - xend > 0 ? -1 : 1;


		while (y <= yend) {
			buffer[x * display_pitch + y] = color;
			y = y + 1;
			if (d < 0)
				d += incr1;
			else {
				x = x + incx;
				d += incr2;
			}
		}
	}
}
