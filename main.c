#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static const char *getenv_default(const char *name, const char *dflt)
{
    return getenv(name) ? : dflt;
}

#if LV_USE_EVDEV
static void lv_linux_init_input_pointer(lv_display_t *disp)
{
    // Enables a pointer (touchscreen/mouse) input device
    // Use 'evtest' to find the correct input device. /dev/input/by-id/ is recommeded if possible
    // Use /dev/input/by-id/my-mouse-or-touchscreen or /dev/input/eventX
    const char *input_device = getenv_default("LV_LINUX_EVDEV_POINTER_DEVICE", "/dev/input/by-id/my-mouse-or-touchscreen");
    lv_indev_t *touch = lv_evdev_create(LV_INDEV_TYPE_POINTER, input_device); 
    lv_indev_set_display(touch, disp);

    // Disable this if you want no cursor
    LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image source.*/
    lv_obj_t * cursor_obj = lv_image_create(lv_screen_active()); /*Create an image object for the cursor */
    lv_image_set_src(cursor_obj, &mouse_cursor_icon); /*Set the image source*/
    lv_indev_set_cursor(touch, cursor_obj); /*Connect the image  object to the driver*/
}
#endif

#if LV_USE_LINUX_FBDEV
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_FBDEV_DEVICE", "/dev/fb0");
    lv_display_t * disp = lv_linux_fbdev_create();
    
    #if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
    #endif
    
    lv_linux_fbdev_set_file(disp, device);
}
#elif LV_USE_LINUX_DRM
static void lv_linux_disp_init(void)
{
    const char *device = getenv_default("LV_LINUX_DRM_CARD", "/dev/dri/card0");
    lv_display_t * disp = lv_linux_drm_create();

    #if LV_USE_EVDEV
    lv_linux_init_input_pointer(disp);
    #endif

    lv_linux_drm_set_file(disp, device, -1);
}
#elif LV_USE_SDL
static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "480");

    lv_sdl_window_create(width, height);
}
#elif LV_USE_X11

static void linux_x11_on_close_callback(lv_event_t * e)
{
    /* Do nothing here */
}

static void lv_linux_disp_init(void)
{
    const int width = atoi(getenv("LV_SDL_VIDEO_WIDTH") ? : "800");
    const int height = atoi(getenv("LV_SDL_VIDEO_HEIGHT") ? : "480");

    lv_display_t * disp = lv_x11_window_create("LVGL X11 Simulation", width, height);
    lv_display_add_event_cb(disp, linux_x11_on_close_callback, LV_EVENT_DELETE, disp);

    LV_IMAGE_DECLARE(mouse_cursor_icon); /*Declare the image source.*/
    lv_x11_inputs_create(disp, &mouse_cursor_icon);
}
#else
#error Unsupported configuration
#endif

int main(char **argv, int argc)
{
    lv_init();

    /*Linux display device init*/
    lv_linux_disp_init();

    /*Create a Demo*/
    // lv_demo_widgets();
    // lv_demo_widgets_start_slideshow();

    lv_demo_music();

    // lv_demo_keypad_encoder();
    // lv_demo_benchmark();
    // lv_demo_flex_layout();
    // lv_demo_multilang();
    // lv_demo_scroll();
    // lv_demo_transform();
    // lv_demo_stress();
    // lv_demos_show_help();



    /*Handle LVGL tasks*/
    while(1) {
        uint32_t idle_time = lv_timer_handler(); /*Returns the time to the next timer execution*/
        usleep(idle_time * 1000);
    }

    return 0;
}
