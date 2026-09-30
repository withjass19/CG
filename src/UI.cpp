#include "UI.h"
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>
#include <iomanip>
#include <iostream>

UI::UI()
    : score(0), attempts(0), madeShots(0), missedShots(0),
      power(50.0f), elevationAngle(48.0f), azimuthAngle(0.0f),
      showTrajectory(true), state(STATE_MENU), bannerTimer(0.0f) {}

UI::~UI() {}

void UI::init(const std::string& vShaderCode, const std::string& fShaderCode) {
    uiShader = Shader(vShaderCode, fShaderCode, true);
    fontRenderer.init();
}

void UI::update(float deltaTime) {
    if (bannerTimer > 0.0f) {
        bannerTimer -= deltaTime;
    }
}

float UI::getAccuracy() const {
    if (attempts <= 0) return 0.0f;
    return ((float)madeShots / (float)attempts) * 100.0f;
}

void UI::renderHUD(int windowWidth, int windowHeight) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight);
    uiShader.use();
    uiShader.setMat4("projection", projection);

    if (state == STATE_MENU) {
        // Overlay menu backdrop
        fontRenderer.renderQuad(uiShader, 0, 0, (float)windowWidth, (float)windowHeight, glm::vec4(0.05f, 0.08f, 0.12f, 0.92f));

        float cx = windowWidth * 0.5f;
        fontRenderer.renderText(uiShader, "3D BASKETBALL SHOOTING SIMULATOR", cx - 280.0f, windowHeight * 0.75f, 2.2f, glm::vec4(1.0f, 0.6f, 0.1f, 1.0f));
        fontRenderer.renderText(uiShader, "Computer Graphics Academic Project", cx - 180.0f, windowHeight * 0.68f, 1.3f, glm::vec4(0.8f, 0.85f, 0.9f, 1.0f));

        fontRenderer.renderQuad(uiShader, cx - 180.0f, windowHeight * 0.45f, 360.0f, 120.0f, glm::vec4(0.12f, 0.18f, 0.25f, 0.85f));
        fontRenderer.renderText(uiShader, "[ SPACE ] Start Gameplay", cx - 140.0f, windowHeight * 0.51f, 1.4f, glm::vec4(0.2f, 0.9f, 0.4f, 1.0f));
        fontRenderer.renderText(uiShader, "[ ESC ] Exit Application", cx - 140.0f, windowHeight * 0.47f, 1.4f, glm::vec4(0.9f, 0.3f, 0.3f, 1.0f));

        fontRenderer.renderText(uiShader, "Controls Preview:", cx - 220.0f, windowHeight * 0.30f, 1.3f, glm::vec4(0.9f, 0.9f, 0.5f, 1.0f));
        fontRenderer.renderText(uiShader, "WASD: Move Camera | Mouse: Look Aim", cx - 220.0f, windowHeight * 0.25f, 1.1f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        fontRenderer.renderText(uiShader, "UP/DOWN: Adjust Angle | LEFT/RIGHT: Aim Dir", cx - 220.0f, windowHeight * 0.21f, 1.1f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));
        fontRenderer.renderText(uiShader, "Hold SPACE: Charge Power & Shoot", cx - 220.0f, windowHeight * 0.17f, 1.1f, glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

        glEnable(GL_DEPTH_TEST);
        return;
    }

    // Top Stats Bar Container
    fontRenderer.renderQuad(uiShader, 20.0f, windowHeight - 90.0f, 440.0f, 70.0f, glm::vec4(0.08f, 0.12f, 0.16f, 0.75f));

    std::stringstream ssScore, ssAttempts, ssAccuracy;
    ssScore << "SCORE: " << score;
    ssAttempts << "SHOTS: " << madeShots << "/" << attempts;
    ssAccuracy << "ACCURACY: " << std::fixed << std::setprecision(1) << getAccuracy() << "%";

    fontRenderer.renderText(uiShader, ssScore.str(), 35.0f, windowHeight - 45.0f, 1.5f, glm::vec4(1.0f, 0.75f, 0.2f, 1.0f));
    fontRenderer.renderText(uiShader, ssAttempts.str(), 200.0f, windowHeight - 45.0f, 1.3f, glm::vec4(0.3f, 0.9f, 0.9f, 1.0f));
    fontRenderer.renderText(uiShader, ssAccuracy.str(), 35.0f, windowHeight - 75.0f, 1.2f, glm::vec4(0.8f, 0.95f, 0.8f, 1.0f));

    // Bottom Power & Angle Gauges Container
    fontRenderer.renderQuad(uiShader, 20.0f, 20.0f, 480.0f, 100.0f, glm::vec4(0.08f, 0.12f, 0.16f, 0.8f));

    // Power Bar Gauge
    std::stringstream ssPower;
    ssPower << "POWER: " << (int)power << "%";
    fontRenderer.renderText(uiShader, ssPower.str(), 35.0f, 90.0f, 1.3f, glm::vec4(0.95f, 0.95f, 0.95f, 1.0f));

    // Power Bar Background & Filled Level
    fontRenderer.renderQuad(uiShader, 170.0f, 90.0f, 220.0f, 16.0f, glm::vec4(0.2f, 0.2f, 0.2f, 0.9f));
    float powerFillW = (power / 100.0f) * 220.0f;
    glm::vec4 powerColor = (power > 80.0f) ? glm::vec4(0.9f, 0.2f, 0.2f, 1.0f) :
                          (power > 50.0f) ? glm::vec4(0.9f, 0.8f, 0.2f, 1.0f) :
                                            glm::vec4(0.2f, 0.85f, 0.3f, 1.0f);
    fontRenderer.renderQuad(uiShader, 170.0f, 90.0f, powerFillW, 16.0f, powerColor);

    // Angle & Aim Display
    std::stringstream ssAngle;
    ssAngle << "ELEVATION ANGLE: " << (int)elevationAngle << " Deg  |  AIM: " << (int)azimuthAngle << " Deg";
    fontRenderer.renderText(uiShader, ssAngle.str(), 35.0f, 60.0f, 1.15f, glm::vec4(0.7f, 0.85f, 1.0f, 1.0f));

    // Trajectory Status
    std::string trajText = showTrajectory ? "TRAJECTORY PREVIEW: ON [T]" : "TRAJECTORY PREVIEW: OFF [T]";
    fontRenderer.renderText(uiShader, trajText, 35.0f, 35.0f, 1.0f, showTrajectory ? glm::vec4(0.4f, 0.9f, 0.4f, 1.0f) : glm::vec4(0.6f, 0.6f, 0.6f, 1.0f));

    // Shot Result Banner
    if (bannerTimer > 0.0f && !resultBannerMessage.empty()) {
        glm::vec4 bannerColor = (resultBannerMessage.find("SWISH") != std::string::npos || resultBannerMessage.find("SCORED") != std::string::npos) ?
                                glm::vec4(0.2f, 0.95f, 0.3f, 1.0f) : glm::vec4(0.95f, 0.3f, 0.2f, 1.0f);
        fontRenderer.renderQuad(uiShader, windowWidth * 0.5f - 180.0f, windowHeight * 0.65f, 360.0f, 60.0f, glm::vec4(0.1f, 0.1f, 0.15f, 0.85f));
        fontRenderer.renderText(uiShader, resultBannerMessage, windowWidth * 0.5f - 140.0f, windowHeight * 0.67f, 1.8f, bannerColor);
    }

    // Pause Screen Overlay
    if (state == STATE_PAUSED) {
        fontRenderer.renderQuad(uiShader, 0, 0, (float)windowWidth, (float)windowHeight, glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));
        fontRenderer.renderText(uiShader, "PAUSED", windowWidth * 0.5f - 60.0f, windowHeight * 0.55f, 2.5f, glm::vec4(1.0f, 0.9f, 0.2f, 1.0f));
        fontRenderer.renderText(uiShader, "Press [ P ] to Resume", windowWidth * 0.5f - 110.0f, windowHeight * 0.47f, 1.3f, glm::vec4(0.9f, 0.9f, 0.9f, 1.0f));
    }

    // Controls Legend Footer
    fontRenderer.renderText(uiShader, "WASD: Move | Mouse: Look | UP/DN: Angle | LEFT/RIGHT: Aim | SPACE: Hold Charge & Release Shoot | R: Reset Ball | C: Reset Cam | P: Pause",
                            20.0f, windowHeight - 20.0f, 0.95f, glm::vec4(0.85f, 0.85f, 0.85f, 0.9f));

    glEnable(GL_DEPTH_TEST);
}
