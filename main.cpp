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
#include "towers.h"

using json = nlohmann::json;

std::vector<std::array<int, 2>> Funktion1(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight);
void DrawLines(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int mouseX, int  mouseY, int windowWidth, int windowHeight);
void SaveToJSON(std::vector<std::array<int, 2>> posList);
std::ifstream LoadJSON();
std::vector<std::array<int, 2>> Creating(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, std::vector<std::array<int, 2>> posList);
void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int mouseX, int mouseY);
bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY);
std::vector<SDL_Rect> SetButtonCoords(int windowWidth, int windowHeight, int buttonWidth, int buttonHeight, int offset, std::vector<SDL_Rect> buttons);
std::vector<std::array<int, 2>> ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, int nextWindow);
void View(SDL_Renderer *renderer, std::vector<std::array<int, 2>> posList, SDL_Event ev, bool *running, int *whichUI);
std::vector<std::array<int, 2>> AddFinalPoint(int mouseX, int mouseY, std::vector<std::array<int, 2>> posList, int windowWidth, int windowHeight);
void Play(SDL_Renderer *renderer, std::vector<Obel> obler, bool *running, int *whichUI, SDL_Event ev, int windowWidth, int windowHeight, float deltaTime, Image obelBilled, std::vector<std::array<int, 2>> posList);
std::vector<SDL_Rect> DrawTowerMenu(int mouseX, int mouseY, SDL_Renderer *renderer, int windowWidth, int windowHeight);
std::vector<SDL_Rect> DrawGrid(SDL_Renderer *renderer, int width, int height, int mouseX, int mouseY, int rows, int collums, int buttons, int offset, int startX, int startY, std::vector<std::string> buttonTitles);

namespace obj
{
    std::vector<Obel> obler { };
    std::vector<ObelTower>
}

int main(int argc, char *argv[])
{
    int windowWidth { 700 };
    int windowHeight { 700 };

    int prevTime { 0 };
    int currentTime { 0 };
    float deltaTime { 0.0f };

    int mouseX { };
    int mouseY { };

    SDL_Window *window { SDL_CreateWindow("obel2d", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN) };
    SDL_Renderer *renderer { SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };

    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
    TTF_Init();

    Image obelBilled;
    obelBilled.init(renderer, "obelLectio.jpg");
    obelBilled.resizeKA('w', 50, windowHeight);


    std::vector<std::array<int, 2>> posList;


    SDL_Event ev;
    bool running { true };

    // 0 = menu, 1 = creating, 2 = viewing menu, 3 = viewing, 4 = Play
    int whichUI { 0 };

    while(running)
    {
        prevTime = currentTime;
        currentTime = SDL_GetTicks();
        deltaTime = (currentTime - prevTime) / 1000.0f;

        switch(whichUI)
        {
            case 0:
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, posList);
                break;
            case 1:
                posList = Creating(renderer, posList, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 2:
                posList = ViewMenu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, 3);
                break;
            case 3:
                View(renderer, posList, ev, &running, &whichUI);
                break;
            case 4:
                posList = ViewMenu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, 5);
                break;
            case 5:
                obler = Play(renderer, obler, &running, &whichUI, ev, windowWidth, windowHeight, deltaTime, obelBilled, posList);
                break;
            default:
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, posList);
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
        std::array<int, 2> temp { windowWidth, posList[posList.size() - 1][1] };
        posList.push_back(temp);
    } else if(posList[0][0] == windowWidth)
    {
        std::array<int, 2> temp { 0, posList[posList.size() - 1][1] };
        posList.push_back(temp);
    } else if(posList[0][1] == 0)
    {
        std::array<int, 2> temp { posList[posList.size() - 1][0], windowWidth };
        posList.push_back(temp);
    } else if(posList[0][1] == windowWidth)
    {
        std::array<int, 2> temp { posList[posList.size() - 1][0], 0  };
        posList.push_back(temp);
    }

    return posList;
}

void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, std::vector<std::array<int, 2>> posList)
{
    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);

    int buttonWidth { 350 };
    int buttonHeight { 80 };
    int offset { 30 };

    std::vector<SDL_Rect> buttons(4);

    json j;
    if(std::filesystem::exists("map.json"))
    {
        LoadJSON() >> j;
    }

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
                    else if(GetMouseHover(buttons[2], mouseX, mouseY) && j.size() > 0)
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

std::vector<std::array<int, 2>> ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, int nextWindow)
{
    json j;
    LoadJSON() >> j;
    std::vector<std::string> titles(j.size());
    for(int i { }; i<titles.size(); ++i)
        titles[i] = std::to_string(i + 1);

    int offset ( 40 );
    std::vector<SDL_Rect> buttons(j.size());
    // int buttonHeight { (windowHeight - offset * static_cast<int>(j.size())) / static_cast<int>(j.size()) };
    // buttons = SetButtonCoords(windowWidth, windowHeight, 350, buttonHeight, offset, buttons);

    std::vector<std::array<int, 2>> posList;

    int mouseX { };
    int mouseY { };

    buttons = DrawGrid(renderer, windowWidth, windowHeight, mouseX, mouseY, 5, 5, buttons.size(), offset, 0, 0, titles);

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
                            *whichUI = nextWindow;
                        }
                    }
                    break;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // DrawMenuButtons(renderer, buttons, titles, mouseX, mouseY);
    buttons = DrawGrid(renderer, windowWidth, windowHeight, mouseX, mouseY, 5, 5, buttons.size(), offset, 0, 0, titles);

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

