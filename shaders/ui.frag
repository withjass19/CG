#version 330 core
in vec2 TexCoords;
out vec4 FragColor;

uniform sampler2D textTexture;
uniform vec4 textColor;
uniform bool isSolidQuad;

void main() {
    if (isSolidQuad) {
        FragColor = textColor;
    } else {
        float sampled = texture(textTexture, TexCoords).r;
        FragColor = vec4(textColor.rgb, textColor.a * sampled);
    }
}
