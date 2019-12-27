#include <utility>
#include <iostream>

struct CLabel{
    std::string text;
    int x;
    int y;
    CLabel(std::string text){
        this->text = std::move(text);
    }
    void setLocation(int x,int y){
        this->x = x;
        this->y = y;

    }
    char* getCharText(){
        return const_cast<char *>(this->text.c_str());
    }
};
