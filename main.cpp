#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <SDL.h>

// Structure for handling scenes.
// Contains: Path (string)
// Is used as a reference to a scene file.
// Does not contain objects for memory's sake.
struct Scene {
    std::string Path;
};

// Component structure.
// Contains: Virtual update template (void)
struct Component {
    virtual void update(SDL_Renderer* renderer) = 0;
};

// Object structure.
// Contains: id (int), component vector (pointers)
struct Object {
    int id;
    std::vector<Component*> components;
};

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
// Render batch, used for rendering Level 2 (Game Scene)
std::vector<Object> batch;

// Scene batch, used for containing scene paths.
std::vector<Scene> scene_batch;

/* This is stuff for istringstream dummies in Load_Queue and Load_Scene */
std::string line;
std::string prefix;

// Loads the map. Reads from resources.map in the root directory to extract paths to scenes.
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
        iss >> Scene.Path;
        scene_batch.push_back(Scene);
        }
    }
    map_file.close();
}

// Loads scenes. Add component data extraction methods if necessary
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
            batch.push_back(obj);
        }
        if (prefix == "com") {
            std::string name;
            iss >> name;
            // Gets the latest object added to the batch.
            Object& prev_obj = batch.back();
            if (name == "Rect_Render") {
                Rect_Render* rect_Render = new Rect_Render;
                iss >> rect_Render->rect.x >> rect_Render->rect.y >> rect_Render->rect.w >> rect_Render->rect.h >> rect_Render->r >> rect_Render->g >> rect_Render->b;
                prev_obj.components.push_back(rect_Render);
            }
        }
    }
    scene_file.close();
}


// Main Function optimization needed when render issue is fixed.
int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Vortex", 0, 0, 1920, 1080, SDL_WINDOW_FULLSCREEN); // Create a window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    load_queue("resources.map");
    load_scene(scene_batch[0]);
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
        for (const auto obj : batch) {
            for (const auto component : obj.components) {
                component->update(renderer);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(12);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
