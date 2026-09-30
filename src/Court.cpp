#include "Court.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

Court::Court() : width(15.0f), length(28.0f) {}

Court::~Court() {}

void Court::init() {
    // Generate floor mesh
    floorMesh = Mesh::createQuad(width, length);
    floorTexture.createProcedural(TEX_WOOD_FLOOR);

    generateCourtLines();
}

void Court::generateCourtLines() {
    std::vector<Vertex> lineVerts;
    std::vector<GLuint> lineInds;

    float hw = width * 0.5f;
    float hl = length * 0.5f;
    float yOffset = 0.005f; // Slightly above floor to prevent Z-fighting
    glm::vec3 normal(0.0f, 1.0f, 0.0f);
    glm::vec2 uv(0.0f, 0.0f);

    auto addLineQuad = [&](glm::vec3 p1, glm::vec3 p2, float thickness) {
        glm::vec3 dir = glm::normalize(p2 - p1);
        glm::vec3 perp = glm::cross(dir, normal) * (thickness * 0.5f);

        GLuint baseIdx = static_cast<GLuint>(lineVerts.size());

        lineVerts.push_back({p1 - perp + glm::vec3(0, yOffset, 0), normal, uv});
        lineVerts.push_back({p1 + perp + glm::vec3(0, yOffset, 0), normal, uv});
        lineVerts.push_back({p2 + perp + glm::vec3(0, yOffset, 0), normal, uv});
        lineVerts.push_back({p2 - perp + glm::vec3(0, yOffset, 0), normal, uv});

        lineInds.push_back(baseIdx + 0);
        lineInds.push_back(baseIdx + 1);
        lineInds.push_back(baseIdx + 2);
        lineInds.push_back(baseIdx + 0);
        lineInds.push_back(baseIdx + 2);
        lineInds.push_back(baseIdx + 3);
    };

    float th = 0.10f; // Line thickness (10cm)

    // 1. Boundary lines
    addLineQuad(glm::vec3(-hw, 0, -hl), glm::vec3(hw, 0, -hl), th);
    addLineQuad(glm::vec3(hw, 0, -hl), glm::vec3(hw, 0, hl), th);
    addLineQuad(glm::vec3(hw, 0, hl), glm::vec3(-hw, 0, hl), th);
    addLineQuad(glm::vec3(-hw, 0, hl), glm::vec3(-hw, 0, -hl), th);

    // 2. Half court center line
    addLineQuad(glm::vec3(-hw, 0, 0), glm::vec3(hw, 0, 0), th);

    // 3. Key/Paint Areas (Front Hoop & Back Hoop)
    float keyWidth = 4.9f;
    float keyLength = 5.8f;
    float hkw = keyWidth * 0.5f;

    // Near Hoop Key (Z = -hl to -hl + keyLength)
    addLineQuad(glm::vec3(-hkw, 0, -hl), glm::vec3(-hkw, 0, -hl + keyLength), th);
    addLineQuad(glm::vec3(hkw, 0, -hl), glm::vec3(hkw, 0, -hl + keyLength), th);
    addLineQuad(glm::vec3(-hkw, 0, -hl + keyLength), glm::vec3(hkw, 0, -hl + keyLength), th);

    // Far Hoop Key (Z = hl - keyLength to hl)
    addLineQuad(glm::vec3(-hkw, 0, hl), glm::vec3(-hkw, 0, hl - keyLength), th);
    addLineQuad(glm::vec3(hkw, 0, hl), glm::vec3(hkw, 0, hl - keyLength), th);
    addLineQuad(glm::vec3(-hkw, 0, hl - keyLength), glm::vec3(hkw, 0, hl - keyLength), th);

    // 4. Center Circle
    float centerRadius = 1.8f;
    int segments = 48;
    for (int i = 0; i < segments; ++i) {
        float a1 = (float)i / segments * 2.0f * 3.14159f;
        float a2 = (float)(i + 1) / segments * 2.0f * 3.14159f;
        glm::vec3 p1(cosf(a1) * centerRadius, 0, sinf(a1) * centerRadius);
        glm::vec3 p2(cosf(a2) * centerRadius, 0, sinf(a2) * centerRadius);
        addLineQuad(p1, p2, th);
    }

    // 5. Three-Point Arcs (Radius 6.75m from hoop center)
    float hoopZ = -hl + 1.575f;
    float threePtR = 6.75f;
    for (int i = 0; i < segments; ++i) {
        float angleFraction = (float)i / segments;
        float a1 = -3.14159f * 0.48f + angleFraction * 3.14159f * 0.96f;
        float a2 = -3.14159f * 0.48f + ((float)(i + 1) / segments) * 3.14159f * 0.96f;

        glm::vec3 p1(sinf(a1) * threePtR, 0, hoopZ + cosf(a1) * threePtR);
        glm::vec3 p2(sinf(a2) * threePtR, 0, hoopZ + cosf(a2) * threePtR);
        if (p1.z >= -hl && p2.z >= -hl && p1.z <= 0.0f) {
            addLineQuad(p1, p2, th);
        }
    }

    lineMesh = Mesh(lineVerts, lineInds);
}

void Court::draw(const Shader& shader) const {
    // Render Wooden Floor
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setVec3("material.ambient", glm::vec3(0.5f, 0.4f, 0.3f));
    shader.setVec3("material.diffuse", glm::vec3(0.9f, 0.8f, 0.7f));
    shader.setVec3("material.specular", glm::vec3(0.4f, 0.4f, 0.4f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setBool("useTexture", true);

    floorTexture.bind(0);
    shader.setInt("texture_diffuse1", 0);
    floorMesh.draw();

    // Render Court Lines
    shader.setBool("useTexture", false);
    shader.setVec3("material.ambient", glm::vec3(0.9f, 0.9f, 0.9f));
    shader.setVec3("material.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
    shader.setVec3("material.specular", glm::vec3(0.8f, 0.8f, 0.8f));
    lineMesh.draw();
}
