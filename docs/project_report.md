# Academic Project Report: 3D Basketball Shooting Simulator

**Course**: Computer Graphics & 3D Simulation  
**Implementation Language**: C++17  
**Graphics API**: Modern OpenGL (Core Profile 3.3)  
**Build System**: CMake  

---

## 1. Introduction

The 3D Basketball Shooting Simulator is a computer graphics application developed to demonstrate fundamental principles of 3D real-time rendering, transformation pipeline mathematics, shader programming, projectile physics, and collision detection. Built natively using C++ and Modern OpenGL, the application avoids high-level game engine abstractions (such as Unity or Unreal) to provide direct control over vertex buffers, shaders, and rendering pipelines.

---

## 2. Problem Statement

Commercial game engines abstract hardware interaction, matrix transformations, and physics pipelines behind high-level APIs. In academic computer graphics, understanding the underlying pipeline—including coordinate transformations, vertex processing, fragment shading, material lighting, and custom collision algorithms—is essential. This project addresses the challenge of building an interactive 3D simulation completely from scratch using Modern OpenGL 3.3 Core Profile.

---

## 3. Objectives

- Implement a modular C++ 3D rendering architecture.
- Utilize Modern OpenGL concepts: Vertex Array Objects (VAO), Vertex Buffer Objects (VBO), Element Buffer Objects (EBO), and custom GLSL vertex/fragment shaders.
- Construct a realistic 3D basketball court environment, complete with court lines, transparent glass backboard, metallic red rim torus, and procedural textures.
- Formulate real-time 3D projectile kinematics equations for basketball flight and spin rotation.
- Develop realistic collision detection algorithms for ground reflection, backboard rebound, rim contact, and downward hoop basket scoring.
- Design an orthographic HUD UI displaying scores, shooting power meters, angle controls, and accuracy statistics.

---

## 4. Technology Used

- **Programming Language**: C++17
- **Graphics API**: OpenGL 3.3 Core Profile
- **Windowing & Input Library**: GLFW 3
- **OpenGL Extension Loader**: GLAD
- **Mathematics Library**: GLM (OpenGL Mathematics)
- **Image Loader**: `stb_image.h`
- **Build System**: CMake 3.16+

---

## 5. System Requirements

- **Operating System**: Windows 10/11 (or Linux/macOS)
- **Compiler**: C++ Compiler supporting C++17 (MinGW GCC 6.3+ or MSVC 2019+)
- **GPU**: Graphics hardware supporting OpenGL 3.3 Core Profile
- **RAM**: Minimum 2 GB RAM

---

## 6. System Architecture

The application is structured cleanly into decoupled components:

```text
+-------------------------------------------------------------+
|                            Game                             |
|    (Main loop, state management, event handling, HUD)       |
+------+-----------+-----------+------------+-----------------+
       |           |           |            |
       v           v           v            v
  +---------+ +---------+ +---------+ +-----------+
  | Camera  | |  Court  | |  Hoop   | |   Ball    |
  +---------+ +---------+ +---------+ +-----+-----+
                                            |
                                            v
                                     +--------------+
                                     |  Physics &   |
                                     | CollisionEng |
                                     +--------------+
```

---

## 7. Computer Graphics Concepts

1. **Vertex Processing & VAO/VBO/EBO**:
   Vertex buffers store 3D vertex positions, normals, and texture coordinates. Vertex Array Objects specify attribute layouts.
2. **Transformations (MVP Pipeline)**:
   - Model Matrix ($M$): Transforms local object coordinates into world coordinates.
   - View Matrix ($V$): Transforms world coordinates into camera view space using `glm::lookAt`.
   - Projection Matrix ($P$): Maps view space to homogeneous clip space using `glm::perspective`.
3. **GLSL Shading**:
   Shaders written in OpenGL Shading Language (GLSL 330 core) execute per-vertex and per-fragment.
4. **Blinn-Phong Lighting Model**:
   Calculates ambient, diffuse, and specular illumination using halfway vector evaluation:
   \[ \mathbf{H} = \frac{\mathbf{L} + \mathbf{V}}{\|\mathbf{L} + \mathbf{V}\|} \]
5. **Alpha Blending & Translucency**:
   Enables rendering semi-transparent objects (such as the glass backboard and hoop net) with depth buffer sorting.

---

