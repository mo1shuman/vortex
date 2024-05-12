#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <sstream>
#include <algorithm>
#include <SDL.h>

bool running = false;
bool dev = false;

struct World {
    std::string Name;
    std::string Path;
    int Layer = 0;
};

std::vector<World> worldBatch;

class Component {
public:
    virtual void update(SDL_Renderer* renderer) = 0;
};

struct GameObject {
    std::string Name;
    int id = 0;
    std::vector<Component*> components;
    ~GameObject() {
        for (auto component : components) {
            delete component;
        }
    }
};
std::vector<GameObject> CurrentBatch;
std::vector<GameObject> UI_Batch;
// RectRenderer Class based on component class
class RectRenderer : public Component {
    public:
    std::string Name = "RectRenderer";
    SDL_Rect rect;
    int r;
    int g;
    int b;
    int a;
    void update(SDL_Renderer* renderer) override {
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderFillRect(renderer, &rect);
    }
};
void LoadQueue(const std::string& Path) {
    std::ifstream mapFile(Path);
    std::string map_Raw((std::istreambuf_iterator<char>(mapFile)), std::istreambuf_iterator<char>());
    std::istringstream map_raw(map_Raw);
    std::string line;
    while (getline(map_raw, line)) {
        World world;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "//") {
            // If line is comment, skip.
        }
        else {
            iss >> world.Name >> world.Path >> world.Layer;
            worldBatch.push_back(world);
        }
    }
    mapFile.close();
}
void AddComponent(GameObject& object, Component* component) {
    int id = static_cast<int>(object.components.size()) + 1;
    object.components.push_back(component);
    // Update the object in CurrentBatch
    for (auto& obj : CurrentBatch) {
        if (obj.id == object.id) {
            obj = object;
            break; // Exit loop after updating the object
        }
    }
}
void LoadWorld(const World world) {
    std::vector<GameObject> batch;
    std::string Path = world.Path;
    std::ifstream worldFile(Path);
    std::string world_Raw((std::istreambuf_iterator<char>(worldFile)), std::istreambuf_iterator<char>());
    std::istringstream world_raw(world_Raw);
    std::string line;
    std::string prefix;
    while (getline(world_raw, line)) {
        GameObject obj;
        std::istringstream iss(line);
        iss >> prefix;
        if (prefix == "obj") {
            iss >> obj.Name >> obj.id;
            if (world.Layer == 0) UI_Batch.push_back(obj);
            if (world.Layer == 2) CurrentBatch.push_back(obj);
        }
        if (prefix == "com") {
            std::string name;
            iss >> name;
            if (name == "RectRenderer") {
                RectRenderer* rectRenderer = new RectRenderer;
                iss >> rectRenderer->rect.x >> rectRenderer->rect.y >> rectRenderer->rect.w >> rectRenderer->rect.h
                >> rectRenderer->r >> rectRenderer->g >> rectRenderer->b >> rectRenderer->a;
                GameObject prev_Obj;
                if (world.Layer == 0) prev_Obj = UI_Batch.back();
                if (world.Layer == 2) prev_Obj = CurrentBatch.back();
                AddComponent(prev_Obj, rectRenderer);
            }
        }
    }
    worldFile.close();
    return;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window = SDL_CreateWindow("Vortex", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1900, 1080, SDL_WINDOW_FULLSCREEN); // Create a window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    running = true;
    LoadQueue("resources.map");
    World CurrentWorld = worldBatch.front();
    LoadWorld(CurrentWorld);
    SDL_Event event;
    running = true;
    std::cout << "Starting main loop";
    while (running) {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        for (const auto& obj : CurrentBatch) {
            for (const auto& component : obj.components) {
                component->update(renderer);
            }
        }
        for (const auto& obj : UI_Batch) {
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
