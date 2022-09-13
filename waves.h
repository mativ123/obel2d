#pragma once

struct waveSpurt
{
    int type { };
    int amount { };
};

class wave
{
    float obelScaling { 10 };
    float spurtScaling { 2 };

    float minRan { 0.5 };
    float topRan { 1.5 };

    std::mt19937 seed { std::random_device{}() };
    public:
        std::vector<Obel> obelTypes;
        std::vector<waveSpurt> spurts;
        int spurtIndex { };
        float spawnRate { 1 };

        void newWave()
        {
            spurts.clear();
            spurtIndex = 0;
            std::uniform_real_distribution distri { minRan, topRan };
            for(int i { }; i<std::round(distri(seed) * spurtScaling); ++i)
            {
                waveSpurt temp;
                temp.amount = std::round(distri(seed) * obelScaling);
                temp.type = 0;
                spurts.push_back(temp);
            }
        }
};
