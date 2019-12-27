<<<<<<< HEAD
#include <utility>

//
// Created by yura on 12/10/19.
//
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
=======
struct
>>>>>>> fe3cc130ab6749fa402cd97be2b0d4e0048e49f8
