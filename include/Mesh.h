#ifndef MESH_H
#define MESH_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

class Mesh {
public:
    GLuint VAO, VBO, EBO;
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;

    Mesh();
    Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
    ~Mesh();

    // Disable copy semantics to avoid GPU buffer deletion on temporaries
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    // Enable move semantics for safe ownership transfer
    Mesh(Mesh&& other) noexcept;
    Mesh& operator=(Mesh&& other) noexcept;

    void setupMesh();
    void cleanup();
    void draw(GLenum mode = GL_TRIANGLES) const;

    static Mesh createSphere(float radius, int sectors = 32, int stacks = 16);
    static Mesh createCube(float width, float height, float depth);
    static Mesh createCylinder(float radius, float height, int sectors = 24);
    static Mesh createTorus(float mainRadius, float tubeRadius, int mainSectors = 32, int tubeSectors = 16);
    static Mesh createQuad(float width, float height);
    static Mesh createRing(float innerRadius, float outerRadius, int sectors = 32);
};

#endif
