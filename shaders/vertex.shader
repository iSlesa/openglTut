#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoor;

out vec3 fNormal;
out vec3 fragPosition;
out vec2 textureCoor;

uniform mat4 model;
uniform mat4 mvp;

void main(){
    gl_Position = mvp * vec4(position, 1.0f);
    fNormal = mat3(transpose(inverse(model))) * normal;
    fragPosition = vec3(model * vec4(position, 1.0f));
    textureCoor = texCoor;
    }
