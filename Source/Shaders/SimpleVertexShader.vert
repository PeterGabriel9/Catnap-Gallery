#version 330 core

// Input vertex data, different for all executions of this shader.
// The position variable has attribute position 0
layout(location = 0) in vec3 vertexPos; 
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
// Add uniform variable for the transform matrix
uniform mat4 transform;

void main(){

 gl_Position = vec4(vertexPos, 1.0);
    gl_Position = transform * gl_Position;
  
    TexCoord = aTexCoord;

}
