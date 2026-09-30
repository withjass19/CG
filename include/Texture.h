#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>
#include <vector>
#include <iostream>

enum TextureType {
    TEX_WOOD_FLOOR,
    TEX_BASKETBALL,
    TEX_NET,
    TEX_SOLID_WHITE
};

class Texture {
public:
    GLuint ID;
    int width, height, nrChannels;

    Texture();
    ~Texture();

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    void cleanup();
    bool loadFromFile(const std::string& path);
    void createProcedural(TextureType type);
    void bind(unsigned int unit = 0) const;

private:
    void setupGLTexture(unsigned char* data, int w, int h, int channels);
};

#endif
