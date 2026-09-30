#include "Hoop.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Hoop::Hoop(glm::vec3 pos)
    : position(pos), rimRadius(0.225f), rimHeight(3.05f), backboardSize(1.8f, 1.05f) {
    rimCenter = glm::vec3(position.x, rimHeight, position.z);
    backboardZ = rimCenter.z - rimRadius - 0.15f; // Mounted 15cm behind rim back edge
}

Hoop::~Hoop() {}

void Hoop::init() {
    // 1. Pole: Vertical steel post behind baseline
    poleMesh = Mesh::createCylinder(0.12f, 3.8f, 24);

    // 2. Extension Arm
    armMesh = Mesh::createCylinder(0.08f, 1.2f, 16);

    // 3. Backboard: Glass plate quad
    backboardMesh = Mesh::createCube(backboardSize.x, backboardSize.y, 0.04f);
    backboardBorderMesh = Mesh::createCube(0.59f, 0.45f, 0.05f); // Inner target square box

    // 4. Rim: Metallic red torus ring
    rimMesh = Mesh::createTorus(rimRadius, 0.018f, 32, 16);

    // 5. Net: Tapered cylinder netting
    netMesh = Mesh::createCylinder(rimRadius * 0.95f, 0.45f, 24);
    netTexture.createProcedural(TEX_NET);
}

void Hoop::draw(const Shader& shader, bool passOpaqueOnly) const {
    glm::mat4 model;

    // --- 1. Support Pole ---
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, 1.9f, backboardZ - 0.6f));
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.1f, 0.1f, 0.12f));
    shader.setVec3("material.diffuse", glm::vec3(0.2f, 0.25f, 0.3f));
    shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setBool("useTexture", false);
    poleMesh.draw();

    // --- 2. Extension Arm ---
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, rimHeight + 0.1f, backboardZ - 0.3f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    shader.setMat4("model", model);
    armMesh.draw();

    // --- 3. Metallic Red Rim ---
    model = glm::mat4(1.0f);
    model = glm::translate(model, rimCenter);
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.4f, 0.05f, 0.05f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.15f, 0.1f));
    shader.setVec3("material.specular", glm::vec3(0.8f, 0.6f, 0.6f));
    shader.setFloat("material.shininess", 64.0f);
    rimMesh.draw();

    // --- 4. Backboard Target Box ---
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, rimHeight + 0.20f, backboardZ + 0.001f));
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.8f, 0.1f, 0.1f));
    shader.setVec3("material.diffuse", glm::vec3(1.0f, 0.2f, 0.2f));
    shader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    backboardBorderMesh.draw();

    if (!passOpaqueOnly) {
        drawTransparent(shader);
    }
}

void Hoop::drawTransparent(const Shader& shader) const {
    glm::mat4 model;

    // --- 5. Net ---
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    model = glm::mat4(1.0f);
    model = glm::translate(model, rimCenter - glm::vec3(0, 0.225f, 0));
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.8f, 0.8f, 0.8f));
    shader.setVec3("material.diffuse", glm::vec3(0.95f, 0.95f, 0.95f));
    shader.setVec3("material.specular", glm::vec3(0.2f, 0.2f, 0.2f));
    shader.setFloat("material.shininess", 16.0f);
    shader.setBool("useTexture", true);
    netTexture.bind(0);
    shader.setInt("texture_diffuse1", 0);
    netMesh.draw();

    // --- 6. Glass Backboard (Semi-Transparent) ---
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position.x, rimHeight + 0.45f, backboardZ));
    shader.setMat4("model", model);
    shader.setVec3("material.ambient", glm::vec3(0.4f, 0.5f, 0.6f));
    shader.setVec3("material.diffuse", glm::vec3(0.7f, 0.85f, 0.95f));
    shader.setVec3("material.specular", glm::vec3(0.9f, 0.9f, 0.95f));
    shader.setFloat("material.shininess", 96.0f);
    shader.setBool("useTexture", false);

    // Render glass backboard with partial transparency
    glDepthMask(GL_FALSE); // Don't write to depth buffer for transparent glass plane
    backboardMesh.draw();
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
}
