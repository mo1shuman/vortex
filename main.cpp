#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <SDL.h>

struct Scene {

    // Path to a Scene
    std::string Path;

    // Layer Number
    int layer;
};

struct Component {
    virtual void update(SDL_Renderer* renderer) = 0;
};

struct Object {
    int id;
    std::vector<Component*> components;
};

std::vector<Object> batch0;
std::vector<Object> batch2;
std::vector<Scene> SceneBatch;

std::string line;
std::string prefix;

struct Rect_Render : Component {
    SDL_Rect rect;
    int r;
    int g;
    int b;
    void update(SDL_Renderer* renderer) {
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

void load_queue(const std::string& Path) {
    Scene Scene;
    std::ifstream map_file(Path);
    std::string map_raw((std::istreambuf_iterator<char>(map_file)), std::istreambuf_iterator<char>());
    std::istringstream map_iss(map_raw);
    while (getline(map_iss, line)) {
        std::istringstream iss(line);
        iss >> prefix;
        if (prefix == "//") { continue; }
        else {
        iss >> Scene.Path >> Scene.layer;
        SceneBatch.push_back(Scene);
        }
    }
    map_file.close();
}

void load_scene(const Scene& Scene) {
    std::ifstream scene_file(Scene.Path);
    std::string scene_Raw((std::istreambuf_iterator<char>(scene_file)), std::istreambuf_iterator<char>());
    std::istringstream scene_iss(scene_Raw);
    while (getline(scene_iss, line)) {
        Object obj;
        std::istringstream iss(line);
        iss >> prefix;
        if (prefix == "obj") {
            iss >> obj.id;
            if (Scene.layer == 0) batch0.push_back(obj);
            if (Scene.layer == 2) batch2.push_back(obj);
        }
        if (prefix == "com") {
            std::string name;
            iss >> name;
            Object prev_Obj;
            if (Scene.layer == 0) prev_Obj = batch0.back();
            if (Scene.layer == 2) prev_Obj = batch2.back(); // Get a reference to the last added object
            if (name == "Rect_Render") {
                Rect_Render* rect_Render = new Rect_Render;
                iss >> rect_Render->rect.x >> rect_Render->rect.y >> rect_Render->rect.w >> rect_Render->rect.h >> rect_Render->r >> rect_Render->g >> rect_Render->b;
                prev_Obj.components.push_back(rect_Render); // Add the component to the object
            }
        }
    }
    scene_file.close();
}


// Main Function optimization needed when render issue is fixed.
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Vortex", 0, 0, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP); // Create a window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    load_queue("resources.map");
    load_scene(SceneBatch[0]);
    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (const auto& obj : batch2) {
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        for (const auto& obj : batch0) {
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
