#include "Collision.h"
#include <cmath>
#include <iostream>

bool CollisionEngine::checkGround(Ball& ball, const Court& court) {
    (void)court;
    if (ball.position.y - ball.radius <= 0.0f) {
        ball.position.y = ball.radius; // Clamp to ground level
        if (ball.velocity.y < 0.0f) {
            ball.velocity.y = -ball.velocity.y * 0.72f; // Restitution coefficient 0.72
            ball.velocity.x *= 0.85f; // Friction
            ball.velocity.z *= 0.85f;
            return true;
        }
    }
    return false;
}

bool CollisionEngine::checkBackboard(Ball& ball, const Hoop& hoop) {
    float bbZ = hoop.backboardZ;
    float hw = hoop.backboardSize.x * 0.5f;
    float hh = hoop.backboardSize.y * 0.5f;
    float bbY = hoop.rimHeight + 0.45f;

    // Check if ball intersects backboard plane rectangle
    bool withinX = std::abs(ball.position.x - hoop.position.x) <= (hw + ball.radius * 0.5f);
    bool withinY = std::abs(ball.position.y - bbY) <= (hh + ball.radius * 0.5f);

    if (withinX && withinY) {
        // Front face reflection (Z direction)
        if (std::abs(ball.position.z - bbZ) <= ball.radius) {
            if (ball.velocity.z < 0.0f) {
                ball.position.z = bbZ + ball.radius;
                ball.velocity.z = -ball.velocity.z * 0.68f; // Backboard bounce reflection
                ball.velocity.x += (ball.position.x - hoop.position.x) * 0.5f;
                ball.hasHitRimOrBackboard = true;
                return true;
            }
        }
    }
    return false;
}

bool CollisionEngine::checkRim(Ball& ball, const Hoop& hoop) {
    glm::vec3 rimC = hoop.rimCenter;
    float rimR = hoop.rimRadius;
    float rimTubeR = 0.02f;

    // Calculate distance from ball center to rim ring plane Y
    float dy = ball.position.y - rimC.y;
    if (std::abs(dy) > (ball.radius + rimTubeR + 0.05f)) return false;

    // Projected 2D distance in XZ plane to rim center
    float dx = ball.position.x - rimC.x;
    float dz = ball.position.z - rimC.z;
    float dist2D = std::sqrt(dx * dx + dz * dz);

    // Distance from ball center to closest point on rim circle torus ring
    float closestX = rimC.x + (dx / (dist2D + 0.0001f)) * rimR;
    float closestZ = rimC.z + (dz / (dist2D + 0.0001f)) * rimR;
    glm::vec3 closestPointOnRim(closestX, rimC.y, closestZ);

    glm::vec3 collisionVec = ball.position - closestPointOnRim;
    float dist3D = glm::length(collisionVec);

    if (dist3D < (ball.radius + rimTubeR)) {
        glm::vec3 normal = glm::normalize(collisionVec);
        ball.position = closestPointOnRim + normal * (ball.radius + rimTubeR);

        // Reflect velocity vector across collision normal with bounce coefficient
        ball.velocity = glm::reflect(ball.velocity, normal) * 0.65f;
        ball.hasHitRimOrBackboard = true;
        return true;
    }

    return false;
}

bool CollisionEngine::checkBasketScored(Ball& ball, const Hoop& hoop, glm::vec3 prevPos) {
    if (ball.isScored) return false;

    float rimY = hoop.rimHeight;
    float rimR = hoop.rimRadius;
    glm::vec3 rimC = hoop.rimCenter;

    // Detect ball passing downward through rim plane (y_prev >= rimY and y_curr < rimY)
    if (prevPos.y >= rimY && ball.position.y < rimY) {
        float dx = ball.position.x - rimC.x;
        float dz = ball.position.z - rimC.z;
        float distXZ = std::sqrt(dx * dx + dz * dz);

        // Ball center must pass within rim opening (allowing margin for ball size)
        if (distXZ <= (rimR - ball.radius * 0.15f)) {
            ball.isScored = true;
            return true;
        }
    }

    return false;
}

CollisionResult CollisionEngine::processAll(Ball& ball, const Hoop& hoop, const Court& court, glm::vec3 prevPos) {
    if (checkBasketScored(ball, hoop, prevPos)) {
        return COL_BASKET_SCORED;
    }
    if (checkRim(ball, hoop)) {
        return COL_RIM;
    }
    if (checkBackboard(ball, hoop)) {
        return COL_BACKBOARD;
    }
    if (checkGround(ball, court)) {
        return COL_GROUND;
    }
    return COL_NONE;
}
