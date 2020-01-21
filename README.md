# GUI_wayland   
## General information   
This toolkit was based on wayland. Main target of it was to create toolkit which would enable to create a simple UI applications.

## Authors  
Kazan Yurii and Mykola Kyrychenko   

## Pre-installation  
Libraries you need to run this toolkit are:  
1. Cairo (for pictures rendering)
2. Pango (text manipulations)
3. EGL (connection between levels)
4. Wayland  

## Compositors  
You need to have weston compositor in order to run your apps. 

## How to use:  
There are a few classes that can be used in order to develop you application.  
1. CFrame 
2. CButton  
3. CLabel   
4. CImage  

## Additional classes:  
1. PainterHandler (struct which paint all you need on the window)
2. CairoHandler (struct to work with images and text)

## What is implemented:  
1. Buttons can perform any **void** return function.  
2. Window (CFrame) can be resized and collapsed.   
3. CLabel can hold text on different languages.
4. CImage can display image on the frame.

### Example  
You can run main.cpp in order to look on example.
