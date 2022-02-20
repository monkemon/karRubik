#pragma once

#include <SDL2/SDL.h>
#include <SDL_image.h>

#include "Cube.h"

//Gui class used only for displaying the cube graphically, SDL called only inside this class

struct Gpoint
{
    int x;
    int y;
    int faceID;
    Gpoint(int x, int y, int fid) : x(x), y(y), faceID(fid) {}
};


class Gui
{
private:
    std::shared_ptr<RC> data;
    SDL_Window   *mainWindow;
    SDL_Renderer     *render;
public:
    Gui();
    ~Gui();

    void die();
    void clear();
    void update();
    void createWindow();
    void deleteWindow();
    void savePNG(std::string filename);
    void setdata(std::shared_ptr<RC> ref);

    SDL_Color getcolor(CubeColor& c);
};