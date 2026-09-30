#ifndef UI_H
#define UI_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include "FontRenderer.h"
#include "Shader.h"

enum GameState {
    STATE_MENU,
    STATE_PLAYING,
    STATE_SHOT_IN_PROGRESS,
    STATE_SHOT_RESULT,
    STATE_PAUSED
};

class UI {
public:
    FontRenderer fontRenderer;
    Shader uiShader;

    int score;
    int attempts;
    int madeShots;
    int missedShots;
    float power;            // 0% - 100%
    float elevationAngle;   // 30 - 75 degrees
    float azimuthAngle;     // -30 to +30 degrees
    bool showTrajectory;
    GameState state;
    std::string resultBannerMessage;
    float bannerTimer;

    UI();
    ~UI();

    void init(const std::string& vShaderCode, const std::string& fShaderCode);
    void update(float deltaTime);
    void renderHUD(int windowWidth, int windowHeight);
    float getAccuracy() const;
};

#endif
