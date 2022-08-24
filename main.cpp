#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <array>
#include <filesystem>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <SDL2/SDL_ttf.h>

#include "../common/func.cpp"
#include "../common/images.h"
#include "../common/fonts.h"
#include "json.hpp"
#include "obel.h"

using json = nlohmann::json;

std::vector<std::array<int, 2>> Funktion1(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight);
void DrawLines(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int mouseX, int  mouseY, int windowWidth, int windowHeight);
void SaveToJSON(std::vector<std::array<int, 2>> posList);
std::ifstream LoadJSON();
std::vector<std::array<int, 2>> Creating(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int mouseX, int mouseY);
bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY);
std::vector<SDL_Rect> SetButtonCoords(int windowWidth, int windowHeight, int buttonWidth, int buttonHeight, int offset, std::vector<SDL_Rect> buttons);
std::vector<std::array<int, 2>> ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void View(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, SDL_Event ev, bool *running, int *whichUI);
std::vector<std::array<int, 2>> AddFinalPoint(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight);
std::vector<Obel> Play(SDL_Renderer *renderer, std::vector<Obel> obler, bool *running, int *whichUI, SDL_Event ev, int windowWidth);

int main(int argc, char *argv[])
{
    int windowWidth { 700 };
    int windowHeight { 700 };

    int mouseX { };
    int mouseY { };

    SDL_Window *window { SDL_CreateWindow("obel2d", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN) };
    SDL_Renderer *renderer { SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();

    Image obelBilled;
    obelBilled.init(renderer, "obelLectio.jpg");
    obelBilled.resizeKA('w', 200, windowHeight);

    Obel obel;
    obel.billed = obelBilled;
    obel.speed = 1;

    std::vector<Obel> obler { obel };

    std::vector<std::array<int, 2>> posList;

    SDL_Event ev;
    bool running { true };

    // 0 = menu, 1 = creating, 2 = viewing menu, 3 = viewing, 4 = Play
    int whichUI { 0 };

    while(running)
    {
        switch(whichUI)
        {
            case 0:
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 1:
                posList = Creating(renderer, posList, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 2:
                posList = ViewMenu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 3:
                View(renderer, posList, ev, &running, &whichUI);
                break;
            case 4:
                obler = Play(renderer, obler, &running, &whichUI, ev, windowWidth);
                break;
            default:
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    window = nullptr;
    renderer = nullptr;

    obelBilled.destroyTexture();

    SDL_Quit();
    IMG_Quit();
    TTF_Quit();

    return 0;
}

std::vector<std::array<int, 2>> Funktion1(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight)
{
    if(posList.size() == 0 && mouseX < windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
    {
        std::array<int, 2> temp { 0, mouseY };
        posList.push_back(temp);
    }else if(posList.size() == 0 && mouseX > windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
    {
        std::array<int, 2> temp { windowWidth, mouseY };
        posList.push_back(temp);
    } else if(posList.size() == 0 && mouseY > windowHeight - (windowHeight / 4))
    {
        std::array<int, 2> temp { mouseX, windowHeight};
        posList.push_back(temp);
    } else if(posList.size() == 0 && mouseY < windowHeight / 4)
    {
        std::array<int, 2> temp { mouseX, 0};
        posList.push_back(temp);
    } else
    {
        std::array<int, 2> temp { mouseX, mouseY };
        posList.push_back(temp);
    }

    return posList;
}

void DrawLines(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int mouseX, int  mouseY, int windowWidth, int windowHeight)
{
    if(posList.size() == 0 && mouseX < windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
        thickLineRGBA(renderer, 0, mouseY, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(posList.size() == 0 && mouseX > windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
        thickLineRGBA(renderer, windowWidth, mouseY, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(posList.size() == 0 && mouseY > windowHeight - (windowHeight / 4))
        thickLineRGBA(renderer, mouseX, windowHeight, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(posList.size() == 0 && mouseY < windowHeight / 4)
        thickLineRGBA(renderer, mouseX, 0, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(posList.size() >= 2)
    {
        for(int i { }; i<posList.size(); ++i)
        {
            if(i < posList.size() - 1)
                thickLineRGBA(renderer, posList[i][0], posList[i][1], posList[i + 1][0], posList[i + 1][1], 10, 0, 0, 0, 255);
        }
        thickLineRGBA(renderer, posList.back()[0], posList.back()[1], mouseX, mouseY, 10, 0, 0, 0, 255);
    } else if(posList.size() > 0)
        thickLineRGBA(renderer, posList[0][0], posList[0][1], mouseX, mouseY, 10, 0, 0, 0, 255);
}

void SaveToJSON(std::vector<std::array<int, 2>> posList)
{
    json j;
    if(std::filesystem::exists("map.json"))
    {
        std::ifstream file("map.json");
        file >> j;
    }

    int num { static_cast<int>(j.size()) + 1 };

    json j_vec(posList);
    j[std::to_string(num)] = j_vec;

    std::ofstream o("map.json");
    o << std::setw(4) << j << '\n';
}

std::ifstream LoadJSON()
{
    std::ifstream file("map.json");

    return file;
}

std::vector<std::array<int, 2>> Creating(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI)
{
    int mouseX { };
    int mouseY { };

    SDL_GetMouseState(&mouseX, &mouseY);
    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
            *running = false;
        else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    posList = Funktion1(mouseX, mouseY, posList, windowWidth, windowHeight);
                    break;
            }
        } else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_s:
                    posList = AddFinalPoint(mouseX, mouseY, posList, windowWidth, windowHeight);
                    SaveToJSON(posList);
                    *whichUI = 0;
                    break;
                case SDLK_r:
                    posList.clear();
                    break;
                case SDLK_z:
                    if(posList.size() > 0)
                        posList.pop_back();
                    break;
                case SDLK_PLUS:
                    *whichUI += 1;
                    break;
                case SDLK_MINUS:
                    *whichUI -= 1;
                    break;
                case SDLK_ESCAPE:
                    *whichUI = 0;
                    break;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    DrawLines(renderer, posList, mouseX, mouseY, windowWidth, windowHeight);

    SDL_RenderPresent(renderer);

    return posList;
}

std::vector<std::array<int, 2>> AddFinalPoint(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight)
{
    if(posList[0][0] == 0)
    {
        std::array<int, 2> temp { 500, posList[posList.size() - 1][1] };
        posList.push_back(temp);
    } else if(posList[0][0] == 500)
    {
        std::array<int, 2> temp { 0, posList[posList.size() - 1][1] };
        posList.push_back(temp);
    } else if(posList[0][1] == 0)
    {
        std::array<int, 2> temp { posList[posList.size() - 1][0], 500  };
        posList.push_back(temp);
    } else if(posList[0][1] == 500)
    {
        std::array<int, 2> temp { posList[posList.size() - 1][0], 0  };
        posList.push_back(temp);
    }

    return posList;
}

void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI)
{
    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);


    int buttonWidth { 350 };
    int buttonHeight { 80 };
    int offset { 30 };

    std::vector<SDL_Rect> buttons(4);

    buttons = SetButtonCoords(windowWidth, windowHeight, buttonWidth, buttonHeight, offset, buttons);

    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
            *running = false;
        else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_PLUS:
                    *whichUI += 1;
                    break;
                case SDLK_MINUS:
                    *whichUI -= 1;
                    break;
                case SDLK_ESCAPE:
                    *running = false;
                    break;
            }
        } else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    if(GetMouseHover(buttons[0], mouseX, mouseY))
                        *whichUI = 4;
                    else if(GetMouseHover(buttons[1], mouseX, mouseY))
                        *whichUI = 1;
                    else if(GetMouseHover(buttons[2], mouseX, mouseY))
                        *whichUI = 2;
                    else if(GetMouseHover(buttons[3], mouseX, mouseY))
                        *running = false;
                    break;
            }
        }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    DrawMenuButtons(renderer, buttons, { "Play", "create", "view", "exit" }, mouseX, mouseY);

    SDL_RenderPresent(renderer);
}

void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int mouseX, int mouseY)
{
    SDL_Color hover { 85, 106, 130, 255 };
    SDL_Color standard { 66, 83, 102, 255 };

    Text buttonText;
    buttonText.color = { 255, 255, 255 };
    buttonText.fontSize = 40;
    buttonText.init(renderer, "arial.ttf");

    SDL_Rect textRect;

    for(int i { }; i<buttons.size(); ++i)
    {
        roundedBoxRGBA(renderer, buttons[i].x - 5, buttons[i].y - 5, buttons[i].x + buttons[i].w + 5, buttons[i].y + buttons[i].h + 5, 25, 43, 53, 64, 255);
        if(GetMouseHover(buttons[i], mouseX, mouseY))
            roundedBoxRGBA(renderer, buttons[i].x + 5, buttons[i].y + 5, buttons[i].x + buttons[i].w - 5, buttons[i].y + buttons[i].h - 5, 15, hover.r, hover.g, hover.b, hover.a);
        else
            roundedBoxRGBA(renderer, buttons[i].x + 5, buttons[i].y + 5, buttons[i].x + buttons[i].w - 5, buttons[i].y + buttons[i].h - 5, 15, standard.r, standard.g, standard.b, standard.a);

        buttonText.textString = buttonTitles[i];
        buttonText.updateSize(renderer);
        buttonText.x = buttons[i].x + (buttons[i].w / 2 - buttonText.w / 2);
        buttonText.y = buttons[i].y + (buttons[i].h / 2 - buttonText.h / 2);
        buttonText.draw(renderer);
        textRect.h = buttonText.h;
        textRect.w = buttonText.w;
        textRect.x = buttonText.x;
        textRect.y = buttonText.y;
    }

    buttonText.destroyFont();
}

bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY)
{
    if(mouseX > button.x && mouseX < button.x + button.w && mouseY > button.y && mouseY < button.y + button.h)
        return true;

    return false;
}

std::vector<SDL_Rect> SetButtonCoords(int windowWidth, int windowHeight, int buttonWidth, int buttonHeight, int offset, std::vector<SDL_Rect> buttons)
{
    for(int i { }; i<buttons.size(); ++i)
    {
        buttons[i].w = buttonWidth;
        buttons[i].h = buttonHeight;
        buttons[i].x = windowWidth / 2 - buttons[i].w / 2;
        buttons[i].y = windowHeight / 2 - (buttons.size() * buttonHeight  + (buttons.size() - 1) * offset) / 2 + i * (offset + buttonHeight);
    }

    return buttons;
}

std::vector<std::array<int, 2>> ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI)
{
    json j;
    LoadJSON() >> j;
    std::vector<std::string> titles(j.size());
    for(int i { }; i<titles.size(); ++i)
        titles[i] = std::to_string(i + 1);

    int offset ( 40 );
    std::vector<SDL_Rect> buttons(j.size());
    int buttonHeight { (windowHeight - offset * static_cast<int>(j.size())) / static_cast<int>(j.size()) };
    buttons = SetButtonCoords(windowWidth, windowHeight, 350, buttonHeight, offset, buttons);

    std::vector<std::array<int, 2>> posList;

    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);

    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
        {
            *running = false;
        } else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_PLUS:
                    ++*whichUI;
                    break;
                case SDLK_MINUS:
                    --*whichUI;
                    break;
                case SDLK_ESCAPE:
                    *whichUI = 0;
                    break;
            }
        } else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    for(int i { }; i<buttons.size(); ++i)
                    {
                        if(GetMouseHover(buttons[i], mouseX, mouseY))
                        {
                            posList = j[std::to_string(i + 1)];
                            *whichUI = 3;
                        }
                    }
                    break;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    DrawMenuButtons(renderer, buttons, titles, mouseX, mouseY);

    SDL_RenderPresent(renderer);

    return posList;
}

void View(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, SDL_Event ev, bool *running, int *whichUI)
{
    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
            *running = false;
        else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_PLUS:
                    ++*whichUI;
                    break;
                case SDLK_MINUS:
                    --*whichUI;
                    break;
                case SDLK_ESCAPE:
                    *whichUI = 2;
                    break;
            }
        }

    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for(int i { }; i<posList.size(); ++i)
    {
        if(i < posList.size() - 1)
            thickLineRGBA(renderer, posList[i][0], posList[i][1], posList[i + 1][0], posList[i + 1][1], 10, 0, 0, 0, 255);
    }

    SDL_RenderPresent(renderer);
}

std::vector<Obel> Play(SDL_Renderer *renderer, std::vector<Obel> obler, bool *running, int *whichUI, SDL_Event ev, int windowWidth)
{
    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
            *running = false;
        if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    *whichUI = 0;
                    for(int i { }; i<obler.size(); ++i)
                        obler[i].billed.x = 0;
                    break;
            }

        }
    }

    obler[0].obelMove();
   // if(obler[obler.size() - 1].x >= windowWidth)
   // {

   // }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    obler[0].drawObel(renderer);

    SDL_RenderPresent(renderer);

    return obler;
}
