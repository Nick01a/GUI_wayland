#include <utility>
#include <string>
#include "cgobject.cpp"
#include <functional>
struct CButton : CGObject{
    std::string title;
    std::function<void()> action;
    int x;
    int y;
    CButton(std::string title, std::function<void()> action ){
        this->title = std::move(title);
        this->action = std::move(action);
    }
    void setLocation(int x,int y){
        this->x = x;
        this->y = y;

    }
    void setSize(int h,int w){
    this->HEIGHT=h;
    this->WIDTH =w;
}
std::string getTitle(){
    return this->title;
}
char* getCharTitle() {
    return const_cast<char *>(this->title.c_str());
}
bool isInside(int pointer_x,int pointer_y){
    return  ((pointer_x > x) && (pointer_x < x + WIDTH) && (pointer_y > y) && (pointer_y < y+HEIGHT));
    }
};