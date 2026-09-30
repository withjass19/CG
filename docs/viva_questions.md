# Computer Graphics Viva Examination Questions & Answers
## Project: 3D Basketball Shooting Simulator (C++ & Modern OpenGL)

---

### Q1: What is the main difference between Immediate Mode OpenGL and Modern Programmable Pipeline OpenGL?
**Answer**: Immediate mode OpenGL (`glBegin`, `glEnd`, `glVertex3f`) executes graphics commands sequentially on the CPU per vertex, creating immense CPU-GPU communication bottlenecks. Modern OpenGL (Core Profile 3.3+) uses programmable shaders (GLSL) and stores vertex data directly in GPU VRAM memory via Vertex Buffer Objects (VBOs), executing parallel vertex and fragment processing on GPU hardware.

---

### Q2: Explain the roles of VBO, VAO, and EBO in Modern OpenGL.
**Answer**:
- **VBO (Vertex Buffer Object)**: Memory buffer stored in GPU VRAM containing raw vertex data (positions, normals, texture coordinates).
- **VAO (Vertex Array Object)**: State container object that stores attribute configuration and mappings for how VBO data should be interpreted by vertex shaders.
- **EBO (Element Buffer Object)**: Index buffer stored in GPU VRAM containing array indices that allow reusing vertices to render triangles efficiently without duplicate data.

---

### Q3: What is the Model-View-Projection (MVP) matrix pipeline?
**Answer**: The MVP pipeline converts 3D local mesh coordinates to 2D screen coordinates:
1. **Model Matrix ($M$)**: Transforms local mesh vertices into global 3D world space (Translation, Rotation, Scaling).
2. **View Matrix ($V$)**: Transforms world space coordinates relative to camera orientation (`glm::lookAt`).
3. **Projection Matrix ($P$)**: Maps camera view coordinates into clip space using perspective projection (`glm::perspective`).

---

### Q4: How is Perspective Projection mathematically calculated?
**Answer**: Perspective projection applies a frustum transformation where objects further from the camera appear smaller by scaling $x$ and $y$ coordinates inversely proportional to depth $z$:
\[ x_{clip} = \frac{x_{view}}{\tan(\text{fov}/2) \cdot \text{aspect}}, \quad y_{clip} = \frac{y_{view}}{\tan(\text{fov}/2)}, \quad z_{clip} = f(z_{view}) \]

---

### Q5: What is GLSL and what are its two primary shader types?
**Answer**: GLSL (OpenGL Shading Language) is a C-like high-level language compiled on GPU hardware.
- **Vertex Shader**: Processes per-vertex inputs, transforms positions into clip space (`gl_Position`), and passes interpolated outputs to fragment shaders.
- **Fragment Shader**: Calculates output pixel color (`FragColor`) for rasterized fragments, computing lighting, material properties, and texture sampling.

---

### Q6: Explain the Blinn-Phong Lighting Model used in this simulator.
**Answer**: Blinn-Phong computes total surface color by adding three components:
1. **Ambient**: Constant base background illumination ($I_a \cdot k_a$).
2. **Diffuse**: Directional surface illumination based on Lambert's cosine law ($I_d \cdot k_d \cdot (\mathbf{N} \cdot \mathbf{L})$).
3. **Specular**: Shininess highlight computed using the halfway vector $\mathbf{H} = \frac{\mathbf{L} + \mathbf{V}}{\|\mathbf{L} + \mathbf{V}\|}$:
   \[ I_s \cdot k_s \cdot (\mathbf{N} \cdot \mathbf{H})^\gamma \]

---

### Q7: Why is the Normal Matrix computed as the Transpose Inverse of the Model Matrix?
**Answer**: If a model matrix contains non-uniform scaling (e.g. scaling $X$ by 2.0 and $Y$ by 1.0), multiplying normal vectors directly by the model matrix distorts their perpendicular angle relative to the surface. Multiplying by $(M^{-1})^T$ preserves orthogonality.

---

### Q8: What physics equations govern the 3D basketball trajectory?
**Answer**: Basic 3D kinematic equations under constant gravitational acceleration $g$:
\[ x(t) = x_0 + v_x t \]
\[ y(t) = y_0 + v_y t - \frac{1}{2} g t^2 \]
\[ z(t) = z_0 + v_z t \]

---

