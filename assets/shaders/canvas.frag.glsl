#version 460

in vec2 vUv;
in vec4 vColor;
in float vCoverage;

uniform sampler2D uTexture;

layout(location=0) out vec4 FragColor;

void main()
{
    vec4 texel = texture(uTexture, vUv);

    // glyph atlas holds coverage alone, its red channel stands for alpha
    vec4 sampled = mix(texel, vec4(1.0, 1.0, 1.0, texel.r), vCoverage);

    FragColor = vColor * sampled;
}
