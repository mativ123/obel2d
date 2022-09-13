class Text
{
    TTF_Font *font { nullptr };
    SDL_Rect textRect;

    public:
        SDL_Color color { 0, 0, 0, SDL_ALPHA_OPAQUE };
        std::string textString;
        int x { 0 };
        int y { 0 };
        int w { 0 };
        int h { 0 };
        int fontSize { 20 };
        int wrapSize { 0 };
        bool textBox { false };

        void init(SDL_Renderer *rendere, const char *src)
        {
            font = TTF_OpenFont(src, fontSize);

            textRect.x = x;
            textRect.y = y;
        }

        void draw(SDL_Renderer *rendere)
        {
            textRect.x = x;
            textRect.y = y;

            SDL_Surface *textSurface = TTF_RenderUTF8_Blended_Wrapped(font, textString.c_str(), color, wrapSize);
            SDL_Texture *text = SDL_CreateTextureFromSurface(rendere, textSurface);

            SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);
            w = textRect.w;
            h = textRect.h;

            //work in progress
            if(textBox)
            {
                SDL_SetRenderDrawColor(rendere, 0, 0, 0, SDL_ALPHA_OPAQUE);
                SDL_RenderDrawLine(rendere, textRect.x + textRect.w, textRect.y, textRect.x + textRect.w, textRect.y + textRect.h);
                SDL_SetRenderDrawColor(rendere, 255, 255, 255, SDL_ALPHA_OPAQUE);
            }
            
            SDL_FreeSurface(textSurface);
            textSurface = nullptr;

            SDL_RenderCopy(rendere, text, NULL, &textRect);

            SDL_DestroyTexture(text);
            text = nullptr;
        }

        void destroyFont()
        {
            TTF_CloseFont(font);
            font = nullptr;
        }

        void updateSize(SDL_Renderer *rendere)
        {
            SDL_Surface *textSurface = TTF_RenderUTF8_Blended_Wrapped(font, textString.c_str(), color, wrapSize);
            SDL_Texture *text = SDL_CreateTextureFromSurface(rendere, textSurface);

            SDL_QueryTexture(text, NULL, NULL, &textRect.w, &textRect.h);
            w = textRect.w;
            h = textRect.h;

            SDL_FreeSurface(textSurface);
            textSurface = nullptr;

            SDL_DestroyTexture(text);
            text = nullptr;
        }
};
