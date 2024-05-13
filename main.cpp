#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <SDL.h>
#include "components.h"
#include "base.h"

bool dev = false;

std::string line;
std::string prefix;


std::vector<Object> batch0;
std::vector<Object> batch1;
std::vector<Object> batch2;
std::vector<Scene> scene_batch;


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
        if (dev) std::cout << "Loaded Scene -----\nScene Path: " << Scene.Path << "\nScene Layer: " << Scene.layer << std::endl;
        scene_batch.push_back(Scene);
        }
    }
    map_file.close();
}

void load_scene(const Scene& Scene) {
    std::ifstream scene_file(Scene.Path);
    std::string scene_Raw((std::istreambuf_iterator<char>(scene_file)), std::istreambuf_iterator<char>());
    std::istringstream scene_iss(scene_Raw);
    while (getline(scene_iss, line)) {
        // Dummy Object
        Object obj;

        std::istringstream iss(line);
        iss >> prefix;
        if (prefix != "com" && prefix != "//") {
            iss >> obj.id;
            if (dev) std::cout << "Added object with id: " << obj.id << " from scene " << Scene.Path << "\nWill be added to layer: "<< Scene.layer << std::endl;
            if (Scene.layer == 0) batch0.push_back(obj);
            if (Scene.layer == 1) batch1.push_back(obj);
            if (Scene.layer == 2) batch2.push_back(obj);
        }
        if (prefix == "com") {
            std::string name;
            iss >> std::ws >> name;
            if (dev) std::cout << "Added component " << name << " to previously loaded object at scene " << Scene.Path << std::endl;

            // For Rect_Render
            if (name == "Rect_Render") {
                if (dev) std::cout << "Adding Rect Render" << std::endl;
                Rect_Render* rect_Render = new Rect_Render;
                iss >> rect_Render->rect.x >> rect_Render->rect.y >> rect_Render->rect.w >> rect_Render->rect.h >> rect_Render->r >> rect_Render->g >> rect_Render->b >> rect_Render->a;
                rect_Render->Inherited_Layer = Scene.layer;
                if (Scene.layer == 0) {
                    Object& prev_obj = batch0.back();
                    prev_obj.components.push_back(rect_Render);
                }
                if (Scene.layer == 1) {
                    Object& prev_obj = batch1.back();
                    prev_obj.components.push_back(rect_Render);
                }
                if (Scene.layer == 2) {
                    Object& prev_obj = batch2.back();
                    prev_obj.components.push_back(rect_Render);
                }
                if (dev) std::cout << "Rect Render added." << std::endl;
            }
            if (name == "Load_Scene") {
                if (dev) std::cout << "Adding Load_Scene" << std::endl;
                LoadScene* load_Scene = new LoadScene;
            }
        }
    }
    scene_file.close();
}


// Main Function optimization needed when render issue is fixed.
int main(int argc, char* argv[]) {
    // Developer logging
    std::vector<std::string> arguments(argv, argv + argc);
    for (size_t i = 1; i < arguments.size(); ++i) { // Start from 1 to skip program name
        if (arguments[i] == "--dev") {
            dev = true;
            std::cout << "Dev logging active" << std::endl;
            break; // No need to continue searching once found
        }
    }

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Vortex", 0, 0, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP); // Create a window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    load_queue("resources.map");
    load_scene(scene_batch[0]);
    if (dev) std::cout << "Size of batch2: " << batch2.size() << std::endl;
    if (dev) std::cout << "Size of batch0: " << batch1.size() << std::endl;
    if (dev) std::cout << "Size of batch0: " << batch0.size() << std::endl;
    if (dev) std::cout << "Size of scene_batch: " << scene_batch.size() << std::endl;
    SDL_Event event;
    bool running = true;
    while (running) {
        while (SDL_PollEvent(&event)){
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // OnKeyDown
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                case SDLK_a:
                    moveLeft = true;
                    break;
                case SDLK_d:
                    moveRight = true;
                    break;
                case SDLK_w:
                    moveUp = true;
                    break;
                case SDLK_s:
                    moveDown = true;
                    break;
                default:
                    break;
                }
            }
            // OnKeyUp
            if (event.type == SDL_KEYUP) {
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                        moveLeft = false;
                        break;
                    case SDLK_d:
                        moveRight = false;
                        break;
                    case SDLK_w:
                        moveUp = false;
                        break;
                    case SDLK_s:
                        moveDown = false;
                        break;
                    default:
                        break;
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (const auto& obj : batch2) {
            if (obj.components.empty()) std::cout << "Components of " << obj.id << " is empty, skipping." << std::endl;
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        for (const auto& obj : batch0) {
            if (obj.components.empty()) std::cout << "Components of " << obj.id << " is empty, skipping." << std::endl;
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        for (const auto& obj : batch1) {
            if (obj.components.empty()) std::cout << "Components of " << obj.id << " is empty, skipping." << std::endl;
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(8);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
