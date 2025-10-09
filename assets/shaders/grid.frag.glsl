#version 460

layout(location = 0) out vec4 FragColor;
in vec2 uv;

uniform mat4 uInvViewProj;   // inverse(P * V)
uniform mat4 uViewProj;      // P * V

// LOD fade thresholds in pixels-per-cell
const float FADE_START_PX = 6.0;    // start fading when cell is ~6 px
const float FADE_END_PX   = 3.0;    // fully fade out by ~3 px

vec3 unproject(vec2 t, float z)
{
    vec4 p = uInvViewProj * vec4(t * 2.0 - 1.0, z, 1.0);
    return p.xyz / p.w;
}

void main()
{
    // ray through the pixel
    vec3 pNear = unproject(uv, -1.0);
    vec3 pFar  = unproject(uv,  1.0);
    vec3 dir   = normalize(pFar - pNear);

    // intersect with plane y = 0
    if (abs(dir.y) < 1e-6)
        discard;
    float t = -pNear.y / dir.y;
    if (t < 0.0)
        discard;

    vec3 pos = pNear + dir * t;

    // depth for proper intersections with scene geometry
    vec4 clip  = uViewProj * vec4(pos, 1.0);
    float depth = clip.z / clip.w * 0.5 + 0.5;
    gl_FragDepth = clamp(depth - 1e-5, 0.0, 1.0);

    // distance to nearest grid line in cell units
    float fx = min(fract(pos.x), 1.0 - fract(pos.x));
    float fz = min(fract(pos.z), 1.0 - fract(pos.z));

    // cell-units per pixel
    float dx = max(fwidth(pos.x), 1e-6);
    float dz = max(fwidth(pos.z), 1e-6);

    // pixels per cell (LOD metric)
    float pxPerCell = min(1.0 / dx, 1.0 / dz);

    // 1 px line thickness with 1 px antialias
    const float halfPx = 0.5;
    float pxX = fx / dx;
    float pxZ = fz / dz;
    float maskX = 1.0 - smoothstep(halfPx, halfPx + 1.0, pxX);
    float maskZ = 1.0 - smoothstep(halfPx, halfPx + 1.0, pxZ);
    float line  = max(maskX, maskZ);

    // LOD fade
    float lod = smoothstep(FADE_END_PX, FADE_START_PX, pxPerCell);
    line *= lod;

    if (line <= 0.0)
        discard;

    FragColor = vec4(1.0);
}
