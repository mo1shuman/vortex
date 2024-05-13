#ifndef BASE_H
#define BASE_H
#include <iostream>
#include <vector>
#include "SDL.h"

struct Component {
    virtual void update(SDL_Renderer* renderer) = 0;
};

struct Object {
    int id;
    std::vector<Component*> components;
};

struct Scene {

    // Path to a Scene
    std::string Path;

    // Layer Number
    int layer;
};
#endif
