#version 330 core

in vec2 TexCoord;
out vec4 color;

uniform sampler2D ourTexture;
uniform vec3 spriteColor; // Culoarea/tenta pe care o trimite DrawSprite

void main() {
    // Inmultim culoarea pixelului din textura cu spriteColor
    // (Daca spriteColor e alb (1,1,1), textura ramane exact originala)
    color = vec4(spriteColor, 1.0) * texture(ourTexture, TexCoord);
}