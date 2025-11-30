#version 460

layout(location = 0) out vec4 FragColor;
in vec2 uv;

uniform sampler2D uSceneColor;
uniform sampler2D uSceneDepth;

uniform mat4 uInvViewProj;
uniform vec3 uCameraPos;
uniform float uFogStart;
uniform float uFogEnd;
uniform vec3 uFogColor;

vec3 unproject(vec2 t, float z)
{
    vec4 p = uInvViewProj * vec4(t * 2.0 - 1.0, z, 1.0);
    return p.xyz / p.w;
}

void main()
{
    vec3 sceneColor = texture(uSceneColor, uv).rgb;
    float depth = texture(uSceneDepth, uv).r;

    // reconstruct world position from depth
    float ndcZ = depth * 2.0 - 1.0;
    vec3 worldPos = unproject(uv, ndcZ);

    // calculate distance fog
    float dist = length(worldPos - uCameraPos);
    float fogFactor = smoothstep(uFogStart, uFogEnd, dist);

    // blend scene with fog
    vec3 finalColor = mix(sceneColor, uFogColor, fogFactor);

    FragColor = vec4(finalColor, 1.0);
}
