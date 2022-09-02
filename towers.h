class ObelTower
{
    public:
        Image billed;
        int dps { };
        float moneyPower;
        int type { };

        std::vector<Obel> skyd(std::vector<Obel> obler, float deltaTime)
        {
            if(obler.size() > 0)
                obler[0].hp -= dps * deltaTime;

            return obler;
        }

        float moneyPlus(float money, float deltaTime)
        {
            money += moneyPower * deltaTime;

            return money;
        }

        void draw(SDL_Renderer *renderer)
        {
            billed.draw(renderer);
        }
};
