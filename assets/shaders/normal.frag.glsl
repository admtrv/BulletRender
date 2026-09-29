#version 330 core

#define MAX_POINT_LIGHTS 3
#define MAX_SPOT_LIGHTS 3

in vec3 vWorldPos;
in vec3 vNor;
in vec2 vUv;
in vec4 vTan;

uniform int uUnlit;

// cell of a sheet this object shows, whole picture when it shows no sheet
uniform vec2 uFrameScale;
uniform vec2 uFrameOffset;

// alpha a picture carries is either ignored, cut at a threshold, or blended
uniform int uAlphaMask;
uniform float uAlphaCutoff;

// each slot samples its own window of its own picture
uniform vec3 uDiffuse;
uniform sampler2D uDiffuseMap;
uniform int uHasDiffuseMap;
uniform vec2 uDiffuseOffset;
uniform vec2 uDiffuseScale;
uniform vec2 uDiffuseFlip;

uniform vec3 uSpecular;
uniform float uShininess;
uniform sampler2D uSpecularMap;
uniform int uHasSpecularMap;
uniform vec2 uSpecularOffset;
uniform vec2 uSpecularScale;
uniform vec2 uSpecularFlip;

uniform sampler2D uNormalMap;
uniform int uHasNormalMap;
uniform vec2 uNormalOffset;
uniform vec2 uNormalScale;
uniform vec2 uNormalFlip;

uniform vec3 uEmissive;
uniform sampler2D uEmissiveMap;
uniform int uHasEmissiveMap;
uniform vec2 uEmissiveOffset;
uniform vec2 uEmissiveScale;
uniform vec2 uEmissiveFlip;

uniform vec3 uCameraPos;

uniform vec3 uAmbientColor;

uniform int uHasDirLight;
uniform vec3 uDirLightDir;
uniform vec3 uDirLightColor;
uniform int uDirCastsShadow;
uniform mat4 uDirLightVP;
uniform sampler2D uDirShadowMap;

struct PointLight {
    vec3 position;
    vec3 color;
    float range;
};

struct SpotLight {
    vec3 position;
    vec3 direction;
    vec3 color;
    float range;
    float innerCos;
    float outerCos;
};

uniform int uPointCount;
uniform PointLight uPointLights[MAX_POINT_LIGHTS];

uniform int uSpotCount;
uniform SpotLight uSpotLights[MAX_SPOT_LIGHTS];
uniform mat4 uSpotLightVP[MAX_SPOT_LIGHTS];
uniform int uSpotCastsShadow[MAX_SPOT_LIGHTS];
uniform sampler2D uSpotShadowMap[MAX_SPOT_LIGHTS];

out vec4 FragColor;

// smooth distance falloff using range
float attenuate(float dist, float range)
{
    float x = clamp(1.0 - (dist * dist) / (range * range), 0.0, 1.0);
    return x * x;
}

// phong (diffuse + specular) for a single light source, with per-material ks/ns
vec3 phong(vec3 N, vec3 V, vec3 L, vec3 lightColor, vec3 surface, vec3 specColor, float shininess)
{
    float ndl = max(dot(N, L), 0.0);
    vec3 diffuse = surface * lightColor * ndl;

    vec3 R = reflect(-L, N);
    float spec = pow(max(dot(R, V), 0.0), max(shininess, 1.0));
    vec3 specular = lightColor * specColor * spec * step(0.0, ndl);

    return diffuse + specular;
}

// pcf 3x3 shadow lookup, returns visibility 0..1 (1 = fully lit)
float sampleShadow(sampler2D shadowMap, vec4 lightSpacePos, float ndl)
{
    // perspective divide and remap from [-1,1] to [0,1]
    vec3 ndc = lightSpacePos.xyz / lightSpacePos.w;
    ndc = ndc * 0.5 + 0.5;

    // outside the shadow map view frustum -> fully lit (border color = 1)
    if (ndc.z > 1.0) return 1.0;

    // slope-scaled bias to fight acne
    float bias = max(0.005 * (1.0 - ndl), 0.0008);

    vec2 texelSize = 1.0 / vec2(textureSize(shadowMap, 0));
    float shadow = 0.0;
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            float depth = texture(shadowMap, ndc.xy + vec2(x, y) * texelSize).r;
            shadow += (ndc.z - bias > depth) ? 0.0 : 1.0;
        }
    }
    return shadow / 9.0;
}

