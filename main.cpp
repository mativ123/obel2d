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

using json = nlohmann::json;

std::vector<std::array<int, 2>> Funktion1(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight);
void DrawLines(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int mouseX, int  mouseY, int windowWidth, int windowHeight);
void SaveToJSON(std::vector<std::array<int, 2>> posList);
std::vector<std::array<int, 2>> LoadFromJSON(std::vector<std::array<int, 2>> posList);
std::vector<std::array<int, 2>> Creating(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int windowWidth, int windowHeight, int mouseX, int mouseY);
bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY);

int main(int argc, char *argv[])
{
    int windowWidth { 500 };
    int windowHeight { 500 };

    int mouseX { };
    int mouseY { };

    SDL_Window *window { SDL_CreateWindow("obel2d", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN) };
    SDL_Renderer *renderer { SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();

    Image obel;
    obel.init(renderer, "obelLectio.jpg");
    obel.resizeKA('w', 20, windowHeight);

    std::vector<std::array<int, 2>> posList;

    SDL_Event ev;
    bool running { true };

    // 0 = menu, 1 = creating, 2 = viewing
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
            default:
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    window = nullptr;
    renderer = nullptr;

    obel.destroyTexture();

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

std::vector<std::array<int, 2>> LoadFromJSON(std::vector<std::array<int, 2>> posList)
{
    std::ifstream file("map.json");
    json j;
    file >> j;


    posList = j[std::to_string(j.size())];

    return posList;
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
                    SaveToJSON(posList);
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
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    DrawLines(renderer, posList, mouseX, mouseY, windowWidth, windowHeight);

    SDL_RenderPresent(renderer);

    return posList;
}

void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI)
{
    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);

    int buttonWidth { 350 };
    int buttonHeight { 100 };

    SDL_Rect createButton;
    createButton.w = buttonWidth;
    createButton.h = buttonHeight;

    SDL_Rect exitButton;
    exitButton.w = buttonWidth;
    exitButton.h = buttonHeight;



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
            }
        } else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    std::cout << "click\n";
                    if(GetMouseHover(createButton, mouseX, mouseY))
                        *whichUI = 1;
                    else if(GetMouseHover(exitButton, mouseX, mouseY))
                        *running = false;
                    break;
            }
        }
    }


    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    DrawMenuButtons(renderer, { createButton, exitButton }, { "create", "exit" }, windowWidth, windowHeight, mouseX, mouseY);

    SDL_RenderPresent(renderer);
}

void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int windowWidth, int windowHeight, int mouseX, int mouseY)
{
    SDL_Color hover { 85, 106, 130, 255 };
    SDL_Color standard { 66, 83, 102, 255 };

    Text buttonText;
    buttonText.color = { 255, 255, 255 };
    buttonText.fontSize = 40;
    buttonText.init(renderer, "arial.ttf");

    SDL_Rect textRect;

    int offset { 40 };

    int uiHeight { };

    for(SDL_Rect i : buttons)
    {
        uiHeight += i.h + offset;
    }
    uiHeight -= offset;

    int y { windowHeight / 2 - uiHeight / 2 };

    for(int i { }; i<buttons.size(); ++i)
    {
        buttons[i].y = y;
        buttons[i].x = windowWidth / 2 - buttons[i].w / 2;
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


        y += buttons[i].h + offset;
    }

    buttonText.destroyFont();
}

bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY)
{
    if(mouseX > button.x && mouseX < button.x + button.w && mouseY > button.y && mouseY < button.y + button.h)
        return true;

    return false;
}
