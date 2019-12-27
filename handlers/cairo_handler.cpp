#include <pango/pangocairo.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <stdlib.h>
struct CairoHandler {


    cairo_t*
    create_layout_context ()
    {
        cairo_surface_t *temp_surface;
        cairo_t *context;

        temp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, 0, 0);
        context = cairo_create (temp_surface);
        cairo_surface_destroy (temp_surface);

        return context;
    }
    unsigned int
    create_texture (int width,
                    int height,
                    unsigned char *pixels)
    {
        unsigned int texture_id;

        glGenTextures (1, &texture_id);
        glBindTexture (GL_TEXTURE_2D, texture_id);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D (GL_TEXTURE_2D,
                      0,
                      GL_RGBA,
                      width,
                      height,
                      0,
                      GL_RGBA,
                      GL_UNSIGNED_BYTE,
                      pixels);

        return texture_id;
    }
    cairo_t*
    create_cairo_context (int width,
                          int height,
                          int channels,
                          cairo_surface_t** surf,
                          unsigned char** buffer)
    {
        *buffer = static_cast<unsigned char *>(calloc (channels * width * height, sizeof (unsigned char)));
        *surf = cairo_image_surface_create_for_data (*buffer,
                                                     CAIRO_FORMAT_ARGB32,
                                                     width,
                                                     height,
                                                     channels * width);
        return cairo_create (*surf);
    }
    void
    get_text_size (PangoLayout *layout,
                   int* width,
                   int* height)
    {
        pango_layout_get_size(layout,width,height);
        *width /= PANGO_SCALE;
        *height /= PANGO_SCALE;
    }
    unsigned int
    render_image(const char *filename,
                int* image_widht,
                int* image_height,
                unsigned int *texture_id){
        cairo_surface_t *image;
        cairo_t *render_context;
        cairo_surface_t *temp_surface;
        cairo_surface_t *surface;
        unsigned char* surface_data = NULL;
        image = cairo_image_surface_create_from_png (filename);
        *image_height = cairo_image_surface_get_height(image);
        *image_widht = cairo_image_surface_get_width(image);
        render_context = create_cairo_context (*image_widht,
                                               *image_height,
                                               4,
                                               &surface,
                                               &surface_data);

        cairo_set_source_rgba(render_context,1,1,1,1);
        cairo_paint(render_context);
        cairo_set_source_surface (render_context,image, 0, 0);
        cairo_paint(render_context);
        *texture_id = create_texture(*image_widht, *image_height, surface_data);

        /* Clean up */
        free (surface_data);
        cairo_destroy (render_context);
        cairo_surface_destroy (surface);

    }
    unsigned int
    render_text (const char *text,
                 int *text_width,
                 int *text_height,
                 unsigned int *texture_id)
    {
        cairo_t *layout_context;
        cairo_t *render_context;
        cairo_surface_t *temp_surface;
        cairo_surface_t *surface;
        unsigned char* surface_data = NULL;
        PangoFontDescription *desc;
        PangoLayout *layout;

        layout_context = create_layout_context ();
        /* Create a PangoLayout, set the font and text */
        layout = pango_cairo_create_layout (layout_context);
        pango_layout_set_text (layout, text, -1);

        /* Load the font */
        desc = pango_font_description_from_string ("Arial 24");
        if(!desc){
            printf("Pango font not found.\n");
        }
        pango_layout_set_font_description (layout, desc);
        pango_font_description_free (desc);

        /* Get text dimensions and create a context to render to */
        get_text_size (layout, text_width, text_height);
        render_context = create_cairo_context (*text_width,
                                               *text_height,
                                               4,
                                               &surface,
                                               &surface_data);

        /* Render */
        cairo_set_source_rgba(render_context,1,1,1,1);
        cairo_paint(render_context);
        cairo_set_source_rgba (render_context, 0, 0, 0, 1);
        pango_cairo_show_layout (render_context, layout);
        *texture_id = create_texture(*text_width, *text_height, surface_data);

        /* Clean up */
        free (surface_data);
        //g_object_unref (layout);
        cairo_destroy (layout_context);
        cairo_destroy (render_context);
        cairo_surface_destroy (surface);
    }
};