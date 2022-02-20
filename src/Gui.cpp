#include "Gui.h"

static const int sideWidth = 200;

Gui::Gui()
{
    this->mainWindow = nullptr;
    this->render     = nullptr;
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        std::cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
    }
    std::cout << "SDL initialised" << std::endl;
}

Gui::~Gui()
{
    if (this->mainWindow != nullptr)
    {
        SDL_DestroyRenderer(this->render);
        SDL_DestroyWindow(this->mainWindow);
    }
    SDL_Quit();
}

void Gui::update()
{
    int cubewidth = this->data->getSize();

    //hardcoded starting positions of each face on the 2d plane
    std::vector<Gpoint> startFace;    
    startFace.push_back(Gpoint(210,   0, 4));  //U
    startFace.push_back(Gpoint(  0, 210, 3));  //L
    startFace.push_back(Gpoint(210, 210, 0));  //F
    startFace.push_back(Gpoint(420, 210, 1));  //R
    startFace.push_back(Gpoint(630, 210, 2));  //B
    startFace.push_back(Gpoint(210, 420, 5));  //D

    int cubeling = sideWidth / cubewidth;

    //this function only affects one face, not it's neighbours
    this->data->rotateFace(5, true);
    this->data->rotateFace(5, true);

    for (auto& f : startFace)
    {
        for (int frows = 0; frows < cubewidth; frows++)
        {
            for (int fcols = 0; fcols < cubewidth; fcols++)
            {
                int current_col = f.faceID * cubewidth;
                auto color = this->data->getElement(fcols + current_col, frows);
                auto cubelingColor = this->getcolor(*color);

                SDL_Rect rekt;
                rekt.x = f.x + fcols * cubeling + 3;
                rekt.y = f.y + frows * cubeling + 3;
                rekt.w = cubeling - 3;
                rekt.h = cubeling - 3;
                SDL_SetRenderDrawColor(this->render, cubelingColor.r, cubelingColor.g, cubelingColor.b, 255);
                SDL_RenderFillRect(this->render, &rekt);
            }
        }
    }
    this->data->rotateFace(5, true);
    this->data->rotateFace(5, true);

    SDL_RenderPresent(this->render);
}


void Gui::clear()
{
    this->data.reset();
    SDL_SetRenderDrawColor(this->render, 0, 0, 0, 255);
    SDL_UpdateWindowSurface( this->mainWindow );
    SDL_RenderClear(this->render);
    SDL_RenderPresent(this->render);
}

void Gui::createWindow()
{
    this->mainWindow = SDL_CreateWindow( "KarRubik", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 840, 630, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
    this->render     = SDL_CreateRenderer(this->mainWindow, -1, 0);

    SDL_Delay(200);

    this->clear();
}

void Gui::setdata(std::shared_ptr<RC> ref)
{
    this->data = ref;
}

void Gui::die()
{
    if (this->mainWindow != nullptr)
    {
        SDL_DestroyRenderer(this->render);
        SDL_DestroyWindow( this->mainWindow );
    }
}


SDL_Color Gui::getcolor(CubeColor& c)
{
    SDL_Color out;
    switch (c)
    {
    case CubeColor::Green:
        out.r = 34;
        out.g = 255;
        out.b = 1;
        out.a = 255;
        break;
    case CubeColor::Red:
        out.r = 242;
        out.g = 34;
        out.b = 255;
        out.a = 255;
        break;
    case CubeColor::Blue:
        out.r = 20;
        out.g = 220;
        out.b = 255;
        out.a = 255;
        break;
    case CubeColor::Orange:
        out.r = 255;
        out.g = 0;
        out.b = 128;
        out.a = 255;
        break;
    case CubeColor::White:
        out.r = 255;
        out.g = 255;
        out.b = 255;
        out.a = 255;
        break;
    case CubeColor::Yellow:
        out.r = 226;
        out.g = 255;
        out.b = 20;
        out.a = 255;
        break;
    default:
        out.r = 0;
        out.g = 0;
        out.b = 0;
        out.a = 0;
        break;
    }
    return out;
}
void Gui::savePNG(std::string filename)
{
    if (this->mainWindow)
    {
        SDL_Surface * out = SDL_GetWindowSurface(this->mainWindow);
        std::cout << SDL_GetError() << std::endl;
        if (out)
        {
            IMG_SavePNG(out, filename.c_str());
        }
        SDL_FreeSurface(out);
    }
}
