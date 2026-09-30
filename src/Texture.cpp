#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "Texture.h"
#include <cmath>
#include <utility>

Texture::Texture() : ID(0), width(0), height(0), nrChannels(0) {}

Texture::Texture(Texture&& other) noexcept
    : ID(other.ID), width(other.width), height(other.height), nrChannels(other.nrChannels) {
    other.ID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept {
    if (this != &other) {
        cleanup();
        ID = other.ID;
        width = other.width;
        height = other.height;
        nrChannels = other.nrChannels;
        other.ID = 0;
    }
    return *this;
}

void Texture::cleanup() {
    if (ID != 0) {
        glDeleteTextures(1, &ID);
        ID = 0;
    }
}

Texture::~Texture() {
    cleanup();
}

bool Texture::loadFromFile(const std::string& path) {
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 0);
    if (data) {
        setupGLTexture(data, width, height, nrChannels);
        stbi_image_free(data);
        return true;
    }
    std::cout << "Texture: Failed to load " << path << ", generating procedural fallback." << std::endl;
    return false;
}

void Texture::createProcedural(TextureType type) {
    width = 256;
    height = 256;
    nrChannels = 4;
    std::vector<unsigned char> data(width * height * 4);

    if (type == TEX_WOOD_FLOOR) {
        // Procedural realistic polished wood floor planks texture
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 4;
                float plankY = (float)y / 16.0f;
                int plankId = (int)plankY;
                float distToEdge = std::fmod(plankY, 1.0f);
                float grain = std::sin(x * 0.15f + plankId * 3.7f) * 0.08f + std::sin(x * 0.05f) * 0.05f;

                // Base warm wood tone
                float r = 0.72f + grain;
                float g = 0.45f + grain * 0.7f;
                float b = 0.22f + grain * 0.4f;

                // Plank boundary lines
                if (distToEdge < 0.06f || (x % 64 == 0 && (x / 64 + plankId) % 2 == 0)) {
                    r *= 0.35f; g *= 0.35f; b *= 0.35f;
                }

                data[index + 0] = (unsigned char)(r * 255.0f);
                data[index + 1] = (unsigned char)(g * 255.0f);
                data[index + 2] = (unsigned char)(b * 255.0f);
                data[index + 3] = 255;
            }
        }
    }
    else if (type == TEX_BASKETBALL) {
        // Procedural orange basketball leather with black rubber seams
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 4;
                float u = (float)x / width;
                float v = (float)y / height;

                // Leather pebble grain noise
                float pebble = (std::sin(x * 0.8f) * std::cos(y * 0.8f)) * 0.05f;

                // Orange base
                float r = 0.88f + pebble;
                float g = 0.36f + pebble * 0.5f;
                float b = 0.08f;

                // Seams math (vertical line, horizontal equator, and curved side ribs)
                float seamDist = 1.0f;
                float dx = std::abs(u - 0.5f);
                float dy = std::abs(v - 0.5f);
                seamDist = std::min(seamDist, dx);
                seamDist = std::min(seamDist, dy);

                // Curved ribs
                float rib1 = std::abs(std::sin(u * 3.14159f) * 0.5f - (v - 0.5f));
                float rib2 = std::abs(std::sin((1.0f - u) * 3.14159f) * 0.5f - (v - 0.5f));
                seamDist = std::min(seamDist, rib1 * 0.5f);
                seamDist = std::min(seamDist, rib2 * 0.5f);

                if (seamDist < 0.025f) {
                    r = 0.12f; g = 0.12f; b = 0.12f; // Black seam line
                }

                data[index + 0] = (unsigned char)(r * 255.0f);
                data[index + 1] = (unsigned char)(g * 255.0f);
                data[index + 2] = (unsigned char)(b * 255.0f);
                data[index + 3] = 255;
            }
        }
    }
    else if (type == TEX_NET) {
        // White rope netting texture with transparency
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int index = (y * width + x) * 4;
                int gridX = x % 32;
                int gridY = y % 32;
                bool isString = (gridX < 3 || gridY < 3 || std::abs(gridX - gridY) < 3);

                data[index + 0] = 240;
                data[index + 1] = 240;
                data[index + 2] = 240;
                data[index + 3] = isString ? 220 : 0; // Alpha transparency
            }
        }
    }
    else { // TEX_SOLID_WHITE
        for (int i = 0; i < width * height * 4; i += 4) {
            data[i + 0] = 255;
            data[i + 1] = 255;
            data[i + 2] = 255;
            data[i + 3] = 255;
        }
    }

    setupGLTexture(data.data(), width, height, nrChannels);
}

void Texture::setupGLTexture(unsigned char* data, int w, int h, int channels) {
    cleanup();
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);

    GLenum format = (channels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind(unsigned int unit) const {
    if (ID == 0) return;
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}