// flip first, so window names same corner either way round, then frame cuts to one cell
vec2 slotUv(vec2 offset, vec2 scale, vec2 flip)
{
    return (mix(vUv, 1.0 - vUv, flip) * scale + offset) * uFrameScale + uFrameOffset;
}

// map holds the detail in the surface's own frame, tbn carries it out to the world
vec3 mappedNormal(vec3 normal)
{
    vec3 tangent = normalize(vTan.xyz - normal * dot(normal, vTan.xyz));
    vec3 bitangent = cross(normal, tangent) * vTan.w;

    vec3 sampled = texture(uNormalMap, slotUv(uNormalOffset, uNormalScale, uNormalFlip)).rgb * 2.0 - 1.0;

    return normalize(mat3(tangent, bitangent, normal) * sampled);
}

void main()
{
    vec3 N = normalize(vNor);

    if (uHasNormalMap != 0)
    {
        N = mappedNormal(N);
    }
    vec3 V = normalize(uCameraPos - vWorldPos);

    vec3 base = uDiffuse;
    float alpha = 1.0;

    if (uHasDiffuseMap != 0)
    {
        vec4 sampled = texture(uDiffuseMap, slotUv(uDiffuseOffset, uDiffuseScale, uDiffuseFlip));

        base *= sampled.rgb;
        alpha = sampled.a;
    }

    // masked surface keeps a pixel whole or drops it whole, nothing between
    if (uAlphaMask != 0 && alpha < uAlphaCutoff) discard;

    // what is barely there is dropped, so depth stays true for passes that read it
    if (alpha < 0.01) discard;

    vec3 specColor = uSpecular;

    if (uHasSpecularMap != 0)
    {
        specColor *= texture(uSpecularMap, slotUv(uSpecularOffset, uSpecularScale, uSpecularFlip)).rgb;
    }

    float shininess = uShininess;

    vec3 emissive = uEmissive;

    if (uHasEmissiveMap != 0)
    {
        emissive *= texture(uEmissiveMap, slotUv(uEmissiveOffset, uEmissiveScale, uEmissiveFlip)).rgb;
    }

    // ambient + emissive
    vec3 color = base * uAmbientColor + emissive;

    // directional
    if (uHasDirLight != 0)
    {
        vec3 L = normalize(uDirLightDir);
        float ndl = max(dot(N, L), 0.0);

        float vis = 1.0;
        if (uDirCastsShadow != 0)
        {
            vec4 lsp = uDirLightVP * vec4(vWorldPos, 1.0);
            vis = sampleShadow(uDirShadowMap, lsp, ndl);
        }

        color += phong(N, V, L, uDirLightColor, base, specColor, shininess) * vis;
    }

    // points (no shadows)
    for (int i = 0; i < uPointCount; i++)
    {
        vec3 toLight = uPointLights[i].position - vWorldPos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 1e-4);
        float att = attenuate(dist, uPointLights[i].range);
        color += phong(N, V, L, uPointLights[i].color, base, specColor, shininess) * att;
    }

    // spots
    for (int i = 0; i < uSpotCount; i++)
    {
        vec3 toLight = uSpotLights[i].position - vWorldPos;
        float dist = length(toLight);
        vec3 L = toLight / max(dist, 1e-4);

        float cosTheta = dot(-L, normalize(uSpotLights[i].direction));
        float coneFactor = smoothstep(uSpotLights[i].outerCos, uSpotLights[i].innerCos, cosTheta);
        float att = attenuate(dist, uSpotLights[i].range) * coneFactor;

        float vis = 1.0;
        if (uSpotCastsShadow[i] != 0 && att > 0.0)
        {
            float ndl = max(dot(N, L), 0.0);
            vec4 lsp = uSpotLightVP[i] * vec4(vWorldPos, 1.0);
            vis = sampleShadow(uSpotShadowMap[i], lsp, ndl);
        }

        color += phong(N, V, L, uSpotLights[i].color, base, specColor, shininess) * att * vis;
    }

    // flat picture carries its own shading, light would only dull it
    FragColor = vec4(uUnlit != 0 ? base + emissive : color, alpha);
}
