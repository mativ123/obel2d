class ObelTower
{
    public:
        Image billed;
        int dps { };

        void skyd(std::vector<Obel> obler, float deltaTime)
        {
            obler[obler.size() - 1].hp -= dps * deltaTime;
        }

        void draw(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }
};
