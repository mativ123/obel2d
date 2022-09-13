#pragma once

struct Wave
{
    // how many obels of different types per wave
    std::vector<std::array<int, 2> obelList;

    // stores amount of seconds between each obel
    float spawnRate { };
};
