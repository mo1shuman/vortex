#define SDL_MAIN_HANDLED
#include <string>
#include <iostream>
#include <vector>
#include <algorithm>
#include <SDL.h>
#include <SDL_audio.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

bool running = false;

struct World {
    std::string Name;
    std::string Path;
    int Layer = 0;
};

class Component {
public:
    virtual ~Component() {}
    virtual void update() = 0;
    virtual std::string getComponentName() const = 0; // Virtual function to get the component name
    int getComponentID() {
        return COMPONENTID;
    }
    void setComponentID(int id) {
        COMPONENTID = id;
    }
private:
    int COMPONENTID;
};

struct Vector2 {
    float x;
    float y;
    bool KeepAspect = false;
};

struct Color {
    int r;
    int g;
    int b;
    int a;
};

class RectRenderer : public Component {
    public:
    std::string Name = "RectRenderer";
    SDL_Rect rect;
    Color color;
    std::string getComponentName() const override {
        return "RectRenderer";
    }
    void update() override {
        // Implement update logic
    }
};

class ImageRenderer : public Component {
    public:
    std::string Name = "ImageRenderer";
    SDL_Rect rect;
    std::string Path = "";
    Vector2 scale;
    std::string getComponentName() const override {
        return "ImageRenderer"; // Return the component name
    }
};

struct GameObject {
    std::string Name;
    int ID;
    std::vector<Component*> components;
    ~GameObject() {
        for (auto component : components) {
            delete component;
        }
    }
};

std::vector<GameObject> CurrentBatch;
std::vector<GameObject> UI_Batch;

int main(int argc, char* argv[]) {
    if ((SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    if (TTF_Init() != -1) {
        SDL_Log("Unable to initialize TTF: %s", TTF_GetError());
    }
    
    // Create a window for main loop
    // [ - IMPORTANT CHANGE - ] Change name for engine by reading from a resource file
    SDL_Window* window = SDL_CreateWindow("Vortex Engine", 0, 0, 800, 600, SDL_WINDOW_FULLSCREEN_DESKTOP);
    if (!window) {
        SDL_Log("Unable to create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Unable to create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    running = true;

    // Create an SDL Event
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }

            // Key Input
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    default:
                    break;
                }
            }
        }

        // Simulate Stuff
        for (const auto& obj : CurrentBatch) {
            for (auto component : obj.components) {
                component->update();
            }
        }
        for (const auto& obj : UI_Batch) {
            for (auto component : obj.components) {
                component->update();
            }
        }

        // Render stuff for CurrentBatch (Layer 2)
        for (const auto& obj : CurrentBatch) {
            for (auto component : obj.components) {
                std::string componentName = component->getComponentName();
                if (componentName == "RectRenderer") {
                    RectRenderer* rectRenderer = dynamic_cast<RectRenderer*>(component);
                    if (rectRenderer) {
                        SDL_SetRenderDrawColor(renderer, rectRenderer->color.r, rectRenderer->color.g, rectRenderer->color.b, rectRenderer->color.a);
                        SDL_RenderFillRect(renderer, &rectRenderer->rect);
                    }
                } else if (componentName == "ImageRenderer") {
                    ImageRenderer* imageRenderer = dynamic_cast<ImageRenderer*>(component);
                    if (imageRenderer) {
                        // Load the image
                        SDL_Surface* surface = IMG_Load(imageRenderer->Path.c_str());
                        if (!surface) {
                            std::cerr << "Unable to load image: " << IMG_GetError() << std::endl;
                            continue; // Skip rendering if image loading failed
                        }

                        // Create texture from surface
                        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface); // Free the surface after creating texture
                        if (!texture) {
                            std::cerr << "Unable to create texture from surface: " << SDL_GetError() << std::endl;
                            continue; // Skip rendering if texture creation failed
                        }

                        // Set rendering parameters
                        SDL_Rect dstRect;
                        dstRect.x = static_cast<int>(imageRenderer->rect.x * imageRenderer->scale.x);
                        dstRect.y = static_cast<int>(imageRenderer->rect.y * imageRenderer->scale.y);
                        dstRect.w = static_cast<int>(imageRenderer->rect.w * imageRenderer->scale.x);
                        dstRect.h = static_cast<int>(imageRenderer->rect.h * imageRenderer->scale.y);

                        // Render texture
                        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
                        SDL_DestroyTexture(texture);
                    }
                }
            }
        }
        // Render stuff for UI_Batch (Layer 0, top layer)
        for (const auto& obj : UI_Batch) {
            for (auto component : obj.components) {
                std::string componentName = component->getComponentName();
                if (componentName == "RectRenderer") {
                    RectRenderer* rectRenderer = dynamic_cast<RectRenderer*>(component);
                    if (rectRenderer) {
                        SDL_SetRenderDrawColor(renderer, rectRenderer->color.r, rectRenderer->color.g, rectRenderer->color.b, rectRenderer->color.a);
                        SDL_RenderFillRect(renderer, &rectRenderer->rect);
                    }
                } else if (componentName == "ImageRenderer") {
                    ImageRenderer* imageRenderer = dynamic_cast<ImageRenderer*>(component);
                    if (imageRenderer) {
                        // Load the image
                        SDL_Surface* surface = IMG_Load(imageRenderer->Path.c_str());
                        if (!surface) {
                            std::cerr << "Unable to load image: " << IMG_GetError() << std::endl;
                            continue; // Skip rendering if image loading failed
                        }

                        // Create texture from surface
                        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface); // Free the surface after creating texture
                        if (!texture) {
                            std::cerr << "Unable to create texture from surface: " << SDL_GetError() << std::endl;
                            continue; // Skip rendering if texture creation failed
                        }

                        // Set rendering parameters
                        SDL_Rect dstRect;
                        dstRect.x = static_cast<int>(imageRenderer->rect.x * imageRenderer->scale.x);
                        dstRect.y = static_cast<int>(imageRenderer->rect.y * imageRenderer->scale.y);
                        dstRect.w = static_cast<int>(imageRenderer->rect.w * imageRenderer->scale.x);
                        dstRect.h = static_cast<int>(imageRenderer->rect.h * imageRenderer->scale.y);

                        // Render texture
                        SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
                        SDL_DestroyTexture(texture);
                    }
                }
                
            }
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

class Application {
    void Quit() {
        running = false;
    }
};

class ObjectUtils {
public:
    void AddComponent(GameObject& object, Component* component) {
        int id = object.components.size() + 1;
        component->setComponentID(id);
        object.components.push_back(component);
    }
    void RemoveComponent(GameObject& object, int componentID) {
        auto iter = std::remove_if(object.components.begin(), object.components.end(), 
            [componentID](Component* component) {
                return component->setComponentID(componentID);
            });
        object.components.erase(iter, object.components.end());
    }
};