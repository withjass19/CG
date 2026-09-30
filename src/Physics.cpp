#include "Physics.h"
#include <cmath>
#include <glm/gtc/matrix_transform.hpp>

const float Physics::GRAVITY = 9.81f;

glm::vec3 Physics::calculateInitialVelocity(float powerPercent, float elevationAngleDeg, float azimuthAngleDeg, glm::vec3 aimTargetDir) {
    // Map power percentage (0% - 100%) to velocity magnitude (7.5 m/s - 14.5 m/s)
    float minSpeed = 7.5f;
    float maxSpeed = 14.5f;
    float speed = minSpeed + (powerPercent / 100.0f) * (maxSpeed - minSpeed);

    float pitchRad = glm::radians(elevationAngleDeg);
    float yawRad = glm::radians(azimuthAngleDeg);

    // Compute base direction vector facing downcourt (towards -Z)
    glm::vec3 baseDir = glm::normalize(aimTargetDir);
    glm::vec3 rightDir = glm::normalize(glm::cross(baseDir, glm::vec3(0, 1, 0)));

    // Rotate horizontal direction by azimuth angle
    glm::vec3 horizDir = glm::normalize(baseDir * cosf(yawRad) + rightDir * sinf(yawRad));

    // Combine horizontal and vertical components with elevation pitch angle
    glm::vec3 velocity;
    velocity.x = horizDir.x * cosf(pitchRad) * speed;
    velocity.y = sinf(pitchRad) * speed;
    velocity.z = horizDir.z * cosf(pitchRad) * speed;

    return velocity;
}

std::vector<glm::vec3> Physics::generateTrajectory(glm::vec3 startPos, glm::vec3 initialVel, float stepTime, int maxSteps) {
    std::vector<glm::vec3> points;
    glm::vec3 pos = startPos;
    glm::vec3 vel = initialVel;

    for (int i = 0; i < maxSteps; ++i) {
        float t = i * stepTime;
        // Projectile motion equations:
        // x(t) = x0 + vx * t
        // y(t) = y0 + vy * t - 0.5 * g * t^2
        // z(t) = z0 + vz * t
        glm::vec3 currentPoint;
        currentPoint.x = startPos.x + initialVel.x * t;
        currentPoint.y = startPos.y + initialVel.y * t - 0.5f * GRAVITY * t * t;
        currentPoint.z = startPos.z + initialVel.z * t;

        points.push_back(currentPoint);

        if (currentPoint.y < 0.1f && i > 2) {
            break; // Stop preview trajectory at court ground level
        }
    }

    return points;
}
