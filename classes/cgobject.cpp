#include <utility>
<<<<<<< HEAD
=======
#include "handler.cpp"
>>>>>>> fe3cc130ab6749fa402cd97be2b0d4e0048e49f8
#include <iostream>
#include <vector>
#include "color.cpp"

struct CGObject {
    struct wl_surface *surface;
<<<<<<< HEAD
=======
    struct wl_buffer *buffer;
>>>>>>> fe3cc130ab6749fa402cd97be2b0d4e0048e49f8
    int HEIGHT;
    int WIDTH;
    uint32_t color;

<<<<<<< HEAD
=======
    void setSize(int h,int w){
        this->WIDTH = w;
        this->HEIGHT = h + 30;
    }
>>>>>>> fe3cc130ab6749fa402cd97be2b0d4e0048e49f8
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
<<<<<<< HEAD
=======
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
>>>>>>> fe3cc130ab6749fa402cd97be2b0d4e0048e49f8
    }
};
