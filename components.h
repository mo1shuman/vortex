#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "SDL.h"
#include <vector>
#include <iostream>
#include "base.h"
#include "main.h"

int moveSpeed = 2;
int verticalSpeed = 1;
bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;

struct Rect_Render : Component {
    SDL_Rect rect;
    int r;
    int g;
    int b;
    int a;
    int Inherited_Layer;
    void update(SDL_Renderer* renderer) {
        if (Inherited_Layer == 2){
            if (moveLeft == true) {
                rect.x = rect.x + moveSpeed;
            }
            if (moveRight == true) {
                rect.x = rect.x - moveSpeed;
            }
            if (moveUp == true) {
                rect.y = rect.y + verticalSpeed;
            }
            if (moveDown == true) {
                rect.y = rect.y - verticalSpeed;
            }
        }
        if (Inherited_Layer == 1){
            // 0.5 is for parallax effect
            if (moveLeft == true) {
                rect.x = rect.x + (moveSpeed * 0.5);
            }
            if (moveRight == true) {
                rect.x = rect.x - (moveSpeed * 0.5);
            }
            if (moveUp == true) {
                rect.y = rect.y + (verticalSpeed * 0.5);
            }
            if (moveDown == true) {
                rect.y = rect.y - (verticalSpeed * 0.5);
            }
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, 255);
        SDL_RenderFillRect(renderer, &rect);
    }
};

struct Load_Scene : Component {
    Scene scene;
    bool loaded;
    void update() {
        if (!loaded) load::load_scene(scene);
        loaded = true;
    }
};
#endif