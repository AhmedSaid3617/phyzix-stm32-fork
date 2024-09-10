#include <algorithm>
#include "app/engine/Scene.h"
#include "Application.h"
#include "lcd_st7735/graphics/gfx.h"
#include "app/engine/LineSegmentBoundary.h"
#include "app/engine/CircleBoundary.h"
#include "app/math/Math.h"

Scene * Application::scene;
Camera * Application::camera;
uint32_t Application::last_tick;
uint32_t Application::last_render_tick;

void handle_physics(Scene * scene, float dt) {
    for (size_t i = 0; i < scene->drawableCount; ++i) {
        auto I = scene->drawables[i];

        if (auto * it = dynamic_cast<DynamicObject*>(I)) {
            printf("v_y: %f\n", it->v.y);

            it->s = it->s + it->v * dt;
            it->v = it->v + it->a * dt;
            it->updateBoundaryFunction(it->boundaries, it->s);

            it->a = it->forceFunction(it->s.x, it->s.y, it->a.x, it->s.y, it->m) * (1 / it->m);

            auto scene_collision = Boundary::intersects(it->boundaries, scene->boundaries);
            if (scene_collision.intersects) {
                //it->x = 50;
                //it->y = 50;
                //it->v_x = -it->v_x;
                //it->v_x = 0;
                //it->v_y = -it->v_y * scene_collision.other->e;
            }

//            for (size_t j = i; j < scene->drawableCount; ++j) {
//                auto J = scene->drawables[j];
//                if (auto * other = dynamic_cast<DynamicObject*>(J)) {
//                    auto object_collision = Boundary::intersects(it->boundaries, it->boundaryCount, other->boundaries, other->boundaryCount);
//                    if (object_collision.intersects) {
//                        it->v_x = -it->v_x;
//                        it->v_y = -it->v_y;
//
//                        other->v_x = -other->v_x;
//                        other->v_y = -other->v_y;
//                    }
//                }
//            }
        }
    }
}

void Application::update() {
    last_tick = Tick;

    while (true) {
        if (scene) {
            handle_physics(scene, (Tick - Application::last_tick) * 10);
        }

        last_tick = Tick;

        os_schedule();
    }
}

void Application::render() {
    last_render_tick = Tick;

    while (true) {
        if (scene && camera && (Tick - last_render_tick > 10)) {
            // TODO: handle camera movement over the scene; clipping objects

            //fillScreen(BLACK);

            scene->forEachDrawable([] (Drawable * d) {
                if (auto * object = dynamic_cast<DynamicObject *>(d)) {
                    object->blackOut();

                    object->prev_x = object->s.x;
                    object->prev_y = object->s.y;
                }

                d->draw();
            });

            last_render_tick = Tick;
        }

        os_schedule();
    }
}

void Application::game() {
    OS_TASK_LOCK();
    fillScreen(BLACK);

    ST7735_WriteString(0, 18 + 1, "HEllo", Font_11x18, RED,BLACK);
    // TODO: move logic to other class or something


    // Beginning scene
    scene = new Scene(160, 256);
    scene->boundaries.list = new Boundary * [4];
    scene->boundaries.count = 4;
    scene->boundaries.list[0] = new LineSegmentBoundary(0.0,0.0,0.0,160.0);
    scene->boundaries.list[1] = new LineSegmentBoundary(0.0,0.0,128,0.0);
    scene->boundaries.list[1]->e = 0.5;
    scene->boundaries.list[2] = new LineSegmentBoundary(128,0.0,128,160.0);
    scene->boundaries.list[3] = new LineSegmentBoundary(0.0,160,128,160.0);
    camera = new Camera(160, 128);

    scene->addDrawable(new StaticObject([] {
        ST7735_FillRectangle(0, 0, 25,25,GREEN);
    }));

    auto * box = new DynamicObject(
            [] (float x, float y) {
                ST7735_FillRectangle(x, y, 25,25,BLUE);
            },
            [] (float x, float y) {
                // TODO fill only coordinates, for scene to re render background for example
                ST7735_FillRectangle(x, y, 25,25,BLACK);
            },[] (Boundaries& boundaries, Vector& s) {
                ((CircleBoundary *) boundaries.list[0])->x = s.x + 25;
                ((CircleBoundary *) boundaries.list[0])->y = s.y;
            },
            50,120);

    box->boundaries.count = 1;
    box->boundaries.list = new Boundary * [1];
    box->boundaries.list[0] = new CircleBoundary(12.5, 12.5, 5);
    box->forceFunction = [](float x, float y, float v_x, float v_y, float m) -> Vector {
        return Vector(0, -9.81 * m * 8 * 1 / 100 * 1 / 100);
    };

    scene->addDrawable(box);

    OS_TASK_UNLOCK();

    while (true) {
    }

    // Monitor current level
    // Switch between scenes
}
