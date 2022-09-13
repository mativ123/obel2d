#pragma once

namespace obj
{
    std::vector<Obel> obler { };
    std::vector<ObelTower> towers;
    std::vector<std::array<int, 2>> posList;
    std::vector<Wave> waves { { { 10, 0}, { 5, 1 } }, 1 };
}

namespace img
{
    Image obelBilled;

    Image victorBilled;

    Image williamBilled;
}

namespace font
{
    Text statFont;

    Text buttonText;

    Text obelHp;
}
