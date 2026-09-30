#include "Mesh.h"
#include <cmath>
#include <iostream>
#include <utility>

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

Mesh::Mesh() : VAO(0), VBO(0), EBO(0) {}

Mesh::Mesh(const std::vector<Vertex>& verts, const std::vector<GLuint>& inds)
    : VAO(0), VBO(0), EBO(0), vertices(verts), indices(inds) {
    setupMesh();
}

Mesh::Mesh(Mesh&& other) noexcept
    : VAO(other.VAO), VBO(other.VBO), EBO(other.EBO),
      vertices(std::move(other.vertices)), indices(std::move(other.indices)) {
    other.VAO = 0;
    other.VBO = 0;
    other.EBO = 0;
}

Mesh& Mesh::operator=(Mesh&& other) noexcept {
    if (this != &other) {
        cleanup();
        VAO = other.VAO;
        VBO = other.VBO;
        EBO = other.EBO;
        vertices = std::move(other.vertices);
        indices = std::move(other.indices);
        other.VAO = 0;
        other.VBO = 0;
        other.EBO = 0;
    }
    return *this;
}

void Mesh::cleanup() {
    if (VAO != 0) { glDeleteVertexArrays(1, &VAO); VAO = 0; }
    if (VBO != 0) { glDeleteBuffers(1, &VBO); VBO = 0; }
    if (EBO != 0) { glDeleteBuffers(1, &EBO); EBO = 0; }
}

Mesh::~Mesh() {
    cleanup();
}

void Mesh::setupMesh() {
    cleanup();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    if (!indices.empty()) {
        glGenBuffers(1, &EBO);
    }

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    if (!indices.empty()) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
    }

    // Position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

    // Normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));

    // TexCoords attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}

