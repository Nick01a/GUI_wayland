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
#include <wait.h>
#include "handlers/c_keyboard_event_handler.cpp"
struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_shell *shell;
struct wl_shell_surface *shell_surface;
struct wl_callback *frame_callback;
struct wl_seat *seat;
int pointer_x;
int pointer_y;
struct wl_keyboard *keyboard;
struct wl_pointer *pointer;
struct wl_shm* shm;

int is_minimized = 0;
int is_fullscreen = 0;
std::vector<CFrame> initial_frames;
std::vector<CButton> button_list;
std::vector<CFrame> frames;
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
    if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED && pointer_y < 30 && is_fullscreen == 0)
        wl_shell_surface_move(shell_surface,
                              seat, serial);
    if ((button == BTN_LEFT) && (pointer_x < 20) && (pointer_y < 30)) {
        frames.pop_back();
        exit(0);
    }
    else if ((button == BTN_LEFT) && (20<pointer_x) && (pointer_x < 40)&& (pointer_y < 30)){
        if (is_fullscreen == 0) {

            exit(2);
        }
        else {

            exit(3);
        }
    }
    else if ((button == BTN_LEFT) && (40<pointer_x) && (pointer_x < 60)&& (pointer_y < 30)){
        if (is_minimized == 0)
            exit(4);
        else
            exit(5);
    }
        for (auto &i : button_list) {
            if (((button == BTN_LEFT) && i.isInside(pointer_x,pointer_y))){
                i.action();
            }
        }
}

static void
pointer_handle_axis(void *data, struct wl_pointer *wl_pointer,
                    uint32_t time, uint32_t axis, wl_fixed_t value)
{
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
    frames.push_back(object);
    std::cerr<<object.getWidth()<<" "<<object.getHeight()<<std::endl;
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
        while ( wl_display_dispatch(display) != -1 ) {
        }
        wl_display_disconnect(display);
        printf("disconnected from display\n");

        exit(0);
}
void build_fork(CFrame frame){
    pid_t child = fork();
    if (child < 0){
        std::cerr<<"Error"<<std::endl;
        return;
    }
    if (child == 0){
        build(std::move(frame));
    }
    else {
        int status;
        waitpid(child,&status,0);
        if ( WIFEXITED(status) ) {
            int es = WEXITSTATUS(status);
            if (es == 1){
                exit(0);
            }
            if (es == 2){
                CFrame new_frame;
                new_frame.setTitle(initial_frames.back().getTitle());
                new_frame.setSize(1000,1000);
                for (auto &i : initial_frames.back().vector) {
                    new_frame.addLabel(i);
                }
                for (auto &i : initial_frames.back().buttons_list) {
                    new_frame.addButton(i);
                }
                for (auto &i : initial_frames.back().images) {
                    new_frame.addImage(i);
                }
                is_fullscreen = 1;
                build_fork(new_frame);
            }
            if (es == 3){
                CFrame new_frame;
                new_frame.setTitle(initial_frames.back().getTitle());
                new_frame.setSize(initial_frames.back().getHeight(),
                        initial_frames.back().getWidth());
                for (auto &i : initial_frames.back().vector) {
                    new_frame.addLabel(i);
                }
                for (auto &i : initial_frames.back().buttons_list) {
                    new_frame.addButton(i);
                }
                for (auto &i : initial_frames.back().images) {
                    new_frame.addImage(i);
                }
                is_fullscreen = 0;
                build_fork(new_frame);
            }
            if (es == 4){
                is_minimized = 1;
                CFrame new_frame;
                new_frame.setTitle(initial_frames.back().getTitle());
                new_frame.setSize(0,initial_frames.back().getWidth());
                build_fork(new_frame);
            }
            if (es == 5){
                is_minimized = 0;
                CFrame new_frame;
                new_frame.setTitle(initial_frames.back().getTitle());
                new_frame.setSize(initial_frames.back().getHeight(),
                                  initial_frames.back().getWidth());
                for (auto &i : initial_frames.back().vector) {
                    new_frame.addLabel(i);
                }
                for (auto &i : initial_frames.back().buttons_list) {
                    new_frame.addButton(i);
                }
                for (auto &i : initial_frames.back().images) {
                    new_frame.addImage(i);
                }
                is_fullscreen = 0;
                build_fork(new_frame);
            }
        }
    }
}
void exit_f(){
    exit(1);
}
void exit_w(){
    exit(0);
}
void print_hello() {
    CFrame cFrame;
    cFrame.setSize(100, 300);
    cFrame.setColor(Color::WHITE);
    cFrame.setTitle("Title!");
    initial_frames.push_back(cFrame);
    CLabel cLabel = CLabel("Are you sure that you\n   want to exit?");
    cLabel.setLocation(50,50);
    cFrame.addLabel(cLabel);
    CButton cButton_e = CButton("Yes",exit_f);
    CButton cButton_n = CButton("No",exit_w);
    cButton_e.setSize(50,50);
    cButton_n.setSize(50,50);
    cButton_e.setLocation(30,100);
    cButton_n.setLocation(80,100);
    cFrame.addButton(cButton_e);
    cFrame.addButton(cButton_n);
    button_list.push_back(cButton_e);
    button_list.push_back(cButton_n);
    build_fork(cFrame);
}
void print_help(){
    CFrame cFrame;
    cFrame.setSize(100, 300);
    cFrame.setColor(Color::WHITE);
    cFrame.setTitle("Help");
    initial_frames.push_back(cFrame);
    CLabel cLabel = CLabel("   The only help is:\nExit this as fast as you can ");
    CLabel one_more = CLabel("רוץ הכי מהר שאתה יכול");
    one_more.setLocation(50,100);
    cLabel.setLocation(50,50);
    cFrame.addLabel(cLabel);
    cFrame.addLabel(one_more);
    build_fork(cFrame);
}

int main() {
    CFrame cFrame;
    CImage cImage = CImage("../pictures/pikachu.png");
    cImage.setLocation(50,250);
    CLabel use_splash = CLabel("Pikachu use Splash");
    CLabel cLabel = CLabel("Charmander use Flamethrower");
    use_splash.setLocation(50,100);
    cLabel.setLocation(250,350);
    CImage charmander = CImage("../pictures/charmander.png");
    CImage fireball = CImage("../pictures/fireball.png");
    charmander.setLocation(250,100);
    fireball.setLocation(200,200);
    cFrame.setSize(400, 500);
    cFrame.setColor(Color::WHITE);
    cFrame.setTitle("Pokemon Attack");
    cFrame.addImage(cImage);
    CButton cButton = CButton(" Exit ",print_hello);
    CButton help = CButton(" Help ",print_help);
    cButton.setSize(50,30);
    cButton.setLocation(0,20);
    help.setSize(50,30);
    help.setLocation(35,20);
    cFrame.addButton(help);
    cFrame.addButton(cButton);
    cFrame.addImage(charmander);
    cFrame.addImage(fireball);
    cFrame.addLabel(cLabel);
    cFrame.addLabel(use_splash);
    button_list.push_back(help);
    button_list.push_back(cButton);
    initial_frames.push_back(cFrame);
    build_fork(cFrame);
}