void Play(SDL_Renderer *renderer, std::vector<Obel> obler, bool *running, int *whichUI, SDL_Event ev, int windowWidth, int windowHeight, float deltaTime, Image obelBilled, std::vector<std::array<int, 2>> posList)
{
    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);

    std::vector<SDL_Rect> buttons;
    static bool isBuilding { };
    int whichBuilding { };
    ObelTower victor;
    victor.billed = obelBilled;
    victor.dps = 1;

    buttons = DrawTowerMenu(mouseX, mouseY, renderer, windowWidth, windowHeight);

    while(SDL_PollEvent(&ev) != 0)
    {
        if(ev.type == SDL_QUIT)
            *running = false;
        else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_ESCAPE:
                    *whichUI = 4;
                    for(int i { }; i<obler.size(); ++i)
                        obler.clear();
                    break;
                case SDLK_SPACE:
                    Obel temp;
                    temp.initObel(posList, renderer);
                    temp.billed = obelBilled;
                    temp.speed = 100;
                    temp.hp = 10;
                    obler.push_back(temp);
                    break;
            }

        } else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    if(GetMouseHover(buttons[0], mouseX, mouseY))
                    {
                        whichBuilding = 0;
                        isBuilding = !isBuilding;
                    } else
                    {
                        isBuilding
                    }
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

    for(int i { static_cast<int>(obler.size()) - 1}; i>=0; --i)
    {
        obler[i].obelMove(deltaTime, posList);
        obler[i].countDown -= 1 * deltaTime;

        obler[i].drawObel(renderer);

        if(obler[i].reachedPoint == posList.size() - 1 || obler[i].hp <= 0)
        {
            obler.erase(obler.begin() + i);
            obler[i].cleanUp();
        }
    }

    buttons = DrawTowerMenu(mouseX, mouseY, renderer, windowWidth, windowHeight);

    if(isBuilding)
    {
        victor.billed.x = mouseX;
        victor.billed.y = mouseY;
        victor.draw(renderer);
    }

    SDL_RenderPresent(renderer);
}

std::vector<SDL_Rect> DrawTowerMenu(int mouseX, int mouseY, SDL_Renderer *renderer, int windowWidth, int windowHeight)
{
    std::vector<SDL_Rect> buttonList;

    int width { 200 };
    SDL_Color color { 163, 150, 137, 255 };

    SDL_Rect menuRect;
    menuRect.h = windowHeight;
    menuRect.w = width;
    menuRect.x = windowWidth - width;
    menuRect.y = 0;

    if(mouseX > windowWidth - width)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &menuRect);
        buttonList = DrawGrid(renderer, menuRect.w, menuRect.h, mouseX, mouseY, 1, 1, 1, 10, menuRect.x, 0, { "din mor" });
    }

    return buttonList;
}

std::vector<SDL_Rect> DrawGrid(SDL_Renderer *renderer, int width, int height, int mouseX, int mouseY, int rows, int collums, int buttons, int offset, int startX, int startY, std::vector<std::string> buttonTitles)
{
    Text buttonText;
    buttonText.color = { 255, 255, 255 };
    buttonText.fontSize = 40;
    buttonText.init(renderer, "arial.ttf");

    SDL_Color hover { 85, 106, 130, 255 };
    SDL_Color standard { 66, 83, 102, 255 };

    std::vector<SDL_Rect> buttonList;

    int buttonHeight { (height - offset * (rows + 1)) / rows };
    int buttonWidth { (width - offset * (rows + 1)) / collums };

    int x { startX + offset };
    int y { startY + offset };

    SDL_Rect buttonRect;
    buttonRect.h = buttonHeight;
    buttonRect.w = buttonWidth;

    for(int i { }; i<buttons; ++i)
    {
        buttonRect.x = x;
        buttonRect.y = y;
        buttonList.push_back(buttonRect);

        roundedBoxRGBA(renderer, buttonRect.x - 5, buttonRect.y - 5, buttonRect.x + buttonRect.w + 5, buttonRect.y + buttonRect.h + 5, 25, 43, 53, 64, 255);
        if(GetMouseHover(buttonRect, mouseX, mouseY))
            roundedBoxRGBA(renderer, buttonRect.x + 5, buttonRect.y + 5, buttonRect.x + buttonRect.w - 5, buttonRect.y + buttonRect.h - 5, 25, hover.r, hover.g, hover.b, 255);
        else
            roundedBoxRGBA(renderer, buttonRect.x + 5, buttonRect.y + 5, buttonRect.x + buttonRect.w - 5, buttonRect.y + buttonRect.h - 5, 25, standard.r, standard.g, standard.b, 255);

        buttonText.textString = buttonTitles[i];
        buttonText.updateSize(renderer);
        buttonText.x = buttonRect.x + (buttonRect.w / 2 - buttonText.w / 2);
        buttonText.y = buttonRect.y + (buttonRect.h / 2 - buttonText.h / 2);
        buttonText.draw(renderer);

        if((i + 1) % collums == 0)
        {
            y += buttonHeight + offset;
            x = startX + offset;
        } else
        {
            x += buttonWidth + offset;
        }
    }

    buttonText.destroyFont();

    return buttonList;
}
