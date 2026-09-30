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

    // Main Directional Light (Sun/Stadium Floodlight)
    vec3 lightDir = normalize(vec3(0.5, 1.2, 0.8));
    
    // Ambient
    vec3 ambient = 0.45 * material.ambient;
    
    // Diffuse
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;
    
    // Specular (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    // Court Point Spotlight near hoop
    vec3 pointLightPos = vec3(0.0, 7.0, -10.0);
    vec3 pointLightDir = normalize(pointLightPos - FragPos);
    float dist = length(pointLightPos - FragPos);
    float attenuation = 1.0 / (1.0 + 0.04 * dist + 0.007 * dist * dist);
    float pDiff = max(dot(norm, pointLightDir), 0.0);
    vec3 pointDiffuse = pDiff * material.diffuse * vec3(0.9, 0.95, 1.0) * attenuation * 0.8;

    vec3 result = (ambient + diffuse + specular + pointDiffuse);

    if (useTexture) {
        vec4 texColor = texture(texture_diffuse1, TexCoords);
        FragColor = vec4(result, 1.0) * texColor;
    } else {
        FragColor = vec4(result, 1.0);
    }
}
