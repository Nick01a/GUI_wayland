#include <utility>
#include <iostream>
#include <vector>
#include "color.cpp"

struct CGObject {
    struct wl_surface *surface;
    int HEIGHT;
    int WIDTH;
    uint32_t color;

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
    }
};
