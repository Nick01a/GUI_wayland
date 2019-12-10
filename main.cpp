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
#include "classes/cairo_handler.cpp"

struct wl_display *display = NULL;
struct wl_compositor *compositor = NULL;
struct wl_shell *shell;
struct wl_egl_window *egl_window;
struct wl_shell_surface *shell_surface;
struct wl_callback *frame_callback;
EGLDisplay egl_display;
EGLConfig egl_conf;
EGLSurface egl_surface;
EGLContext egl_context;
CairoHandler cairoHandler;
void print_borders(int w,int h){
    const GLchar *vShaderStr =
            "attribute vec4 vPosition;    \n"
            "void main()                  \n"
            "{                            \n"
            "   gl_Position = vPosition;  \n"
            "}                            \n";

    const GLchar *fShaderStr =
            "precision mediump float;\n"
            "void main()                                  \n"
            "{                                            \n"
            "  gl_FragColor = vec4 ( 0.0, 0.0, 0.0, 0.0 );\n"
            "}                                            \n";

    GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader2, 1, &vShaderStr, NULL);
    glCompileShader(vertexShader2);
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fShaderStr, NULL);
    glCompileShader(fragmentShader2);
    GLuint shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader2);
    glAttachShader(shaderProgram2, fragmentShader2);
    glBindAttribLocation(shaderProgram2, 0, "vPosition");

    // Link the program
    glLinkProgram(shaderProgram2);
    GLfloat gVertices[] ={
            -1.0f + 2.0f / w,  1.0f - 2.0f / h, 1.0f, 0.0f, 0.0f, // Top-left
            1.0f - 2.0f / w ,  1.0f - 2.0f / h, 0.0f, 1.0f, 0.0f, // Top-right
            1.0f - 2.0f/w, -1.0f + 2.0f / h, 0.0f, 0.0f, 1.0f, // Bottom-right
            -1.0f + 2.0f / w, -1.0f + 2.0f / h, 1.0f, 1.0f, 1.0f  // Bottom-left
    };
    GLushort indices[] = {0, 1, 2, 2, 3, 0};
    glUseProgram(shaderProgram2);
    glLineWidth(2);
    glVertexAttribPointer(0, 6, GL_FLOAT, GL_FALSE, 0, gVertices);
    glEnableVertexAttribArray(0);
    glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_SHORT, indices);

    glDeleteShader(vertexShader2);
    glDeleteShader(fragmentShader2);
    glDeleteProgram(shaderProgram2);
    glFlush();
}
void print_triangle(int w,int h){
        const GLchar *vShaderStr =
                "attribute vec4 vPosition;    \n"
                "void main()                  \n"
                "{                            \n"
                "   gl_Position = vPosition;  \n"
                "}                            \n";

        const GLchar *fShaderStr =
                "precision mediump float;\n"
                "void main()                                  \n"
                "{                                            \n"
                "  gl_FragColor = vec4 ( 0.0, 0.0, 0.0, 0.0 );\n"
                "}                                            \n";

        GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader2, 1, &vShaderStr, NULL);
        glCompileShader(vertexShader2);
        GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader2, 1, &fShaderStr, NULL);
        glCompileShader(fragmentShader2);
        GLuint shaderProgram2 = glCreateProgram();
        glAttachShader(shaderProgram2, vertexShader2);
        glAttachShader(shaderProgram2, fragmentShader2);
        glBindAttribLocation(shaderProgram2, 0, "vPosition");

        // Link the program
        glLinkProgram(shaderProgram2);
        GLfloat vVertices[] = {
            -1.0f + 2.0f / w,  1.0f - 2.0f / h, 1.0f, 0.0f, 0.0f, // Top-left
            1.0f ,  1.0f - 2.0f / h, 0.0f, 1.0f, 0.0f, // Top-right
            1.0f, 1.0f - 40.0f / h, 0.0f, 0.0f, 1.0f, // Bottom-right
            -1.0f + 2.0f / w, 1.0f - 40.0f / h, 1.0f, 1.0f, 1.0f,  // Bottom-left
    };
        GLushort indices[] = {0, 1, 2, 2, 3, 0,5,4,1};
        glUseProgram(shaderProgram2);
        glLineWidth(2);
        glVertexAttribPointer(0, 6, GL_FLOAT, GL_FALSE, 0, vVertices);
        glEnableVertexAttribArray(0);
        glDrawElements(GL_LINE_LOOP, 6, GL_UNSIGNED_SHORT, indices);

        glDeleteShader(vertexShader2);
        glDeleteShader(fragmentShader2);
        glDeleteProgram(shaderProgram2);
        glFlush();
    }
