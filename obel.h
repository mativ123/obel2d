class Obel
{
    public:
        Image billed;
        int speed { };
        int reachedPoint { };

        void drawObel(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }

        void obelMove()
        {
            billed.x += speed;
        }
};
