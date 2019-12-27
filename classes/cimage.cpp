#include <utility>
#include <iostream>
struct CImage{
    std::string filename;
    int x;
    int y;
    CImage(std::string text){
        this->filename = std::move(text);
    }
    void setLocation(int x,int y){
        this->x = x;
        this->y = y;

    }
    char* getCharText(){
        return const_cast<char *>(this->filename.c_str());
    }
};
struct ExitCImage :CImage{
    ExitCImage(std::string text = "exit.png") : CImage(std::move(text)) {
    }
};