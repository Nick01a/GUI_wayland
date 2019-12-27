//
// Created by yura on 12/3/19.
//

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
#include "handlers/printer.cpp"

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_shell *shell;
struct wl_egl_window *egl_window;
struct wl_shell_surface *shell_surface;
struct wl_callback *frame_callback;
struct wl_seat *seat;
int pointer_x;
int pointer_y;
int window_w;
int window_h;
struct wl_keyboard *keyboard;
struct wl_pointer *pointer;
struct wl_shm* shm;
PrinterHandler printerHandler;
EGLDisplay egl_display;
EGLConfig egl_conf;
EGLSurface egl_surface;
EGLContext egl_context;

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
        wl_display_disconnect(display);
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

static const struct wl_keyboard_listener keyboard_listener = {
        keyboard_handle_keymap,
        keyboard_handle_enter,
        keyboard_handle_leave,
        keyboard_handle_key,
        keyboard_handle_modifiers,
};
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
    if ((button == BTN_LEFT) && (pointer_x > window_w - 30) && (pointer_y < 30)) {
        printf("disconnected from display\n");
        exit(0);
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
init_egl() {
    EGLint major, minor, count, n, size;
    EGLConfig *configs;
    int i;
    EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };


    egl_display = eglGetDisplay((EGLNativeDisplayType) display);
    if (egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Can't create egl display\n");
        exit(1);
    } else {
        fprintf(stderr, "Created egl display\n");
    }

    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        fprintf(stderr, "Can't initialise egl display\n");
        exit(1);
    }
    printf("EGL major: %d, minor %d\n", major, minor);

    eglGetConfigs(egl_display, NULL, 0, &count);
    printf("EGL has %d configs\n", count);

    EGLint numConfigs = -1;
    eglGetConfigs(egl_display, NULL, 0, &numConfigs);
    configs = new EGLConfig[numConfigs];
    eglChooseConfig(egl_display, config_attribs,
                    configs, count, &n);

    for (i = 0; i < n; i++) {
        eglGetConfigAttrib(egl_display,
                           configs[i], EGL_BUFFER_SIZE, &size);
        printf("Buffer size for config %d is %d\n", i, size);
        eglGetConfigAttrib(egl_display,
                           configs[i], EGL_RED_SIZE, &size);
        printf("Red size for config %d is %d\n", i, size);

        // just choose the first one
        egl_conf = configs[i];
        break;
    }

    egl_context =
            eglCreateContext(egl_display,
                             egl_conf,
                             EGL_NO_CONTEXT, context_attribs);
}


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
void build(std::vector<CFrame> items) {
    display = wl_display_connect(NULL);
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
    init_egl();
    for (CFrame object : items) {

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
        egl_window = wl_egl_window_create(object.surface, object.getWidth(), object.getHeight());
        egl_surface = eglCreateWindowSurface(egl_display, egl_conf, (NativeWindowType) egl_window, NULL);
        eglMakeCurrent(egl_display, egl_surface,
                       egl_surface, egl_context);

        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        window_w = object.getWidth();
        window_h = object.getHeight();
        for (int i = 0; i < object.vector.size(); ++i) {
            printerHandler.print_title(object.vector[i].getCharText(),object.getWidth(),object.getHeight(),
                    object.vector[i].x,object.vector[i].y);
        }
        printerHandler.print_image(object.cImage.getCharText(),object.getWidth(),object.getHeight(),
                    object.cImage.x,object.cImage.y);
        for (int i = 0; i < object.images.size(); ++i) {
            printerHandler.print_image(object.images[i].getCharText(),object.getWidth(),object.getHeight(),
                        object.images[i].x,object.images[i].y);
        }
        printerHandler.print_title(object.getCharTitle(), object.getWidth(), object.getHeight(),0,0);
        printerHandler.print_triangle(object.getWidth(), object.getHeight());
        printerHandler.print_borders(object.getWidth(),object.getHeight());
        if (eglSwapBuffers(egl_display, egl_surface))
            fprintf(stderr, "Swapped buffers\n");
        while ( wl_display_dispatch(display) != -1 ) {
        }
        wl_display_disconnect(display);
        printf("disconnected from display\n");

        exit(0);
    }
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
    std::vector<CFrame> items;
    items.push_back(cgObject);
    build(items);
}