### Q9: How is initial launch velocity vector computed from power, elevation, and azimuth angles?
**Answer**:
- Launch speed $v_{launch}$ is mapped from power percentage.
- $v_x = v_{launch} \cdot \cos(\theta) \cdot \sin(\phi)$
- $v_y = v_{launch} \cdot \sin(\theta)$
- $v_z = -v_{launch} \cdot \cos(\theta) \cdot \cos(\phi)$
Where $\theta$ is elevation angle and $\phi$ is horizontal azimuth angle.

---

### Q10: How does the trajectory preview arc work in real time?
**Answer**: The system pre-calculates an array of sample 3D points $(x_i, y_i, z_i)$ at discrete time steps $t_i = i \cdot \Delta t$ using the kinematic equations before shooting, rendering a sequence of small glowing sphere markers along the path.

---

### Q11: How is dynamic basketball backspin rotation calculated?
**Answer**: As the ball moves with velocity $\mathbf{v}$, spin axis is calculated using cross product $\mathbf{a} = \mathbf{v} \times \mathbf{u}_{up}$. The rotation matrix is updated each frame by rotating around $\mathbf{a}$ by angle $\Delta \alpha = \frac{\|\mathbf{v}\|}{R_{ball}} \Delta t$.

---

### Q12: How is Ball-Ground collision detected and resolved?
**Answer**: When $y_{ball} - R_{ball} \le 0$, vertical velocity is reversed and damped using coefficient of restitution $e = 0.72$:
\[ v_y = -v_y \cdot 0.72 \]
Horizontal components $v_x, v_z$ are reduced by friction factor $0.85$.

---

### Q13: How is Ball-Backboard collision detected?
**Answer**: The backboard is modeled as a 3D plane/AABB box at $z = z_{bb}$. When ball position satisfies $|x_{ball} - x_{bb}| \le w/2$ and $|y_{ball} - y_{bb}| \le h/2$ and $|z_{ball} - z_{bb}| \le R_{ball}$, velocity $v_z$ is reflected with restitution.

---

