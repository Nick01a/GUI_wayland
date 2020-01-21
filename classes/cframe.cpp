#include <utility>
#include <GLES2/gl2.h>
#include <wayland-egl.h>
#include "cbutton.cpp"
#include "clabel.cpp"
#include "cimage.cpp"
#include "../handlers/c_keyboard_event_handler.cpp"
#include "../handlers/printer.cpp"
struct CFrame : CGObject{
    std::string title;
    std::vector<CLabel> vector;
    std::vector<CImage> images;
    std::vector<CButton> buttons_list;
    PrinterHandler printerHandler;
    EGLDisplay egl_display;
    EGLConfig egl_conf;
    EGLSurface egl_surface;
    EGLContext egl_context;
    struct wl_egl_window *egl_window;
    CImage cImage = CImage("exit.png");
    void setTitle(std::string title){
        this->title = std::move(title);
    }
    void setSize(int h,int w){
        this->HEIGHT=h+30;
        this->WIDTH =w;
        this->cImage.setLocation(4,4);
    }
    std::string getTitle(){
        return this->title;
    }
    char* getCharTitle() {
        return const_cast<char *>(this->title.c_str());
    }
    void addLabel(CLabel &c){
        this->vector.push_back(c);
    }
    void addImage(CImage &c){
        this->images.push_back(c);
    }
    void addButton(CButton &c){
        this->buttons_list.push_back(c);
    }
    void
    init_egl(wl_display *display) {
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
        eglSwapInterval(egl_display, EGL_MIN_SWAP_INTERVAL);
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
    void paint(){
        egl_window = wl_egl_window_create(surface, getWidth(), getHeight());
        egl_surface = eglCreateWindowSurface(egl_display, egl_conf, (NativeWindowType) egl_window, NULL);
        eglMakeCurrent(egl_display, egl_surface,
                       egl_surface, egl_context);
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        for (auto &i : vector) {
            printerHandler.print_title(i.getCharText(),getWidth(),getHeight(),
                                       i.x, i.y);
        }
        printerHandler.print_image(cImage.getCharText(),getWidth(),getHeight(),
                                   cImage.x,cImage.y);
        for (auto &image : images) {
            printerHandler.print_image(image.getCharText(),getWidth(),getHeight(),
                                       image.x, image.y);
        }
        for (auto &button : buttons_list) {
            printerHandler.print_title(button.getCharTitle(),getWidth(),getHeight(),
                                       button.x, button.y);
        }
        printerHandler.print_title(getCharTitle(), getWidth(), getHeight(),100,0);
        printerHandler.print_triangle(getWidth(), getHeight());
        printerHandler.print_borders(getWidth(),getHeight());
        if (eglSwapBuffers(egl_display, egl_surface))
            fprintf(stderr, "Swapped buffers\n");
    }


};