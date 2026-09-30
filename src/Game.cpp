#include "Game.h"
#include "Physics.h"
#include "Collision.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>

static Game* g_GameInstance = nullptr;

static void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (g_GameInstance) {
        g_GameInstance->windowWidth = width;
        g_GameInstance->windowHeight = height;
    }
}

static void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (g_GameInstance) {
        g_GameInstance->handleMouseMovement(xpos, ypos);
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (g_GameInstance) {
        g_GameInstance->handleKeyInput(key, scancode, action, mods);
    }
}

Game::Game(int width, int height)
    : windowWidth(width), windowHeight(height), window(nullptr),
      camera(glm::vec3(0.0f, 2.2f, 4.0f), glm::vec3(0.0f, 1.0f, 0.0f), -90.0f, -10.0f),
      hoop(glm::vec3(0.0f, 0.0f, -12.425f)), ball(0.24f),
      isSpacePressed(false), powerChargeSpeed(85.0f),
      deltaTime(0.0f), lastFrame(0.0f),
      lastMouseX(width / 2.0f), lastMouseY(height / 2.0f), firstMouse(true) {
    g_GameInstance = this;
}

Game::~Game() {
    if (window) {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
}

static std::string readFileString(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return "";
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

bool Game::init() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(windowWidth, windowHeight, "3D Basketball Shooting Simulator", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetKeyCallback(window, key_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Initialize Shaders
    lightingShader = Shader("shaders/lighting.vert", "shaders/lighting.frag");
    trajectoryShader = Shader("shaders/trajectory.vert", "shaders/trajectory.frag");

    // Initialize UI
    std::string uiVert = readFileString("shaders/ui.vert");
    std::string uiFrag = readFileString("shaders/ui.frag");
    ui.init(uiVert, uiFrag);

    // Initialize 3D Scene Components
    court.init();
    hoop.init();
    ball.init();

    // Reset ball in front of player/shooting position
    resetBallPosition();

    // Trajectory Dot Mesh
    trajectoryDotMesh = Mesh::createSphere(0.045f, 12, 12);

    return true;
}

void Game::resetBallPosition() {
    glm::vec3 shootPos = camera.Position + camera.Front * 1.0f - glm::vec3(0.0f, 0.35f, 0.0f);
    ball.reset(shootPos);
}

void Game::shootBall() {
    if (ball.isShot) return;

    glm::vec3 aimDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
    if (glm::length(aimDir) < 0.001f) aimDir = glm::vec3(0.0f, 0.0f, -1.0f);

    glm::vec3 initVel = Physics::calculateInitialVelocity(ui.power, ui.elevationAngle, ui.azimuthAngle, aimDir);
    ball.velocity = initVel;
    ball.isShot = true;
    ui.attempts++;
    ui.state = STATE_SHOT_IN_PROGRESS;
}

void Game::handleMouseMovement(double xpos, double ypos) {
    if (firstMouse) {
        lastMouseX = (float)xpos;
        lastMouseY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastMouseX;
    float yoffset = lastMouseY - (float)ypos; // Reversed since Y-coordinates go from bottom to top

    lastMouseX = (float)xpos;
    lastMouseY = (float)ypos;

    if (ui.state == STATE_PLAYING || ui.state == STATE_SHOT_IN_PROGRESS || ui.state == STATE_SHOT_RESULT) {
        camera.ProcessMouseMovement(xoffset, yoffset);
    }
}

void Game::handleKeyInput(int key, int scancode, int action, int mods) {
    (void)scancode; (void)mods;

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            if (ui.state == STATE_MENU) {
                glfwSetWindowShouldClose(window, true);
            } else {
                ui.state = STATE_MENU;
            }
        }
        if (key == GLFW_KEY_P) {
            if (ui.state == STATE_PLAYING || ui.state == STATE_SHOT_IN_PROGRESS) {
                ui.state = STATE_PAUSED;
            } else if (ui.state == STATE_PAUSED) {
                ui.state = STATE_PLAYING;
            }
        }
        if (key == GLFW_KEY_R) {
            resetBallPosition();
            if (ui.state != STATE_MENU) ui.state = STATE_PLAYING;
        }
        if (key == GLFW_KEY_C) {
            camera.Reset();
        }
        if (key == GLFW_KEY_T) {
            ui.showTrajectory = !ui.showTrajectory;
        }
        if (key == GLFW_KEY_SPACE) {
            if (ui.state == STATE_MENU) {
                ui.state = STATE_PLAYING;
            } else if (!ball.isShot && (ui.state == STATE_PLAYING || ui.state == STATE_SHOT_RESULT)) {
                isSpacePressed = true;
            }
        }
    }

    if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_SPACE && isSpacePressed) {
            isSpacePressed = false;
            shootBall();
        }
    }
}

