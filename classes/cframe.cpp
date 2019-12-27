#include <utility>
#include "cbutton.cpp"
#include "clabel.cpp"
#include "cimage.cpp"
struct CFrame : CGObject{
    std::string title;
    std::vector<CLabel> vector;
    std::vector<CImage> images;
    std::vector<CButton> buttons;
    CImage cImage = CImage("exit.png");
    void setTitle(std::string title){
        this->title = std::move(title);
    }
    void setSize(int h,int w){
        this->HEIGHT=h+30;
        this->WIDTH =w;
        this->cImage.setLocation(w-20,4);
    }
    std::string getTitle(){
        return this->title;
    }
    char* getCharTitle() {
        return const_cast<char *>(this->title.c_str());
    }
    void addLabel(CLabel &c){
        this->vector.push_back(c);
    }
    void addImage(CImage &c){
        this->images.push_back(c);
    }
    void addButton(CButton &c){
        this->buttons.push_back(c);
    }
};