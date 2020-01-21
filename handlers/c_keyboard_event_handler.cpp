#include <stdint.h>
#include <iostream>
struct CKeyboardEventHandler{
    static void
    keyboard_handle_keymap(void *data, struct wl_keyboard *keyboard,
                           uint32_t format, int fd, uint32_t size)
    {
    }

    static void
    keyboard_handle_enter(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, struct wl_surface *surface,
                          struct wl_array *keys)
    {
        fprintf(stderr, "Keyboard gained focus\n");
    }

    static void
    keyboard_handle_leave(void *data, struct wl_keyboard *keyboard,
                          uint32_t serial, struct wl_surface *surface)
    {
        fprintf(stderr, "Keyboard lost focus\n");
    }

    static void
    keyboard_handle_key(void *data, struct wl_keyboard *keyboard,
                        uint32_t serial, uint32_t time, uint32_t key,
                        uint32_t state)
    {
        if (key == 0x01) {
            printf("disconnected from display\n");
            exit(0);
        }
    }


    static void
    keyboard_handle_modifiers(void *data, struct wl_keyboard *keyboard,
                              uint32_t serial, uint32_t mods_depressed,
                              uint32_t mods_latched, uint32_t mods_locked,
                              uint32_t group)
    {
    }
};