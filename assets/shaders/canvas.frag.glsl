#version 460

in vec2 vUv;
in vec4 vColor;

uniform sampler2D uTexture;

layout(location=0) out vec4 FragColor;

void main()
{
    FragColor = vColor * texture(uTexture, vUv);
}
