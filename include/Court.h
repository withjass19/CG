#ifndef COURT_H
#define COURT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Mesh.h"
#include "Texture.h"
#include "Shader.h"

class Court {
public:
    Mesh floorMesh;
    Mesh lineMesh;
    Mesh keyAreaMesh;
    Mesh centerCircleMesh;
    Mesh threePointArcMesh;
    Texture floorTexture;

    float width;  // X axis (15.0m)
    float length; // Z axis (28.0m)

    Court();
    ~Court();

    void init();
    void draw(const Shader& shader) const;

private:
    void generateCourtLines();
};

#endif
