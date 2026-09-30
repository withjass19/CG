#ifndef BALL_H
#define BALL_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Ball {
public:
    Mesh sphereMesh;
    Mesh shadowMesh;
    Texture ballTexture;

    glm::vec3 position;
    glm::vec3 velocity;
    glm::mat4 rotationMatrix;

    float radius;       // 0.24m (standard NBA size 7 basketball radius)
    float mass;         // 0.62kg
    bool isShot;
    bool isScored;
    bool hasHitRimOrBackboard;

    glm::vec3 initialPosition;

    Ball(float r = 0.24f);
    ~Ball();

    void init();
    void reset(glm::vec3 startPos);
    void update(float deltaTime, float gravity = 9.81f);
    void draw(const Shader& shader) const;
    void drawShadow(const Shader& shader) const;
};

#endif
