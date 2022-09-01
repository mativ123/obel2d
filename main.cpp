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

void Funktion1(int mouseX, int mouseY, int windowWidth, int windowHeight);
void DrawLines(SDL_Renderer *renderer, int mouseX, int  mouseY, int windowWidth, int windowHeight);
void SaveToJSON();
std::ifstream LoadJSON();
void Creating(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void Menu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI);
void DrawMenuButtons(SDL_Renderer *renderer, std::vector<SDL_Rect> buttons, std::vector<std::string> buttonTitles, int mouseX, int mouseY);
bool GetMouseHover(SDL_Rect button, int mouseX, int mouseY);
std::vector<SDL_Rect> SetButtonCoords(int windowWidth, int windowHeight, int buttonWidth, int buttonHeight, int offset, std::vector<SDL_Rect> buttons);
void ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, int nextWindow);
void View(SDL_Renderer *renderer, SDL_Event ev, bool *running, int *whichUI);
void AddFinalPoint(int mouseX, int mouseY, int windowWidth, int windowHeight);
void Play(SDL_Renderer *renderer, bool *running, int *whichUI, SDL_Event ev, int windowWidth, int windowHeight, float deltaTime, Image obelBilled, Image victorBilled);
std::vector<SDL_Rect> DrawTowerMenu(int mouseX, int mouseY, SDL_Renderer *renderer, int windowWidth, int windowHeight);
std::vector<SDL_Rect> DrawGrid(SDL_Renderer *renderer, int width, int height, int mouseX, int mouseY, int rows, int collums, int buttons, int offset, int startX, int startY, std::vector<std::string> buttonTitles);

namespace obj
{
    std::vector<Obel> obler { };
    std::vector<ObelTower> towers;
    std::vector<std::array<int, 2>> posList;
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

    Image victorBilled;
    victorBilled.init(renderer, "victor.jpg");
    victorBilled.resizeKA('w', 50, windowHeight);

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
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 1:
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                Creating(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
            case 2:
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                ViewMenu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, 3);
                break;
            case 3:
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                View(renderer, ev, &running, &whichUI);
                break;
            case 4:
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                ViewMenu(renderer, windowWidth, windowHeight, &running, ev, &whichUI, 5);
                break;
            case 5:
                SDL_SetWindowSize(window, windowHeight + 200, windowHeight);
                Play(renderer, &running, &whichUI, ev, windowWidth, windowHeight, deltaTime, obelBilled, victorBilled);
                break;
            default:
                SDL_SetWindowSize(window, windowHeight, windowHeight);
                Menu(renderer, windowWidth, windowHeight, &running, ev, &whichUI);
                break;
        }
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    window = nullptr;
    renderer = nullptr;

    obelBilled.destroyTexture();
    victorBilled.destroyTexture();

    SDL_Quit();
    IMG_Quit();
    TTF_Quit();

    return 0;
}

