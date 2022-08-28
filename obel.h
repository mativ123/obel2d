class Obel
{
    float pi { 3.14159265359 };

    public:
        Image billed;
        float x { };
        float y { };
        int speed { };
        int reachedPoint { };
        float countDown { };

        void drawObel(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }

        void obelMove(float deltaTime, std::vector<std::array<int, 2>> posList)
        {numericalnumerical
            if(countDown <= 0)
            {
                //billed.x += std::round(speed * deltaTime);
                float angle = std::tan((posList[reachedPoint + 1][0] - posList[reachedPoint][0]) / std::tan(posList[reachedPoint + 1][1] - posList[reachedPoint][1]));
                x += speed * deltaTime * std::cos(angle * pi / 180);
                y += speed * deltaTime * std::sin(angle * pi / 180);
            }

            billed.x = x;
            billed.y = y;

            if(x == posList[reachedPoint][0] && y == posList[reachedPoint][1])
                ++reachedPoint;
        }
};
