SDL_Texture *LoadTexture(std::string filePath, SDL_Renderer *renderTarget)
{
    SDL_Texture *texture { nullptr };
    SDL_Surface *surface { IMG_Load(filePath.c_str()) };
    if(surface == NULL)
    {
        std::cout << "ERROR: " << SDL_GetError() << '\n';
    } else
    {
        texture = SDL_CreateTextureFromSurface(renderTarget, surface);
    }

    SDL_FreeSurface(surface);

    return texture;
}
