#ifndef PHYSICS_H
#define PHYSICS_H

#include <glm/glm.hpp>
#include <vector>

class Physics {
public:
    static const float GRAVITY; // 9.81 m/s^2

    static glm::vec3 calculateInitialVelocity(float powerPercent, float elevationAngleDeg, float azimuthAngleDeg, glm::vec3 aimTargetDir);

    static std::vector<glm::vec3> generateTrajectory(glm::vec3 startPos, glm::vec3 initialVel, float stepTime = 0.05f, int maxSteps = 40);
};

#endif
