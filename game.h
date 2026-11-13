#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

class Game
{
    public:
        Game();
        ~Game();

        bool init();
        void runLoop();
        void shutdown(); 

    private:
        void processInput();
        void updateGame();
        void render();
        void renderText(const std::string&, float x, float y);

        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        TTF_Font* mFont;
        bool isRunning;

        SDL_FRect mPaddle1;
        SDL_FRect mPaddle2;
        SDL_FRect mBall;
        float ballVelocityX;
        float ballVelocityY;

        int player1Score;
        int player2Score;

};
