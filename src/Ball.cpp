#include "Ball.h"
#include <cmath>

Ball::Ball(float r)
    : radius(r), mass(0.62f), isShot(false), isScored(false), hasHitRimOrBackboard(false) {
    position = glm::vec3(0.0f, 1.8f, 0.0f);
    velocity = glm::vec3(0.0f);
    rotationMatrix = glm::mat4(1.0f);
    initialPosition = position;
}

Ball::~Ball() {}

void Ball::init() {
    sphereMesh = Mesh::createSphere(radius, 32, 24);
    shadowMesh = Mesh::createRing(0.0f, radius * 1.2f, 24);
    ballTexture.createProcedural(TEX_BASKETBALL);
}

void Ball::reset(glm::vec3 startPos) {
    position = startPos;
    initialPosition = startPos;
    velocity = glm::vec3(0.0f);
    rotationMatrix = glm::mat4(1.0f);
    isShot = false;
    isScored = false;
    hasHitRimOrBackboard = false;
}

void Ball::update(float deltaTime, float gravity) {
    if (!isShot) return;

    // Kinematic motion update
    position += velocity * deltaTime;
    velocity.y -= gravity * deltaTime;

    // Calculate spin rotation based on speed and flight vector (backspin)
    float speed = glm::length(velocity);
    if (speed > 0.1f) {
        glm::vec3 spinAxis = glm::normalize(glm::cross(velocity, glm::vec3(0, 1, 0)));
        if (glm::length(spinAxis) > 0.01f) {
            float angle = (speed / radius) * deltaTime * 1.5f;
            rotationMatrix = glm::rotate(glm::mat4(1.0f), angle, spinAxis) * rotationMatrix;
        }
    }
}

void Ball::draw(const Shader& shader) const {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = model * rotationMatrix;

    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.35f, 0.15f, 0.05f));
    shader.setVec3("material.diffuse", glm::vec3(0.95f, 0.45f, 0.1f));
    shader.setVec3("material.specular", glm::vec3(0.4f, 0.4f, 0.4f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setBool("useTexture", true);

    ballTexture.bind(0);
    shader.setInt("texture_diffuse1", 0);
    sphereMesh.draw();
}

void Ball::drawShadow(const Shader& shader) const {
    if (position.y < radius) return;

    // Projected soft shadow on court floor y = 0.002
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, 0.002f, position.z));
    float heightFactor = std::max(0.2f, 1.0f - (position.y / 8.0f));
    model = glm::scale(model, glm::vec3(1.0f + position.y * 0.1f, 1.0f, 1.0f + position.y * 0.1f));

    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
    shader.setVec3("material.diffuse", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setVec3("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
    shader.setFloat("material.shininess", 1.0f);
    shader.setBool("useTexture", false);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    shader.setVec4("colorOverride", glm::vec4(0.0f, 0.0f, 0.0f, 0.45f * heightFactor));

    shadowMesh.draw();

    shader.setVec4("colorOverride", glm::vec4(0.0f)); // reset override
    glDisable(GL_BLEND);
}
