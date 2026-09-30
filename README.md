# 3D Basketball Shooting Simulator — C++ & Modern OpenGL

An interactive 3D Basketball Shooting Simulation built with **C++17 and Modern OpenGL 3.3 Core Profile**, featuring real-time projectile kinematics, Blinn-Phong lighting, procedural mesh generation, dynamic trajectory preview, and robust collision detection.

---

## 🎯 Project Objective

Designed as an academic Computer Graphics demonstration and Viva examination project. It demonstrates key graphics pipeline concepts (VAO/VBO/EBO, GLSL Shaders, Matrix Transformations, Depth Testing, Alpha Blending, and Custom Procedural Textures) along with basic 3D physics.

---

## ✨ Features

- **Full 3D Basketball Court**: Realistic wooden parquet floor, court boundary lines, key/paint area, center circle, and 3-point arcs.
- **Detailed 3D Hoop**: Metal post support, transparent glass backboard, metallic red rim torus, and netting.
- **Basketball Rendering**: 3D procedural UV sphere mesh with orange leather material and black seam lines.
- **Dynamic Trajectory Preview**: Pre-computes predicted parabolic arc points updated live as you alter power or shooting angle.
- **Interactive Controls**: WASD move, mouse look, arrow key aim adjustment, spacebar hold power charging meter (`[████████------] 72%`).
- **3D Physics & Spin**: Real-time 3D kinematic trajectory equations with flight backspin matrix calculation.
- **Collision & Basket Detection**: Handles ball-ground bounce, backboard reflection, rim contact impulse, and rim cylinder pass-through basket detection (+2 pts score, accuracy tracking).
- **In-Game HUD**: Text rendering engine using orthographic projection displaying Score, Attempts, Made/Missed, Accuracy %, Power gauge, and Angle.

---

## 🛠️ Technology Stack

- **Language**: C++17
- **Graphics API**: Modern OpenGL 3.3 Core Profile
- **Windowing & Input**: GLFW 3
- **OpenGL Extension Loader**: GLAD
- **Mathematics**: GLM (OpenGL Mathematics)
- **Texture Loader**: `stb_image.h`
- **Build System**: CMake (VERSION >= 3.16)

---

## ⚙️ Building on Windows

### Prerequisites
- C++ Compiler (MinGW GCC 6.3+ or Visual Studio 2019/2022 MSVC)
- CMake (3.16 or newer)

### Build Instructions

1. Open PowerShell / Command Prompt in project directory:
   ```bash
   cd c:\Users\Jaspreet\Documents\Msc\sem3\cg
   ```

2. Generate Build Files (using CMake with MinGW or MSVC):
   ```bash
   # MinGW:
   C:\tools\cmake\bin\cmake.exe -B build -G "MinGW Makefiles"

   # OR Visual Studio:
   C:\tools\cmake\bin\cmake.exe -B build
   ```

3. Compile the Application:
   ```bash
   C:\tools\cmake\bin\cmake.exe --build build --config Release
   ```

4. Launch Executable:
   ```bash
   .\build\BasketballSimulator.exe
   ```

---

## 🎮 Controls

| Key / Input | Action |
|---|---|
| **W / A / S / D** | Move Camera / Player Position |
| **Mouse Movement** | Look Around / Aim View |
| **UP / DOWN Arrows** | Increase / Decrease Shooting Elevation Angle (30° - 75°) |
| **LEFT / RIGHT Arrows** | Adjust Horizontal Aim Direction (-30° to +30°) |
| **SPACE (Hold & Release)** | Charge Shooting Power Meter & Launch Basketball |
| **R** | Reset Basketball Position |
| **C** | Reset Camera View |
| **T** | Toggle Dynamic Trajectory Preview Arc |
| **P** | Pause / Resume Simulation |
| **ESC** | Return to Main Menu / Exit |

---

## 🏛️ System Architecture

```text
cg/
├── CMakeLists.txt
├── README.md
│
├── shaders/
│   ├── lighting.vert      # World geometry vertex processing & normal matrices
│   ├── lighting.frag      # Blinn-Phong lighting & procedural texture sampler
│   ├── trajectory.vert    # Trajectory preview marker vertex transform
│   ├── trajectory.frag    # Glow color fragment shader
│   ├── ui.vert            # Orthographic 2D HUD vertex shader
│   └── ui.frag            # Bitmap font atlas text fragment shader
│
├── include/
│   ├── Camera.h           # 3D FPS camera (LookAt, Perspective)
│   ├── Shader.h           # GLSL Shader program manager
│   ├── Texture.h          # Procedural texture generator (Wood, Leather, Net)
│   ├── Mesh.h             # VAO/VBO/EBO Mesh abstraction & primitives
│   ├── Court.h            # 3D Wooden basketball court & line geometry
│   ├── Hoop.h             # 3D Hoop, glass backboard, torus rim, net
│   ├── Ball.h             # 3D Sphere basketball model & shadow projection
│   ├── Physics.h          # 3D Kinematics & trajectory equations
│   ├── Collision.h        # Ground bounce, rim/backboard bounce, score detection
│   ├── FontRenderer.h     # Procedural 8x16 ASCII font atlas renderer
│   ├── UI.h               # HUD UI screens, power meter, stats display
│   └── Game.h             # Main game state loop and event dispatcher
│
├── src/
│   ├── main.cpp           # Program entry point
│   └── ... (Implementations)
│
└── docs/
    ├── project_report.md  # Detailed academic Computer Graphics project report
    └── viva_questions.md  # 35+ Viva questions and concise answers
```

---

## 🧮 Computer Graphics & Mathematics Concepts

1. **3D Coordinate Systems & Transformations**:
   - Model Matrix: $M = T \cdot R \cdot S$
   - View Matrix: $V = \text{lookAt}(E, T, U)$
   - Projection Matrix: $P = \text{perspective}(\theta, \text{aspect}, z_{near}, z_{far})$
2. **Shaders & Vertex Processing**:
   - Normal Transformation via transpose inverse model matrix: $N_{world} = (M^{-1})^T \cdot N_{local}$.
3. **Blinn-Phong Lighting Model**:
   - $I = I_a k_a + I_d k_d (\mathbf{N} \cdot \mathbf{L}) + I_s k_s (\mathbf{N} \cdot \mathbf{H})^\gamma$
4. **Alpha Blending & Glass Depth Sorting**:
   - Semi-transparent glass backboard rendered with `glEnable(GL_BLEND)` and depth mask control `glDepthMask(GL_FALSE)` to eliminate Z-sorting artifacts.
5. **Projectile Kinematics**:
   - $x(t) = x_0 + v_x t$
   - $y(t) = y_0 + v_y t - \frac{1}{2} g t^2$
   - $z(t) = z_0 + v_z t$