static void
global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                        const char *interface, uint32_t version) {
    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, wl_compositor_interface.name) == 0) { // "wl_compositor"
        compositor = static_cast<wl_compositor *>(wl_registry_bind(registry, id,
                                                                   &wl_compositor_interface, 1));
    } else if (strcmp(interface, wl_shell_interface.name) == 0) { // "wl_shell"
        shell = static_cast<wl_shell *>(wl_registry_bind(registry, id,
                                                         &wl_shell_interface, 1));
//    } else if (strcmp(interface, wl_shm_interface.name) == 0) { // "wl_shm"
//        shm = static_cast<wl_shm *>(wl_registry_bind(registry, id,
//                                                     &wl_shm_interface, 1));
//    }
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

const GLchar *vShaderStr =
        "#version 300 es                            \n"
        "layout(location = 0) in vec4 a_position;   \n"
        "layout(location = 1) in vec2 a_texCoord;   \n"
        "out vec2 v_texCoord;                       \n"
        "void main()                                \n"
        "{                                          \n"
        "   gl_Position = a_position;               \n"
        "   v_texCoord = a_texCoord;                \n"
        "}                                          \n";

const GLchar *fShaderStr =
        "#version 300 es                                     \n"
        "precision mediump float;                            \n"
        "in vec2 v_texCoord;                                 \n"
        "layout(location = 0) out vec4 outColor;             \n"
        "uniform sampler2D s_texture;                        \n"
        "void main()                                         \n"
        "{                                                   \n"
        "  outColor = texture( s_texture, v_texCoord );      \n"
        "}                                                   \n";

void print_title(char *title, int w, int h) {
    unsigned int texture_id;
    int text_width = 0;
    int text_height = 0;
    GLuint vertexShader2 = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader2, 1, &vShaderStr, NULL);
    glCompileShader(vertexShader2);
    GLuint fragmentShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader2, 1, &fShaderStr, NULL);
    glCompileShader(fragmentShader2);
    GLuint shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vertexShader2);
    glAttachShader(shaderProgram2, fragmentShader2);
    glLinkProgram(shaderProgram2);
    glUseProgram(shaderProgram2);

    cairoHandler.render_text(title,
                             &text_width,
                             &text_height,
                             &texture_id);

    printf("Text w: %i, h: %i\n", text_width, text_height);

    GLfloat vVertices[] = {-1.0f + 4.0f / w, 1.0f, -1.0f,  // Position 0
                           0.0f, 0.0f,        // TexCoord 0
                           -1.0f + 4.0f / w, 1.0f - 40.0f/h, -1.0f,  // Position 1
                           0.0f, 1,        // TexCoord 1
                           -1.0f + text_width / (float) w, 1.0f - 40.0f / h, -0.7f,  // Position 2
                           1, 1,        // TexCoord 2
                           -1.0f + text_width / (float) w, 1.0f, -0.7f,  // Position 3
                           1, 0.0f,           // TexCoord 3
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};
    // Load the vertex position
    glVertexAttribPointer(0, 3, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), vVertices);
    // Load the texture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT,
                          GL_FALSE, 5 * sizeof(GLfloat), &vVertices[3]);

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // Bind the texture
    glActiveTexture(GL_TEXTURE0);

    // Get the sampler location
    GLint samplerLoc = glGetUniformLocation(shaderProgram2, "s_texture");
    glUniform1i(samplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
    glDeleteShader(vertexShader2);
    glDeleteShader(fragmentShader2);
    glDeleteProgram(shaderProgram2);
    glFlush();

}

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
        print_title(object.getCharTitle(), object.getWidth(), object.getHeight());
        print_triangle(object.getWidth(), object.getHeight());
        print_borders(object.getWidth(),object.getHeight());
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
    cgObject.setSize(400, 500);
    cgObject.setColor(Color::WHITE);
    cgObject.setTitle("Hello wotlf");
    std::vector<CFrame> items;
    items.push_back(cgObject);
    build(items);

}