void Game::processInput(float dt) {
    if (ui.state == STATE_PAUSED || ui.state == STATE_MENU) return;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, dt);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, dt);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, dt);

    // Adjust Shooting Elevation Angle
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        ui.elevationAngle += 25.0f * dt;
        if (ui.elevationAngle > 78.0f) ui.elevationAngle = 78.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        ui.elevationAngle -= 25.0f * dt;
        if (ui.elevationAngle < 25.0f) ui.elevationAngle = 25.0f;
    }

    // Adjust Shooting Azimuth Aim
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        ui.azimuthAngle += 25.0f * dt;
        if (ui.azimuthAngle > 35.0f) ui.azimuthAngle = 35.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        ui.azimuthAngle -= 25.0f * dt;
        if (ui.azimuthAngle < -35.0f) ui.azimuthAngle = -35.0f;
    }
}

void Game::update(float dt) {
    ui.update(dt);

    if (ui.state == STATE_PAUSED || ui.state == STATE_MENU) return;

    // Charge power while holding SPACE
    if (isSpacePressed && !ball.isShot) {
        ui.power += powerChargeSpeed * dt;
        if (ui.power > 100.0f) {
            ui.power = 0.0f; // Reset loop gauge
        }
    }

    // Keep ball in front of player if not shot
    if (!ball.isShot) {
        glm::vec3 shootPos = camera.Position + camera.Front * 1.0f - glm::vec3(0.0f, 0.35f, 0.0f);
        ball.position = shootPos;
    } else {
        glm::vec3 prevPos = ball.position;
        ball.update(dt);

        // Process Collisions & Scoring
        CollisionResult col = CollisionEngine::processAll(ball, hoop, court, prevPos);
        if (col == COL_BASKET_SCORED) {
            ui.score += 2;
            ui.madeShots++;
            ui.resultBannerMessage = "SWISH! BASKET SCORED (+2 PTS)";
            ui.bannerTimer = 3.0f;
            ui.state = STATE_SHOT_RESULT;
        } else if (col == COL_RIM || col == COL_BACKBOARD) {
            if (!ball.isScored && ui.resultBannerMessage.empty()) {
                ui.resultBannerMessage = (col == COL_RIM) ? "RIM BOUNCE!" : "BACKBOARD TOUCH!";
                ui.bannerTimer = 1.5f;
            }
        } else if (col == COL_GROUND && !ball.isScored) {
            if (ui.resultBannerMessage.empty()) {
                ui.missedShots++;
                ui.resultBannerMessage = "SHOT MISSED";
                ui.bannerTimer = 2.0f;
                ui.state = STATE_SHOT_RESULT;
            }
        }
    }
}

void Game::render() {
    glClearColor(0.1f, 0.14f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspectRatio = (float)windowWidth / (float)windowHeight;
    glm::mat4 projection = camera.GetProjectionMatrix(aspectRatio);
    glm::mat4 view = camera.GetViewMatrix();

    // 1. Scene Lighting Shader Pass
    lightingShader.use();
    lightingShader.setMat4("projection", projection);
    lightingShader.setMat4("view", view);
    lightingShader.setVec3("viewPos", camera.Position);

    // Opaque Scene Geometry
    court.draw(lightingShader);
    hoop.draw(lightingShader, true); // Opaque parts of hoop
    ball.draw(lightingShader);
    ball.drawShadow(lightingShader);

    // 2. Trajectory Preview Dots Pass
    if (ui.showTrajectory && !ball.isShot && ui.state != STATE_MENU) {
        trajectoryShader.use();
        trajectoryShader.setMat4("projection", projection);
        trajectoryShader.setMat4("view", view);

        glm::vec3 aimDir = glm::vec3(camera.Front.x, 0.0f, camera.Front.z);
        if (glm::length(aimDir) < 0.001f) aimDir = glm::vec3(0.0f, 0.0f, -1.0f);
        glm::vec3 initVel = Physics::calculateInitialVelocity(ui.power, ui.elevationAngle, ui.azimuthAngle, aimDir);
        std::vector<glm::vec3> trajPoints = Physics::generateTrajectory(ball.position, initVel, 0.05f, 35);

        for (size_t i = 0; i < trajPoints.size(); ++i) {
            glm::mat4 dotModel = glm::mat4(1.0f);
            dotModel = glm::translate(dotModel, trajPoints[i]);
            float dotScale = 1.0f - ((float)i / (float)trajPoints.size()) * 0.5f;
            dotModel = glm::scale(dotModel, glm::vec3(dotScale));

            trajectoryShader.setMat4("model", dotModel);
            trajectoryShader.setVec4("dotColor", glm::vec4(1.0f, 0.85f, 0.2f, 0.9f));
            trajectoryDotMesh.draw();
        }
    }

    // 3. Transparent Pass (Glass backboard & Net)
    lightingShader.use();
    hoop.drawTransparent(lightingShader);

    // 4. UI HUD Pass
    ui.renderHUD(windowWidth, windowHeight);

    glfwSwapBuffers(window);
}

void Game::run() {
    while (!glfwWindowShouldClose(window)) {
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        if (deltaTime > 0.1f) deltaTime = 0.1f; // Cap delta time spike

        glfwPollEvents();
        processInput(deltaTime);
        update(deltaTime);
        render();
    }
}
