//
// Created by shams on 8/7/24.
//

#ifndef PHYZIX_SCENE_H
#define PHYZIX_SCENE_H


#include <cstdint>
#include <cstdio>
#include <algorithm>
#include "Drawable.h"

class Scene {
private:
    uint16_t x_size;
    uint16_t y_size;
    Drawable ** drawables = nullptr;
    size_t drawableCount = 0;
    size_t drawableCapacity = 0;

    void resizeDrawables(size_t newCapacity) {
        auto ** newDrawables = new Drawable*[newCapacity];

        if (drawables) {
            std::copy(drawables, drawables + drawableCount, newDrawables);
            delete[] drawables;
        }

        drawables = newDrawables;
        drawableCapacity = newCapacity;
    }

public:
    Scene(uint16_t x_size, uint16_t y_size) {
        this->x_size = x_size;
        this->y_size = y_size;
    };

    void addDrawable(Drawable* drawable) {
        if (drawableCount == drawableCapacity) {
            resizeDrawables(drawableCapacity == 0 ? 1 : drawableCapacity * 2);
        }
        drawables[drawableCount++] = drawable;
    }

    void drawAllDrawables() const {
        for (size_t i = 0; i < drawableCount; ++i) {
            drawables[i]->draw();
        }
    }
};


#endif //PHYZIX_SCENE_H