## 8. Mathematical Model

### Transformation Matrix Equation
For any point $\mathbf{P}_{local}$ on a 3D mesh, its clip space position $\mathbf{P}_{clip}$ is given by:
\[ \mathbf{P}_{clip} = \mathbf{P}_{proj} \cdot \mathbf{V}_{view} \cdot \mathbf{M}_{model} \cdot \mathbf{P}_{local} \]

### Normal Matrix Transformation
Normals are transformed using the transpose inverse of the upper $3 \times 3$ model matrix to prevent distortion under non-uniform scaling:
\[ \mathbf{N}_{world} = \text{normalize}\left( (\mathbf{M}_{model}^{-1})^T \cdot \mathbf{N}_{local} \right) \]

---

## 9. Physics Model

Basketball flight is governed by 3D parabolic kinematics equations:

\[ x(t) = x_0 + (v_{launch} \cdot \cos\theta \cdot \cos\phi) \cdot t \]
\[ y(t) = y_0 + (v_{launch} \cdot \sin\theta) \cdot t - \frac{1}{2} g t^2 \]
\[ z(t) = z_0 + (v_{launch} \cdot \cos\theta \cdot \sin\phi) \cdot t \]

Where:
- $v_{launch}$ is initial launch velocity proportional to charged power percentage.
- $\theta$ is elevation pitch angle.
- $\phi$ is azimuth aim direction angle.
- $g = 9.81 \text{ m/s}^2$ is gravitational acceleration.

---

## 10. Collision Detection

### A. Ball-Ground Collision
Detects when $y_{ball} - R_{ball} \le 0$. Reverses vertical velocity vector:
\[ v_y = -v_y \cdot e_g \]
Where $e_g = 0.72$ is the ground coefficient of restitution.

### B. Ball-Backboard Reflection
Detects intersection with backboard plane at $z = z_{bb}$. Reflects $v_z$:
\[ v_z = -v_z \cdot e_{bb} \]

### C. Ball-Rim Collision
Calculates 3D distance between ball center and the closest point on the torus ring. Computes normal impulse vector reflection:
\[ \mathbf{v}_{bounce} = \mathbf{v} - 2(\mathbf{v} \cdot \mathbf{N}) \mathbf{N} \]

### D. Basket Scoring Zone Detection
Invisible disk detector at rim height $y_{rim} = 3.05\text{m}$. Triggers a score increment when the ball center passes downward through the disk within the rim opening.

---

## 11. Implementation

Implementation details include:
- `Shader`: Compiles vertex and fragment GLSL shaders with compile log checking.
- `Mesh`: Generates procedural primitive meshes (Sphere, Cube, Cylinder, Torus, Quad, Ring).
- `Texture`: Generates procedural high-resolution wood grain textures, orange basketball leather textures, and net patterns.
- `FontRenderer`: Creates a 128x128 ASCII bitmap font atlas for crisp orthographic text rendering.

---

## 12. Testing

The application was tested across multiple test scenarios:
1. **Camera Controls Test**: Confirmed smooth WASD keyboard movement and mouse look rotation.
2. **Trajectory Accuracy Test**: Verified that predicted trajectory preview dots align with actual basketball flight path.
3. **Collision Response Test**: Tested backboard rebound, rim edge bounce, and ground damping.
4. **Scoring Test**: Validated that clean swishes and rim-bounce baskets increase score (+2 pts) and update accuracy percentage without NaN errors.

---

## 13. Results

The simulation runs smoothly at 60+ FPS on standard graphics hardware. Visual quality features realistic materials, lighting specular highlights, translucent backboard rendering, soft floor shadows, and clear HUD feedback.

---

## 14. Limitations

- Air drag and Magnus force (curve shots due to spin) are approximated simply.
- Net cloth physics uses a rigid/textured mesh rather than soft-body cloth simulation.

---

## 15. Future Scope

- Integration of soft-body net cloth deformation physics.
- Advanced shadow mapping (Cascaded Shadow Maps).
- Multiplayer shooting competition mode over local sockets.

---

## 16. Conclusion

The 3D Basketball Shooting Simulator successfully demonstrates the core principles of Computer Graphics using modern C++ and OpenGL. By directly managing shaders, geometry buffers, transformation matrices, and custom physics equations, the project serves as a complete academic reference suitable for demonstration and viva examination.
