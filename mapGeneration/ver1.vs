#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;
layout(location = 2) in float texType;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


out vec4 outColor;
out vec2 TexCoord;
out float boolTexType;

void main() {
gl_Position = projection * view * model * vec4(aPos.x*2, aPos.y*2, aPos.z*2, 1.0);
outColor = vec4(1.0, 0.0, 0.5, 1.0);
TexCoord = aTex;
boolTexType = texType;
}