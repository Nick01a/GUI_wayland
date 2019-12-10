#include <utility>
#include "handler.cpp"
#include <iostream>
#include <vector>
#include "color.cpp"

struct CGObject {
    struct wl_surface *surface;
    struct wl_buffer *buffer;
    int HEIGHT;
    int WIDTH;
    uint32_t color;

    void setSize(int h,int w){
        this->WIDTH = w;
        this->HEIGHT = h + 30;
    }
    void setColor(uint32_t color_i){
        this->color = color_i;
    }
    int getHeight(){
        return this->HEIGHT;
    }
    int getWidth(){
        return this->WIDTH;
    }
    uint32_t getColor(){
        return this->color;
    }//

    virtual void
    paint_pixels() {
        auto *pixel = static_cast<uint32_t *>(shm_data);
        for (int n = 0; n < WIDTH*HEIGHT; n++) {
            if ((n < 3*WIDTH) || (n > (HEIGHT-4)*WIDTH) || ( n % WIDTH < 3) || (n % WIDTH > WIDTH - 4)
            || ((n > 27*WIDTH) && (n < 30*WIDTH)))
                *pixel++=0x00;
            else if ((n > 3*WIDTH) && (n < 30*WIDTH))
                *pixel++=0xffffff;
            else
                *pixel++ = color;
        }
    }
};
