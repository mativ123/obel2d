class ObelTower
{
    public:
        Image billed;
        int dps { };

        std::vector<Obel> skyd(std::vector<Obel> obler, float deltaTime)
        {
            if(obler.size() > 0)
                obler[0].hp -= dps * deltaTime;

            return obler;
        }

        void draw(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }
};
