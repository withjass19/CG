#ifndef COLLISION_H
#define COLLISION_H

#include "Ball.h"
#include "Hoop.h"
#include "Court.h"

enum CollisionResult {
    COL_NONE,
    COL_GROUND,
    COL_BACKBOARD,
    COL_RIM,
    COL_BASKET_SCORED
};

class CollisionEngine {
public:
    static bool checkGround(Ball& ball, const Court& court);
    static bool checkBackboard(Ball& ball, const Hoop& hoop);
    static bool checkRim(Ball& ball, const Hoop& hoop);
    static bool checkBasketScored(Ball& ball, const Hoop& hoop, glm::vec3 prevPos);
    static CollisionResult processAll(Ball& ball, const Hoop& hoop, const Court& court, glm::vec3 prevPos);
};

#endif
