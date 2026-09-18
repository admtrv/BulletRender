#version 460

layout(location=0) in vec2 aPos;
layout(location=1) in vec2 aUv;
layout(location=2) in vec4 aColor;

uniform mat4 uProj;

out vec2 vUv;
out vec4 vColor;

void main()
{
    vUv = aUv;
    vColor = aColor;
    gl_Position = uProj * vec4(aPos, 0.0, 1.0);
}
