#include "SDL_render.h"
#include "SDL_video.h"
#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <SDL.h>

struct Scene {
    std::string Name;
    std::string Path;
};

struct Component {
    virtual void update(SDL_Renderer* renderer) = 0;
};

struct Object {
    std::string Name;
    int id;
    std::vector<Component*> components;
};

std::vector<Object> Render_Batch;
std::vector<Scene> SceneBatch;

std::string line;
std::string prefix;

struct Rect_Render : public Component {
    SDL_Rect rect;
    int r;
    int g;
    int b;
    int a;
    void update(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
    }
};
void Load_Queue(const std::string& Path) {
    Scene Scene;
    std::ifstream map_file(Path);
    std::string map_raw((std::istreambuf_iterator<char>(map_file)), std::istreambuf_iterator<char>());
    std::istringstream map_iss(map_raw);
    while (getline(map_iss, line)) {
        std::istringstream iss(line);
        iss >> prefix;
        if (prefix == "//") { continue; }
        iss >> Scene.Name >> Scene.Path;
        SceneBatch.push_back(Scene);
    }
    map_file.close();
}

void Load_Scene(const Scene Scene) {
    std::ifstream scene_file(Scene.Path);
    std::string scene_Raw((std::istreambuf_iterator<char>(scene_file)), std::istreambuf_iterator<char>());
    std::istringstream scene_iss(scene_Raw);
    while (getline(scene_iss, line)) {
        Object obj;
        std::istringstream iss(line);
        iss >> prefix;
        if (prefix == "obj") {
            iss >> obj.Name >> obj.id;
            Render_Batch.push_back(obj);
        }
        if (prefix == "com") {
            std::string name;
            iss >> name;
            Object prev_Obj;
            prev_Obj = Render_Batch.back();
            if (name == "Rect_Render") {
                Rect_Render* rect_Render = new Rect_Render;
                iss >> rect_Render->rect.x >> rect_Render->rect.y >> rect_Render->rect.w >> rect_Render->rect.h >> rect_Render->r >> rect_Render->g >> rect_Render->b >> rect_Render->a;
            }
        }
    }
    scene_file.close();
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Vortex", 0, 0, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP); // Create a window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    Load_Queue("resources.map");
    Load_Scene(SceneBatch[0]);
    SDL_Event event;
    while (true) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (const auto& obj : Render_Batch) {
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        SDL_RenderPresent(renderer);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
