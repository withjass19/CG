#ifndef GAME_H
#define GAME_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Camera.h"
#include "Shader.h"
#include "Court.h"
#include "Hoop.h"
#include "Ball.h"
#include "UI.h"
#include "Mesh.h"

class Game {
public:
    int windowWidth;
    int windowHeight;
    GLFWwindow* window;

    Camera camera;
    Court court;
    Hoop hoop;
    Ball ball;
    UI ui;

    Shader lightingShader;
    Shader trajectoryShader;
    Mesh trajectoryDotMesh;

    bool isSpacePressed;
    float powerChargeSpeed; // % per second
    float deltaTime;
    float lastFrame;

    float lastMouseX;
    float lastMouseY;
    bool firstMouse;

    Game(int width = 1280, int height = 720);
    ~Game();

    bool init();
    void run();
    void processInput(float dt);
    void update(float dt);
    void render();
    void shootBall();
    void resetBallPosition();

    void handleMouseMovement(double xpos, double ypos);
    void handleKeyInput(int key, int scancode, int action, int mods);
};

#endif
