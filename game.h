#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "game.h"
#include <iostream>
#include <string>

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

        bool isPaused;

        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        TTF_Font* mFont;
        bool isStarted;
        bool isRunning;
        bool hasHitOccured;

        SDL_FRect mPaddle1;
        SDL_FRect mPaddle2;
        SDL_FRect mBall;
        float ballVelocityX;
        float ballVelocityY;

        uint64_t mLastTicks;
        float deltaTime;

        int player1Score;
        int player2Score;

};
