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

        void drawObel(SDL_Renderer *renderer)
        {
            hpText.updateSize(renderer);
            hpText.x = billed.x + (billed.w / 2 - hpText.w / 2);
            hpText.y = billed.y - 50;
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

            Text text;
            text.color = { 0, 0, 0 };
            text.fontSize = 50;
            text.init(renderer, "arial.ttf");
            hpText = text;
        }

        void cleanUp()
        {
            hpText.destroyFont();
        }
};
