#ifndef HOOP_H
#define HOOP_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Hoop {
public:
    Mesh poleMesh;
    Mesh armMesh;
    Mesh backboardMesh;
    Mesh backboardBorderMesh;
    Mesh rimMesh;
    Mesh netMesh;
    Texture netTexture;

    glm::vec3 position;     // Position of hoop base on court (0, 0, -13.5)
    glm::vec3 rimCenter;    // Exact 3D center of rim ring (0, 3.05, -12.425)
    float rimRadius;        // 0.225m (45cm diameter)
    float rimHeight;        // 3.05m
    float backboardZ;       // Z plane of backboard face (-12.65)
    glm::vec2 backboardSize;// Width 1.8m, Height 1.05m

    Hoop(glm::vec3 pos = glm::vec3(0.0f, 0.0f, -12.425f));
    ~Hoop();

    void init();
    void draw(const Shader& shader, bool passOpaqueOnly = false) const;
    void drawTransparent(const Shader& shader) const;
};

#endif
