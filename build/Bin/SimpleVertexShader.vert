#version 330 core
// Primim toate cele 4 float-uri la location 0:
// vertex.xy = Pozitia (X, Y)
// vertex.zw = Coordonatele Texturii (U, V)
layout (location = 0) in vec4 vertex;
out vec2 TexCoord;
// Matricea model pe care o trimite DrawSprite
uniform mat4 model;
void main() {
    TexCoord = vertex.zw; // Trimitem coordonatele de textura la fragment shader
    gl_Position = model * vec4(vertex.xy, 0.0, 1.0); // Aplicam translatia, rotatia si scalarea
}