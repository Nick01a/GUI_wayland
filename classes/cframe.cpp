#include <utility>
//
// Created by yura on 12/3/19.
//
#include "cgobject.cpp"
struct CFrame : CGObject{
    std::string title;
    void setTitle(std::string title){
        this->title = std::move(title);
    }
    std::string getTitle(){
        return this->title;
    }
    char* getCharTitle() {
        return const_cast<char *>(this->title.c_str());
    }
};