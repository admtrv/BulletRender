#version 330 core

layout (location=0) in vec3 aPos;
layout (location=1) in vec3 aNor;
layout (location=2) in vec2 aUv;
layout (location=3) in vec4 aTan;

uniform mat4 uModel;
uniform mat3 uNormalMatrix;
uniform mat4 uView;
uniform mat4 uProj;

out vec3 vWorldPos;
out vec3 vNor;
out vec2 vUv;
out vec4 vTan;

void main()
{
    vec4 worldPos = uModel * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    vNor = normalize(uNormalMatrix * aNor);
    vUv = aUv;

    // sign rides along untouched, it says which way the third axis runs
    vTan = vec4(normalize(uNormalMatrix * aTan.xyz), aTan.w);

    gl_Position = uProj * uView * worldPos;
}