void Funktion1(int mouseX, int mouseY, int windowWidth, int windowHeight)
{
    if(obj::posList.size() == 0 && mouseX < windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
    {
        std::array<int, 2> temp { 0, mouseY };
        obj::posList.push_back(temp);
    }else if(obj::posList.size() == 0 && mouseX > windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
    {
        std::array<int, 2> temp { windowWidth, mouseY };
        obj::posList.push_back(temp);
    } else if(obj::posList.size() == 0 && mouseY > windowHeight - (windowHeight / 4))
    {
        std::array<int, 2> temp { mouseX, windowHeight};
        obj::posList.push_back(temp);
    } else if(obj::posList.size() == 0 && mouseY < windowHeight / 4)
    {
        std::array<int, 2> temp { mouseX, 0};
        obj::posList.push_back(temp);
    } else
    {
        std::array<int, 2> temp { mouseX, mouseY };
        obj::posList.push_back(temp);
    }
}

void DrawLines(SDL_Renderer *renderer, int mouseX, int  mouseY, int windowWidth, int windowHeight)
{
    if(obj::posList.size() == 0 && mouseX < windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
        thickLineRGBA(renderer, 0, mouseY, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(obj::posList.size() == 0 && mouseX > windowWidth / 2 && mouseY < windowHeight - (windowHeight / 4) && mouseY > windowHeight / 4)
        thickLineRGBA(renderer, windowWidth, mouseY, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(obj::posList.size() == 0 && mouseY > windowHeight - (windowHeight / 4))
        thickLineRGBA(renderer, mouseX, windowHeight, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(obj::posList.size() == 0 && mouseY < windowHeight / 4)
        thickLineRGBA(renderer, mouseX, 0, mouseX, mouseY, 10, 0, 0, 0, 255);
    else if(obj::posList.size() >= 2)
    {
        for(int i { }; i<obj::posList.size(); ++i)
        {
            if(i < obj::posList.size() - 1)
                thickLineRGBA(renderer, obj::posList[i][0], obj::posList[i][1], obj::posList[i + 1][0], obj::posList[i + 1][1], 10, 0, 0, 0, 255);
        }
        thickLineRGBA(renderer, obj::posList.back()[0], obj::posList.back()[1], mouseX, mouseY, 10, 0, 0, 0, 255);
    } else if(obj::posList.size() > 0)
        thickLineRGBA(renderer, obj::posList[0][0], obj::posList[0][1], mouseX, mouseY, 10, 0, 0, 0, 255);
}

void SaveToJSON()
{
    json j;
    if(std::filesystem::exists("map.json"))
    {
        std::ifstream file("map.json");
        file >> j;
    }

    int num { static_cast<int>(j.size()) + 1 };

    json j_vec(obj::posList);
    j[std::to_string(num)] = j_vec;

    std::ofstream o("map.json");
    o << std::setw(4) << j << '\n';
}

std::ifstream LoadJSON()
{
    std::ifstream file("map.json");

    return file;
}

void Creating(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI)
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
                    Funktion1(mouseX, mouseY, windowWidth, windowHeight);
                    break;
            }
        } else if(ev.type == SDL_KEYDOWN)
        {
            switch(ev.key.keysym.sym)
            {
                case SDLK_s:
                    AddFinalPoint(mouseX, mouseY, windowWidth, windowHeight);
                    SaveToJSON();
                    *whichUI = 0;
                    break;
                case SDLK_r:
                    obj::posList.clear();
                    break;
                case SDLK_z:
                    if(obj::posList.size() > 0)
                        obj::posList.pop_back();
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

    DrawLines(renderer, mouseX, mouseY, windowWidth, windowHeight);

    SDL_RenderPresent(renderer);

}

void AddFinalPoint(int mouseX, int mouseY, int windowWidth, int windowHeight)
{
    if(obj::posList[0][0] == 0)
    {
        std::array<int, 2> temp { windowWidth, obj::posList[obj::posList.size() - 1][1] };
        obj::posList.push_back(temp);
    } else if(obj::posList[0][0] == windowWidth)
    {
        std::array<int, 2> temp { 0, obj::posList[obj::posList.size() - 1][1] };
        obj::posList.push_back(temp);
    } else if(obj::posList[0][1] == 0)
    {
        std::array<int, 2> temp { obj::posList[obj::posList.size() - 1][0], windowWidth };
        obj::posList.push_back(temp);
    } else if(obj::posList[0][1] == windowWidth)
    {
        std::array<int, 2> temp { obj::posList[obj::posList.size() - 1][0], 0  };
        obj::posList.push_back(temp);
    }
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

void ViewMenu(SDL_Renderer *renderer, int windowWidth, int windowHeight, bool *running, SDL_Event ev, int *whichUI, int nextWindow)
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
                            obj::posList = j[std::to_string(i + 1)];
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
}

void View(SDL_Renderer *renderer, SDL_Event ev, bool *running, int *whichUI)
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

    for(int i { }; i<obj::posList.size(); ++i)
    {
        if(i < obj::posList.size() - 1)
            thickLineRGBA(renderer, obj::posList[i][0], obj::posList[i][1], obj::posList[i + 1][0], obj::posList[i + 1][1], 10, 0, 0, 0, 255);
    }

    SDL_RenderPresent(renderer);
}

void Play(SDL_Renderer *renderer, bool *running, int *whichUI, SDL_Event ev, int windowWidth, int windowHeight, float deltaTime, Image obelBilled, Image victorBilled)
{
    int mouseX { };
    int mouseY { };
    SDL_GetMouseState(&mouseX, &mouseY);

    std::vector<SDL_Rect> buttons;
    static bool isBuilding { };
    int whichBuilding { };

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
                    for(int i { }; i<obj::obler.size(); ++i)
                        obj::obler.clear();
                    obj::towers.clear();
                    isBuilding = false;
                    break;
                case SDLK_SPACE:
                    Obel temp;
                    temp.initObel(obj::posList, renderer);
                    temp.billed = obelBilled;
                    temp.speed = 100;
                    temp.hp = 10;
                    obj::obler.push_back(temp);
                    break;
            }

        } else if(ev.type == SDL_MOUSEBUTTONDOWN)
        {
            switch(ev.button.button)
            {
                case SDL_BUTTON_LEFT:
                    if(GetMouseHover(buttons[0], mouseX, mouseY) && !isBuilding)
                    {
                        isBuilding = true;
                        ObelTower temp;
                        temp.billed = victorBilled;
                        temp.dps = 1;
                        obj::towers.push_back(temp);
                    } else if(GetMouseHover(buttons[1], mouseX, mouseY) && !isBuilding)
                    {
                        isBuilding = true;
                        ObelTower temp;
                        temp.billed = victorBilled;
                        temp.dps = 10;
                        obj::towers.push_back(temp);
                    } else
                    {
                        isBuilding = false;
                    }
                    break;
            }
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for(int i { }; i<obj::posList.size(); ++i)
    {
        if(i < obj::posList.size() - 1)
            thickLineRGBA(renderer, obj::posList[i][0], obj::posList[i][1], obj::posList[i + 1][0], obj::posList[i + 1][1], 10, 0, 0, 0, 255);
    }

    for(int i { static_cast<int>(obj::obler.size()) - 1}; i>=0; --i)
    {
        obj::obler[i].obelMove(deltaTime, obj::posList);
        obj::obler[i].countDown -= 1 * deltaTime;

        obj::obler[i].drawObel(renderer);

        if(obj::obler[i].reachedPoint == obj::posList.size() - 1 || obj::obler[i].hp <= 0)
        {
            obj::obler[i].cleanUp();
            obj::obler.erase(obj::obler.begin() + i);
        }
    }

    buttons = DrawTowerMenu(mouseX, mouseY, renderer, windowWidth, windowHeight);
    if(isBuilding)
    {
        for(int i { }; i<obj::towers.size() - 1; ++i)
        {
            obj::towers[i].draw(renderer);
            obj::obler = obj::towers[i].skyd(obj::obler, deltaTime);
        }
        obj::towers[obj::towers.size() - 1].billed.x = mouseX;
        obj::towers[obj::towers.size() - 1].billed.y = mouseY;
        obj::towers[obj::towers.size() - 1].draw(renderer);
    } else
    {
        for(int i { }; i<obj::towers.size(); ++i)
        {
            obj::towers[i].draw(renderer);
            obj::obler = obj::towers[i].skyd(obj::obler, deltaTime);
        }
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
    menuRect.x = windowWidth;
    menuRect.y = 0;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &menuRect);
    buttonList = DrawGrid(renderer, menuRect.w, menuRect.h, mouseX, mouseY, 2, 1, 2, 40, menuRect.x, 0, { "1 dps", "10 dps" });

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
    int buttonWidth { (width - offset * (collums + 1)) / collums };

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
