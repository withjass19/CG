#include "Shader.h"

// Default Embedded Shaders (Fallback if file loading fails)
static const char* DEFAULT_LIGHTING_VERT = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    TexCoords = aTexCoords;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

static const char* DEFAULT_LIGHTING_FRAG = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
uniform sampler2D texture_diffuse1;
uniform bool useTexture;
uniform vec3 viewPos;
uniform vec4 colorOverride;

void main() {
    if (colorOverride.a > 0.0) {
        FragColor = colorOverride;
        return;
    }

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Main Directional Light
    vec3 lightDir = normalize(vec3(0.5, 1.2, 0.8));
    vec3 ambient = 0.55 * material.ambient;
    float diff = max(dot(norm, lightDir), 0.15);
    vec3 diffuse = diff * material.diffuse;
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), max(material.shininess, 1.0));
    vec3 specular = spec * material.specular;

    // Overhead Court Point Light
    vec3 pointLightPos = vec3(0.0, 7.0, -10.0);
    vec3 pointLightDir = normalize(pointLightPos - FragPos);
    float dist = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.03 * dist + 0.005 * dist * dist);
    float pDiff = max(dot(norm, pointLightDir), 0.1);
    vec3 pointDiffuse = pDiff * material.diffuse * attenuation * 0.6;

    vec3 lighting = ambient + diffuse + specular + pointDiffuse;

    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        if (texColor.a < 0.05) discard;
        FragColor = vec4(lighting, 1.0) * texColor;
    } else {
        FragColor = vec4(lighting, 1.0);
    }
}
)";

static const char* DEFAULT_TRAJECTORY_VERT = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

static const char* DEFAULT_TRAJECTORY_FRAG = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 dotColor;

void main() {
    FragColor = dotColor;
}
)";

Shader::Shader() : ID(0) {}

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    bool fileLoaded = false;
    try {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        vShaderFile.close();
        fShaderFile.close();
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
        if (!vertexCode.empty() && !fragmentCode.empty()) {
            fileLoaded = true;
        }
    }
    catch (std::ifstream::failure& e) {
        (void)e;
    }

    if (!fileLoaded) {
        std::string vPath(vertexPath);
        if (vPath.find("trajectory") != std::string::npos) {
            vertexCode = DEFAULT_TRAJECTORY_VERT;
            fragmentCode = DEFAULT_TRAJECTORY_FRAG;
        } else {
            vertexCode = DEFAULT_LIGHTING_VERT;
            fragmentCode = DEFAULT_LIGHTING_FRAG;
        }
    }

    compileFromSource(vertexCode.c_str(), fragmentCode.c_str());
}

Shader::Shader(const std::string& vertexCode, const std::string& fragmentCode, bool fromMemory) {
    (void)fromMemory;
    compileFromSource(vertexCode.c_str(), fragmentCode.c_str());
}

void Shader::compileFromSource(const char* vShaderCode, const char* fShaderCode) {
    GLuint vertex, fragment;

    // Vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() const {
    if (ID != 0) {
        glUseProgram(ID);
    }
}

void Shader::setBool(const std::string &name, bool value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string &name, int value) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, float value) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string &name, const glm::vec2 &value) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), value.x, value.y);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), value.x, value.y, value.z, value.w);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::checkCompileErrors(GLuint shader, std::string type) {
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
