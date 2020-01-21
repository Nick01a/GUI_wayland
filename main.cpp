#include <utility>
#include "classes/cframe.cpp"
#include <EGL/egl.h>
#include <wayland-egl.h>
#include <pango/pangocairo.h>
#include <wayland-egl-backend.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-server.h>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>
#include <linux/input-event-codes.h>
#include <zconf.h>
#include "xdg-shell-client-protocol.h"

//struct xdg_wm_base *wm_base = NULL;
struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_shell *shell;
//struct xdg_shell *shell;
//struct wl_shell_surface *shell_surface;
struct xdg_surface *shell_surface;
struct wl_callback *frame_callback;
struct wl_seat *seat;
int pointer_x;
int pointer_y;
struct wl_keyboard *keyboard;
struct wl_pointer *pointer;
struct wl_shm* shm;
std::vector<CButton> buttons;

static void
pointer_handle_enter(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface,
                     wl_fixed_t sx, wl_fixed_t sy)
{
}

static void
pointer_handle_leave(void *data, struct wl_pointer *pointer,
                     uint32_t serial, struct wl_surface *surface)
{
}

static void
pointer_handle_motion(void *data, struct wl_pointer *pointer,
                      uint32_t time, wl_fixed_t sx, wl_fixed_t sy)
{
    pointer_x = wl_fixed_to_int(sx);
    pointer_y = wl_fixed_to_int(sy);
}

static void
pointer_handle_button(void *data, struct wl_pointer *wl_pointer,
                      uint32_t serial, uint32_t time, uint32_t button,
                      uint32_t state)
{
    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
        wl_shell_surface_move(shell_surface,
                              seat, serial);
    if ((button == BTN_LEFT) && (pointer_x < 30) && (pointer_y < 30)) {
        exit(0);
    }
    for (auto &i : buttons) {
        if (((button == BTN_LEFT) && i.isInside(pointer_x,pointer_y))){
            i.action();
        }
    }
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
    printf("Pointer handle axis\n");
}
static const struct wl_pointer_listener pointer_listener = {
        pointer_handle_enter,
        pointer_handle_leave,
        pointer_handle_motion,
        pointer_handle_button,
        pointer_handle_axis
};


static const struct wl_keyboard_listener keyboard_listener = {
        CKeyboardEventHandler::keyboard_handle_keymap,
        CKeyboardEventHandler::keyboard_handle_enter,
        CKeyboardEventHandler::keyboard_handle_leave,
        CKeyboardEventHandler::keyboard_handle_key,
        CKeyboardEventHandler::keyboard_handle_modifiers,
};
static void
seat_handle_capabilities(void *data, struct wl_seat *seat,
                         uint32_t caps)
{
    if (caps & WL_SEAT_CAPABILITY_POINTER){
        pointer = wl_seat_get_pointer(seat);
        wl_pointer_add_listener(pointer,&pointer_listener,NULL);
    }
    if (caps & WL_SEAT_CAPABILITY_KEYBOARD) {
        keyboard = wl_seat_get_keyboard(seat);
        wl_keyboard_add_listener(keyboard, &keyboard_listener, NULL);
    } else if (!(caps & WL_SEAT_CAPABILITY_KEYBOARD)) {
        wl_keyboard_destroy(keyboard);
        keyboard = NULL;
    }


}

static const struct wl_seat_listener seat_listener = {
        static_cast<void (*)(void *, wl_seat *, uint32_t)>(seat_handle_capabilities),
};
void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                        const char *interface, uint32_t version) {
    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = static_cast<wl_compositor *>(wl_registry_bind(registry,
                                                                   id,
                                                                   &wl_compositor_interface,
                                                                   1));
    } else if (strcmp(interface, "wl_shell") == 0) {
        shell = static_cast<wl_shell *>(wl_registry_bind(registry, id,
                                                         &wl_shell_interface, 1));

    } else if (strcmp(interface, "wl_seat") == 0) {
        seat = static_cast<wl_seat *>(wl_registry_bind(registry, id,
                                                       &wl_seat_interface, 1));
        wl_seat_add_listener(seat, &seat_listener, NULL);
    } else if (strcmp(interface, "wl_shm") == 0) {
        shm = static_cast<wl_shm *>(wl_registry_bind(registry, id,
                                                     &wl_shm_interface, 1));
    }
}
static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id) {
    printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
        global_registry_handler,
        global_registry_remover
};


static void
handle_ping(void *data, struct wl_shell_surface *shell_surface,
            uint32_t serial)
{
    wl_shell_surface_pong(shell_surface, serial);
    fprintf(stderr, "Pinged and ponged\n");
}

static void
handle_configure(void *data, struct wl_shell_surface *shell_surface,
                 uint32_t edges, int32_t width, int32_t height)
{
}

static void
handle_popup_done(void *data, struct wl_shell_surface *shell_surface)
{
}

static const struct wl_shell_surface_listener shell_surface_listener = {
        handle_ping,
        handle_configure,
        handle_popup_done
};


void build(CFrame object) {
    display = wl_display_connect(NULL);
//    display = xdg_wm_base_connect(NULL);
    if (display == NULL) {
        fprintf(stderr, "Can't connect to display\n");
        exit(1);
    }
    printf("connected to display\n");

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);
    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (compositor == nullptr) {
        fprintf(stderr, "Can't find compositor\n");
        exit(1);
    } else {
        fprintf(stderr, "Found compositor\n");
    }
    object.init_egl(display);
        object.surface = wl_compositor_create_surface(compositor);
        if (object.surface == nullptr) {
            fprintf(stderr, "Can't create surface\n");
            exit(1);
        } else {
            fprintf(stderr, "Created surface\n");
        }

        shell_surface = wl_shell_get_shell_surface(shell, object.surface);
        wl_shell_surface_add_listener(shell_surface,&shell_surface_listener,NULL);
        if (shell_surface == nullptr) {
            fprintf(stderr, "Can't create shell surface\n");
            exit(1);
        } else {
            fprintf(stderr, "Created shell surface\n");
        }
        wl_shell_surface_set_toplevel(shell_surface);
        frame_callback = wl_surface_frame(object.surface);


        object.paint();
        wl_surface_commit(object.surface);
        while ( wl_display_dispatch(display) != -1 ) {
        }
        wl_display_disconnect(display);
        printf("disconnected from display\n");

        exit(0);
}
void print_hello() {
    CFrame cgObject;
    cgObject.setSize(100, 100);
    cgObject.setColor(Color::WHITE);
    cgObject.setTitle("Title!");
    build(cgObject);
}
int main() {
    CFrame cgObject;
    CImage cImage = CImage("test.png");
    cImage.setLocation(100,100);
    CLabel cLabel = CLabel("Label Here!");
    cLabel.setLocation(300,400);
    cgObject.setSize(400, 500);
    cgObject.setColor(Color::WHITE);
    cgObject.setTitle("Title Here!");
    cgObject.addLabel(cLabel);
    cgObject.addImage(cImage);
    CButton cButton = CButton("Click me",print_hello);
    cButton.setSize(100,100);
    cButton.setLocation(50,50);
    cgObject.addButton(cButton);
    buttons.push_back(cButton);
    build(cgObject);
    usleep(1000);
}





