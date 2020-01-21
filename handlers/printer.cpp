#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include "cairo_handler.cpp"
struct PrinterHandler{
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



    void print_title(char *title, int w, int h, int x, int y) {
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

        GLfloat vVertices[] = {-1.0f + (float) 2*x / w + 4.0f / w, 1.0f- (float) 2*y / h, -1.0f,  // Position 0
                               0.0f, 0.0f,        // TexCoord 0
                               -1.0f +(float) 2*x / w+ 4.0f / w, 1.0f - (float) 2*y / h- 40.0f/h, -1.0f,  // Position 1
                               0.0f, 1,        // TexCoord 1
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f - (float) 2*y / h- 40.0f / h, -0.7f,  // Position 2
                               1, 1,        // TexCoord 2
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f- (float) 2*y / h, -0.7f,  // Position 3
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
    void print_button(char *filename, int w, int h,int x,int y) {
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

        cairoHandler.render_button(filename,
                                  &text_width,
                                  &text_height,
                                  &texture_id);

        GLfloat vVertices[] = {-1.0f + (float) 2*x / w + 4.0f / w, 1.0f- (float) 2*y / h, -1.0f,  // Position 0
                               0.0f, 0.0f,        // TexCoord 0
                               -1.0f +(float) 2*x / w+ 4.0f / w, 1.0f - (float) 2*y / h- (float)text_height/h, -1.0f,  // Position 1
                               0.0f, 1,        // TexCoord 1
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f - (float) 2*y / h- (float)text_height / h, -0.7f,  // Position 2
                               1, 1,        // TexCoord 2
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f- (float) 2*y / h, -0.7f,  // Position 3
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
    void print_image(char *filename, int w, int h,int x,int y) {
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

        cairoHandler.render_image(filename,
                                  &text_width,
                                  &text_height,
                                  &texture_id);

        GLfloat vVertices[] = {-1.0f + (float) 2*x / w + 4.0f / w, 1.0f- (float) 2*y / h, -1.0f,  // Position 0
                               0.0f, 0.0f,        // TexCoord 0
                               -1.0f +(float) 2*x / w+ 4.0f / w, 1.0f - (float) 2*y / h- (float)text_height/h, -1.0f,  // Position 1
                               0.0f, 1,        // TexCoord 1
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f - (float) 2*y / h- (float)text_height / h, -0.7f,  // Position 2
                               1, 1,        // TexCoord 2
                               -1.0f +(float) 2*x / w+ text_width / (float) w, 1.0f- (float) 2*y / h, -0.7f,  // Position 3
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
};