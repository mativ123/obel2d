#pragma once

class Obel
{
    double pi { 3.14159265359 };

    double calcAngle(std::vector<std::array<int, 2>> posList)
    {
        double a { static_cast<double>(posList[reachedPoint + 1][1] - posList[reachedPoint][1]) };
        double b { static_cast<double>(posList[reachedPoint + 1][0] - posList[reachedPoint][0]) };

        double A { std::atan2(a, b) };

        return A;
    }

    public:
        Image billed;
        Text hpText;
        double x { };
        double y { };
        int speed { };
        int reachedPoint { };
        float countDown { };
        float hp { };
        float damage { };

        void drawObel(SDL_Renderer *renderer)
        {
            hpText.updateSize(renderer);
            hpText.x = billed.x + (billed.w / 2 - hpText.w / 2);
            hpText.y = billed.y - 20;
            hpText.textString = std::to_string(static_cast<int>(std::round(hp)));
            hpText.draw(renderer);

            billed.draw(renderer);
        }

        void obelMove(float deltaTime, std::vector<std::array<int, 2>> posList)
        {
            double angle { };

            if(countDown <= 0)
            {
                angle = calcAngle(posList);

                x += speed * deltaTime * std::cos(angle);
                y += speed * deltaTime * std::sin(angle);
                billed.x = x - billed.w / 2;
                billed.y = y - billed.h / 2;
            }

            bool xPM { x > posList[reachedPoint + 1][0] - 3 && x < posList[reachedPoint + 1][0] + 3 };
            bool yPM { y > posList[reachedPoint + 1][1] - 3 && y < posList[reachedPoint + 1][1] + 3 };
            if(xPM && yPM)
            {
                ++reachedPoint;
                x = posList[reachedPoint][0];
                y = posList[reachedPoint][1];
            }
        }

        void initObel(std::vector<std::array<int, 2>> posList, SDL_Renderer *renderer)
        {
            x = posList[0][0];
            y = posList[0][1];
        }
};

namespace obel
{
    Obel obel1;
    obel1.speed = 20;
    obel1.hp = 10;
    obel1.damage = 2;

    Obel obel2;
    obel2.speed = 50;
    obel2.hp = 20;
    obel2.damage = 4;

    Obel obel3;
    obel3.speed = 70;
    obel3.hp = 30;
    obel3.damage = 7;

    Obel obel4;
    obel4.speed = 100;
    obel4.hp = 50;
    obel4.damage = 10;

    Obel obel5;
    obel5.speed = 150;
    obel5.hp = 70;
    obel5.damage = 15;

    std::vector<Obel> obelTypes { obel1, obel2, obel3, obel4, obel5 };
}
