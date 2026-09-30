#include "FontRenderer.h"
#include <vector>
#include <iostream>

FontRenderer::FontRenderer() : VAO(0), VBO(0), fontTextureID(0) {}

FontRenderer::~FontRenderer() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (fontTextureID != 0) glDeleteTextures(1, &fontTextureID);
}

void FontRenderer::init() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    generateFontAtlas();
}

void FontRenderer::generateFontAtlas() {
    // 128x128 pixel font atlas (16x16 grid of 8x8 glyphs for ASCII 0-255)
    int atlasW = 128;
    int atlasH = 128;
    std::vector<unsigned char> fontData(atlasW * atlasH, 0);

    // 8x8 bitmap representation for alphanumeric ASCII characters
    static const unsigned char font8x8[96][8] = {
        {0,0,0,0,0,0,0,0}, // ' '
        {24,60,60,24,24,0,24,0}, // '!'
        {102,102,102,0,0,0,0,0}, // '"'
        {102,102,255,102,255,102,102,0}, // '#'
        {24,62,96,60,6,124,24,0}, // '$'
        {99,102,12,24,48,102,198,0}, // '%'
        {60,102,76,56,108,102,61,0}, // '&'
        {24,24,48,0,0,0,0,0}, // '\''
        {12,24,48,48,48,24,12,0}, // '('
        {48,24,12,12,12,24,48,0}, // ')'
        {0,102,60,255,60,102,0,0}, // '*'
        {0,24,24,126,24,24,0,0}, // '+'
        {0,0,0,0,24,24,48,0}, // ','
        {0,0,0,126,0,0,0,0}, // '-'
        {0,0,0,0,0,24,24,0}, // '.'
        {3,6,12,24,48,96,192,0}, // '/'
        {60,102,110,114,102,102,60,0}, // '0'
        {24,56,24,24,24,24,126,0}, // '1'
        {60,102,6,12,24,48,126,0}, // '2'
        {60,102,6,28,6,102,60,0}, // '3'
        {12,28,60,108,126,12,12,0}, // '4'
        {126,96,124,6,6,102,60,0}, // '5'
        {60,102,96,124,102,102,60,0}, // '6'
        {126,6,12,24,48,48,48,0}, // '7'
        {60,102,102,60,102,102,60,0}, // '8'
        {60,102,102,62,6,102,60,0}, // '9'
        {0,24,24,0,0,24,24,0}, // ':'
        {0,24,24,0,0,24,24,48}, // ';'
        {12,24,48,96,48,24,12,0}, // '<'
        {0,0,126,0,126,0,0,0}, // '='
        {48,24,12,6,12,24,48,0}, // '>'
        {60,102,6,12,24,0,24,0}, // '?'
        {60,102,110,110,96,98,60,0}, // '@'
        {24,60,102,102,126,102,102,0}, // 'A'
        {124,102,102,124,102,102,124,0}, // 'B'
        {60,102,96,96,96,102,60,0}, // 'C'
        {120,108,102,102,102,108,120,0}, // 'D'
        {126,96,96,120,96,96,126,0}, // 'E'
        {126,96,96,120,96,96,96,0}, // 'F'
        {60,102,96,110,102,102,60,0}, // 'G'
        {102,102,102,126,102,102,102,0}, // 'H'
        {60,24,24,24,24,24,60,0}, // 'I'
        {30,12,12,12,12,108,56,0}, // 'J'
        {102,108,120,112,120,108,102,0}, // 'K'
        {96,96,96,96,96,96,126,0}, // 'L'
        {99,119,127,107,99,99,99,0}, // 'M'
        {102,118,126,110,102,102,102,0}, // 'N'
        {60,102,102,102,102,102,60,0}, // 'O'
        {124,102,102,124,96,96,96,0}, // 'P'
        {60,102,102,102,110,60,14,0}, // 'Q'
        {124,102,102,124,120,108,102,0}, // 'R'
        {60,102,96,60,6,102,60,0}, // 'S'
        {126,24,24,24,24,24,24,0}, // 'T'
        {102,102,102,102,102,102,60,0}, // 'U'
        {102,102,102,102,102,60,24,0}, // 'V'
        {99,99,99,107,127,119,99,0}, // 'W'
        {102,102,60,24,60,102,102,0}, // 'X'
        {102,102,102,60,24,24,24,0}, // 'Y'
        {126,6,12,24,48,96,126,0}, // 'Z'
        {60,48,48,48,48,48,60,0}, // '['
        {192,96,48,24,12,6,3,0}, // '\'
        {60,12,12,12,12,12,60,0}, // ']'
        {24,60,102,0,0,0,0,0}, // '^'
        {0,0,0,0,0,0,255,0}, // '_'
        {48,24,12,0,0,0,0,0}, // '`'
        {0,0,60,6,62,102,62,0}, // 'a'
        {96,96,124,102,102,102,124,0}, // 'b'
        {0,0,60,96,96,96,60,0}, // 'c'
        {6,6,62,102,102,102,62,0}, // 'd'
        {0,0,60,102,126,96,60,0}, // 'e'
        {28,48,124,48,48,48,48,0}, // 'f'
        {0,0,62,102,102,62,6,124}, // 'g'
        {96,96,124,102,102,102,102,0}, // 'h'
        {24,0,56,24,24,24,60,0}, // 'i'
        {12,0,28,12,12,12,108,56}, // 'j'
        {96,96,108,120,112,108,102,0}, // 'k'
        {56,24,24,24,24,24,60,0}, // 'l'
        {0,0,108,127,127,107,99,0}, // 'm'
        {0,0,124,102,102,102,102,0}, // 'n'
        {0,0,60,102,102,102,60,0}, // 'o'
        {0,0,124,102,102,124,96,96}, // 'p'
        {0,0,62,102,102,62,6,6}, // 'q'
        {0,0,124,102,96,96,96,0}, // 'r'
        {0,0,62,96,60,6,124,0}, // 's'
        {32,32,120,32,32,36,24,0}, // 't'
        {0,0,102,102,102,102,62,0}, // 'u'
        {0,0,102,102,102,60,24,0}, // 'v'
        {0,0,99,107,127,119,99,0}, // 'w'
        {0,0,102,60,24,60,102,0}, // 'x'
        {0,0,102,102,102,62,6,124}, // 'y'
        {0,0,126,12,24,48,126,0}, // 'z'
        {14,24,24,112,24,24,14,0}, // '{'
        {24,24,24,24,24,24,24,0}, // '|'
        {112,24,24,14,24,24,112,0}, // '}'
        {44,88,0,0,0,0,0,0}, // '~'
        {0,0,0,0,0,0,0,0}
    };

    for (int charIdx = 0; charIdx < 96; ++charIdx) {
        int cellX = (charIdx % 16) * 8;
        int cellY = (charIdx / 16) * 8;

        for (int py = 0; py < 8; ++py) {
            unsigned char row = font8x8[charIdx][py];
            for (int px = 0; px < 8; ++px) {
                bool pixelOn = (row & (1 << (7 - px))) != 0;
                int atlasIndex = (cellY + py) * atlasW + (cellX + px);
                fontData[atlasIndex] = pixelOn ? 255 : 0;
            }
        }
    }

    glGenTextures(1, &fontTextureID);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, atlasW, atlasH, 0, GL_RED, GL_UNSIGNED_BYTE, fontData.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void FontRenderer::renderText(const Shader& shader, const std::string& text, float x, float y, float scale, glm::vec4 color) const {
    shader.use();
    shader.setVec4("textColor", color);
    shader.setBool("isSolidQuad", false);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    shader.setInt("textTexture", 0);

    glBindVertexArray(VAO);

    float glyphW = 8.0f * scale;
    float glyphH = 16.0f * scale;

    for (char c : text) {
        int charIdx = (unsigned char)c - 32;
        if (charIdx < 0 || charIdx >= 96) charIdx = 0;

        float u0 = (charIdx % 16) * 8.0f / 128.0f;
        float v0 = (charIdx / 16) * 8.0f / 128.0f;
        float u1 = u0 + 8.0f / 128.0f;
        float v1 = v0 + 8.0f / 128.0f;

        float vertices[6][4] = {
            { x,          y + glyphH, u0, v0 },
            { x,          y,          u0, v1 },
            { x + glyphW, y,          u1, v1 },

            { x,          y + glyphH, u0, v0 },
            { x + glyphW, y,          u1, v1 },
            { x + glyphW, y + glyphH, u1, v0 }
        };

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += glyphW;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void FontRenderer::renderQuad(const Shader& shader, float x, float y, float w, float h, glm::vec4 color) const {
    shader.use();
    shader.setVec4("textColor", color);
    shader.setBool("isSolidQuad", true);

    float vertices[6][4] = {
        { x,     y + h, 0.0f, 1.0f },
        { x,     y,     0.0f, 0.0f },
        { x + w, y,     1.0f, 0.0f },

        { x,     y + h, 0.0f, 1.0f },
        { x + w, y,     1.0f, 0.0f },
        { x + w, y + h, 1.0f, 1.0f }
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}
