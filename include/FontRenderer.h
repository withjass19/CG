#ifndef FONT_RENDERER_H
#define FONT_RENDERER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "Shader.h"

class FontRenderer {
public:
    GLuint VAO, VBO;
    GLuint fontTextureID;

    FontRenderer();
    ~FontRenderer();

    void init();
    void renderText(const Shader& shader, const std::string& text, float x, float y, float scale, glm::vec4 color) const;
    void renderQuad(const Shader& shader, float x, float y, float w, float h, glm::vec4 color) const;

private:
    void generateFontAtlas();
};

#endif
