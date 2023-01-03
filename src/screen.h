#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

class Screen{
    

    public:
    SDL_Event e;
    SDL_Window* window;
    SDL_Renderer* renderer;
    std::vector<SDL_FPoint> points;
    Screen(){
        SDL_Init(SDL_INIT_VIDEO);
        SDL_CreateWindowAndRenderer(320*2, 240*2, 0, &window, &renderer);
        SDL_RenderSetScale(renderer,2,2);

    }

    void pixel(float x, float y){
        SDL_FPoint p = {x,y};
        points.push_back(p);
    }

    void show(const std::vector<std::vector< SDL_Vertex>> verts){
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);
        for(auto& a : verts){
            SDL_RenderGeometry( renderer, nullptr, a.data(), a.size(), nullptr, 0 );
        }
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
        for(auto& point : points){
            SDL_RenderDrawPointF(renderer, point.x, point.y);
        }
        SDL_RenderPresent(renderer);
    }

    void clear(){
        points.clear();
    }

    void input(){
        while(SDL_PollEvent(&e)){
            if(e.type == SDL_QUIT){
                SDL_Quit();
                exit(0);
            }
        }
    }



};