class Image
{
    SDL_Rect imageRect;
    SDL_Texture *texture { nullptr };
    public:
        int w { 0 };
        int h { 0 };
        int x { 0 };
        int y { 0 };
        float rot { 0.0f };

    void init(SDL_Renderer *rendere, std::string src)
    {
        texture = LoadTexture(src, rendere);
        SDL_QueryTexture(texture, NULL, NULL, &w, &h);
        imageRect = { x, y, w, h };
    }
    void draw(SDL_Renderer *rendere)
    {
        imageRect = { x, y, w, h };

        SDL_RenderCopyEx(rendere, texture, NULL, &imageRect, rot, NULL, SDL_FLIP_NONE);
    }

    void destroyTexture()
    {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }

    void resizeKA(char side, int inpW, int inpH)
    {
        if(side == 'w')
        {
            h = round(inpW * static_cast<float>(h) / static_cast<float>(w));
            w = inpW;
        } else if(side == 'h')
        {
            w = round(inpH * static_cast<float>(w) / static_cast<float>(h));
            h = inpH;
        } else
        {
            std::cout << "error: " << side << " is not a valid side\n";
        }
    }
};