void Mesh::draw(GLenum mode) const {
    if (VAO == 0) return;
    glBindVertexArray(VAO);
    if (!indices.empty()) {
        glDrawElements(mode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawArrays(mode, 0, static_cast<GLsizei>(vertices.size()));
    }
    glBindVertexArray(0);
}

Mesh Mesh::createSphere(float radius, int sectors, int stacks) {
    std::vector<Vertex> verts;
    std::vector<GLuint> inds;

    float x, y, z, xy;
    float nx, ny, nz, lengthInv = 1.0f / radius;
    float s, t;

    float sectorStep = 2.0f * M_PI / sectors;
    float stackStep = M_PI / stacks;
    float sectorAngle, stackAngle;

    for (int i = 0; i <= stacks; ++i) {
        stackAngle = M_PI / 2.0f - i * stackStep;
        xy = radius * cosf(stackAngle);
        y = radius * sinf(stackAngle);

        for (int j = 0; j <= sectors; ++j) {
            sectorAngle = j * sectorStep;

            x = xy * cosf(sectorAngle);
            z = xy * sinf(sectorAngle);

            nx = x * lengthInv;
            ny = y * lengthInv;
            nz = z * lengthInv;

            s = (float)j / sectors;
            t = (float)i / stacks;

            Vertex vertex;
            vertex.Position = glm::vec3(x, y, z);
            vertex.Normal = glm::vec3(nx, ny, nz);
            vertex.TexCoords = glm::vec2(s, t);
            verts.push_back(vertex);
        }
    }

    int k1, k2;
    for (int i = 0; i < stacks; ++i) {
        k1 = i * (sectors + 1);
        k2 = k1 + sectors + 1;

        for (int j = 0; j < sectors; ++j, ++k1, ++k2) {
            if (i != 0) {
                inds.push_back(k1);
                inds.push_back(k2);
                inds.push_back(k1 + 1);
            }
            if (i != (stacks - 1)) {
                inds.push_back(k1 + 1);
                inds.push_back(k2);
                inds.push_back(k2 + 1);
            }
        }
    }

    Mesh mesh(verts, inds);
    return mesh;
}

Mesh Mesh::createCube(float width, float height, float depth) {
    float w = width * 0.5f;
    float h = height * 0.5f;
    float d = depth * 0.5f;

    std::vector<Vertex> verts = {
        // Front face
        {{-w, -h,  d}, { 0.0f,  0.0f,  1.0f}, {0.0f, 0.0f}},
        {{ w, -h,  d}, { 0.0f,  0.0f,  1.0f}, {1.0f, 0.0f}},
        {{ w,  h,  d}, { 0.0f,  0.0f,  1.0f}, {1.0f, 1.0f}},
        {{-w,  h,  d}, { 0.0f,  0.0f,  1.0f}, {0.0f, 1.0f}},
        // Back face
        {{ w, -h, -d}, { 0.0f,  0.0f, -1.0f}, {0.0f, 0.0f}},
        {{-w, -h, -d}, { 0.0f,  0.0f, -1.0f}, {1.0f, 0.0f}},
        {{-w,  h, -d}, { 0.0f,  0.0f, -1.0f}, {1.0f, 1.0f}},
        {{ w,  h, -d}, { 0.0f,  0.0f, -1.0f}, {0.0f, 1.0f}},
        // Top face
        {{-w,  h,  d}, { 0.0f,  1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ w,  h,  d}, { 0.0f,  1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ w,  h, -d}, { 0.0f,  1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w,  h, -d}, { 0.0f,  1.0f,  0.0f}, {0.0f, 1.0f}},
        // Bottom face
        {{-w, -h, -d}, { 0.0f, -1.0f,  0.0f}, {0.0f, 0.0f}},
        {{ w, -h, -d}, { 0.0f, -1.0f,  0.0f}, {1.0f, 0.0f}},
        {{ w, -h,  d}, { 0.0f, -1.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w, -h,  d}, { 0.0f, -1.0f,  0.0f}, {0.0f, 1.0f}},
        // Right face
        {{ w, -h,  d}, { 1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{ w, -h, -d}, { 1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{ w,  h, -d}, { 1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{ w,  h,  d}, { 1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}},
        // Left face
        {{-w, -h, -d}, {-1.0f,  0.0f,  0.0f}, {0.0f, 0.0f}},
        {{-w, -h,  d}, {-1.0f,  0.0f,  0.0f}, {1.0f, 0.0f}},
        {{-w,  h,  d}, {-1.0f,  0.0f,  0.0f}, {1.0f, 1.0f}},
        {{-w,  h, -d}, {-1.0f,  0.0f,  0.0f}, {0.0f, 1.0f}}
    };

    std::vector<GLuint> inds = {
        0, 1, 2,  0, 2, 3,
        4, 5, 6,  4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    Mesh mesh(verts, inds);
    return mesh;
}

Mesh Mesh::createCylinder(float radius, float height, int sectors) {
    std::vector<Vertex> verts;
    std::vector<GLuint> inds;

    float halfH = height * 0.5f;
    float sectorStep = 2.0f * M_PI / sectors;

    // Side vertices
    for (int i = 0; i <= sectors; ++i) {
        float angle = i * sectorStep;
        float x = cosf(angle);
        float z = sinf(angle);
        float u = (float)i / sectors;

        // Top edge
        verts.push_back({{x * radius, halfH, z * radius}, {x, 0.0f, z}, {u, 1.0f}});
        // Bottom edge
        verts.push_back({{x * radius, -halfH, z * radius}, {x, 0.0f, z}, {u, 0.0f}});
    }

    for (int i = 0; i < sectors; ++i) {
        int idx = i * 2;
        inds.push_back(idx);
        inds.push_back(idx + 1);
        inds.push_back(idx + 3);

        inds.push_back(idx);
        inds.push_back(idx + 3);
        inds.push_back(idx + 2);
    }

    Mesh mesh(verts, inds);
    return mesh;
}

Mesh Mesh::createTorus(float mainRadius, float tubeRadius, int mainSectors, int tubeSectors) {
    std::vector<Vertex> verts;
    std::vector<GLuint> inds;

    float mainStep = 2.0f * M_PI / mainSectors;
    float tubeStep = 2.0f * M_PI / tubeSectors;

    for (int i = 0; i <= mainSectors; ++i) {
        float mainAngle = i * mainStep;
        float cosMain = cosf(mainAngle);
        float sinMain = sinf(mainAngle);

        for (int j = 0; j <= tubeSectors; ++j) {
            float tubeAngle = j * tubeStep;
            float cosTube = cosf(tubeAngle);
            float sinTube = sinf(tubeAngle);

            float x = (mainRadius + tubeRadius * cosTube) * cosMain;
            float y = tubeRadius * sinTube;
            float z = (mainRadius + tubeRadius * cosTube) * sinMain;

            float nx = cosTube * cosMain;
            float ny = sinTube;
            float nz = cosTube * sinMain;

            float u = (float)i / mainSectors;
            float v = (float)j / tubeSectors;

            verts.push_back({{x, y, z}, {nx, ny, nz}, {u, v}});
        }
    }

    for (int i = 0; i < mainSectors; ++i) {
        for (int j = 0; j < tubeSectors; ++j) {
            int first = (i * (tubeSectors + 1)) + j;
            int second = first + tubeSectors + 1;

            inds.push_back(first);
            inds.push_back(second);
            inds.push_back(first + 1);

            inds.push_back(second);
            inds.push_back(second + 1);
            inds.push_back(first + 1);
        }
    }

    Mesh mesh(verts, inds);
    return mesh;
}

Mesh Mesh::createQuad(float width, float height) {
    float w = width * 0.5f;
    float h = height * 0.5f;

    std::vector<Vertex> verts = {
        {{-w, 0.0f,  h}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{ w, 0.0f,  h}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
        {{ w, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
        {{-w, 0.0f, -h}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}}
    };

    std::vector<GLuint> inds = {
        0, 1, 2, 0, 2, 3
    };

    Mesh mesh(verts, inds);
    return mesh;
}

Mesh Mesh::createRing(float innerRadius, float outerRadius, int sectors) {
    std::vector<Vertex> verts;
    std::vector<GLuint> inds;

    float step = 2.0f * M_PI / sectors;

    for (int i = 0; i <= sectors; ++i) {
        float angle = i * step;
        float cosA = cosf(angle);
        float sinA = sinf(angle);
        float u = (float)i / sectors;

        // Inner vertex
        verts.push_back({{cosA * innerRadius, 0.0f, sinA * innerRadius}, {0.0f, 1.0f, 0.0f}, {u, 0.0f}});
        // Outer vertex
        verts.push_back({{cosA * outerRadius, 0.0f, sinA * outerRadius}, {0.0f, 1.0f, 0.0f}, {u, 1.0f}});
    }

    for (int i = 0; i < sectors; ++i) {
        int idx = i * 2;
        inds.push_back(idx);
        inds.push_back(idx + 1);
        inds.push_back(idx + 3);

        inds.push_back(idx);
        inds.push_back(idx + 3);
        inds.push_back(idx + 2);
    }

    Mesh mesh(verts, inds);
    return mesh;
}
