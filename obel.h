class Obel
{
    public:
        Image billed;
        int speed { };
        int reachedPoint { };
        float countDown { };

        void drawObel(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }

        void obelMove(float deltaTime)
        {
            if(countDown <= 0)
                billed.x += std::round(speed * deltaTime);
        }
};