### Q14: How is Ball-Rim collision detected?
**Answer**: The rim is a torus ring of radius $R_{rim}$. The system calculates the distance from the ball center to the closest point on the 3D rim circle. If this distance is less than $R_{ball} + R_{tube}$, the velocity vector is reflected across the contact normal:
\[ \mathbf{v}' = \text{reflect}(\mathbf{v}, \mathbf{N}) \cdot 0.65 \]

---

### Q15: How does the hoop scoring detector differentiate between a scored basket and a miss?
**Answer**: An invisible scoring plane is defined at rim height $y_{rim} = 3.05\text{m}$. The engine detects when the ball's previous position was above the rim ($y_{prev} \ge 3.05\text{m}$) and current position is below ($y_{curr} < 3.05\text{m}$) while its horizontal distance to the rim center is within the inner rim opening radius.

---

### Q16: What is GLFW and why is it used?
**Answer**: GLFW (Graphics Library Framework) is a lightweight C library providing cross-platform window creation, OpenGL context initialization, timer queries, and keyboard/mouse input callback handling.

---

### Q17: What is GLAD?
**Answer**: GLAD is an OpenGL Extension Loading Library that dynamically queries driver function pointers at runtime (`gladLoadGLLoader`), making modern OpenGL function calls (`glGenVertexArrays`, `glCompileShader`, `glUseProgram`) accessible in C++.

---

### Q18: What is GLM?
**Answer**: GLM (OpenGL Mathematics) is a header-only C++ mathematics library structured according to GLSL specifications. It provides vector types (`vec2`, `vec3`, `vec4`) and matrix transformations (`translate`, `rotate`, `scale`, `lookAt`, `perspective`).

---

### Q19: What is Depth Testing (`GL_DEPTH_TEST`)?
**Answer**: Depth testing uses a depth buffer (Z-buffer) to store the depth value of every rasterized pixel. OpenGL tests new fragment depth against existing values to ensure closer geometry obscures geometry behind it.

---

### Q20: How is alpha blending used for the transparent glass backboard?
**Answer**: Alpha blending calculates output color by blending source fragment alpha:
\[ C_{result} = C_{src} \cdot \alpha_{src} + C_{dest} \cdot (1 - \alpha_{src}) \]
Enabled using `glEnable(GL_BLEND)` and `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` with depth mask disabled (`glDepthMask(GL_FALSE)`) during the glass pass.

---

### Q21: What is texturing and how is `stb_image.h` used?
**Answer**: Texturing maps a 2D image onto 3D geometry surfaces using UV coordinates $(u, v) \in [0, 1]^2$. `stb_image.h` loads image files into memory arrays passed to `glTexImage2D`.

---

### Q22: What happens if texture files fail to load?
**Answer**: The custom `Texture` engine generates procedural fallback textures in memory (e.g. wood grain planks, orange leather with seam lines) so the application runs without crashing.

---

### Q23: How is text rendered in the OpenGL HUD?
**Answer**: The built-in `FontRenderer` creates a 128x128 bitmap font atlas in memory for ASCII characters 32-126. Screen text is rendered by drawing orthographic textured quads using an orthographic projection matrix (`glm::ortho`).

---

### Q24: How does the power meter charging work?
**Answer**: When SPACE is held, power variable increments linearly over time:
\[ P = (P + \text{speed} \cdot \Delta t) \bmod 100 \]
When released, launch velocity is initialized using the charged power percentage.

---

### Q25: How is Accuracy percentage calculated safely?
**Answer**: Accuracy is calculated as:
\[ \text{Accuracy} = \frac{\text{Made Shots}}{\text{Attempts}} \times 100 \]
If $\text{Attempts} == 0$, the function returns $0.0\%$ to avoid division by zero (NaN/Infinity).

---

### Q26: What are Uniforms in GLSL?
**Answer**: Uniforms are global GLSL variables passed from C++ CPU code to GPU shaders that stay constant for all vertices and fragments during a single draw call (e.g. Model/View/Projection matrices, material properties).

---

### Q27: What is rasterization in the OpenGL pipeline?
**Answer**: Rasterization converts vector primitives (triangles) output by the vertex shader into discrete 2D fragments (pixels) on the screen framebuffer.

---

### Q28: How is the projected ball shadow implemented?
**Answer**: A projected circular ring quad is rendered at floor level $y = 0.002\text{m}$ directly beneath the ball's $(x, z)$ position, scaling its radius and opacity based on height $y_{ball}$.

---

### Q29: What is FPS Delta Time ($\Delta t$) and why is it essential?
**Answer**: Delta time is the elapsed time between consecutive frames. Multiplying movement and velocity by $\Delta t$ ensures physics and camera motion remain consistent regardless of GPU frame rate (30 FPS vs 144 FPS).

---

### Q30: What game states are implemented in the state machine?
**Answer**:
1. `STATE_MENU`: Main title menu & controls.
2. `STATE_PLAYING`: Active shooting mode & aiming.
3. `STATE_SHOT_IN_PROGRESS`: Ball flying trajectory.
4. `STATE_SHOT_RESULT`: Displays shot outcome banner.
5. `STATE_PAUSED`: Pauses simulation execution.

---

### Q31: How is the 3D sphere geometry created procedurally?
**Answer**: A UV sphere is generated using latitude ($\theta \in [-\pi/2, \pi/2]$) and longitude ($\phi \in [0, 2\pi]$) angles, evaluating:
\[ x = R \cdot \cos\theta \cdot \cos\phi, \quad y = R \cdot \sin\theta, \quad z = R \cdot \cos\theta \cdot \sin\phi \]

---

### Q32: What is Z-fighting and how is it prevented in the court lines?
**Answer**: Z-fighting occurs when two coplanar surfaces share identical depth values, causing visual flickering. It is prevented by offsetting court lines by a small vertical epsilon ($y = +0.005\text{m}$) above the court floor.

---

### Q33: How does mouse look aiming work in the Camera class?
**Answer**: Mouse offsets $\Delta x, \Delta y$ update camera `Yaw` and `Pitch` angles. The front direction vector is computed as:
\[ \mathbf{F} = (\cos(\text{Yaw})\cos(\text{Pitch}), \sin(\text{Pitch}), \sin(\text{Yaw})\cos(\text{Pitch})) \]
Pitch is constrained to $[-89^\circ, 89^\circ]$ to prevent gimbal flip.

---

### Q34: What is the purpose of CMake in this project?
**Answer**: CMake generates platform-native build files (MinGW Makefiles, Visual Studio Solution, etc.) from a single `CMakeLists.txt` file, automatically managing dependencies (`glfw`, `glm`, `glad`) across operating systems.

---

### Q35: How would you scale this project for higher visual realism?
**Answer**: Add Cascaded Shadow Mapping (CSM) for dynamic directional light shadows, high-resolution Normal Maps for wood grain depth, PhysX/Bullet soft-body net deformation, and Screen Space Ambient Occlusion (SSAO).
