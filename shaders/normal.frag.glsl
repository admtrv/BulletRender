#version 330 core
in vec3 vNor;
uniform vec3 uLightDir;
out vec4 FragColor;

void main() {
    vec3 N = normalize(vNor);
    float ndl = max(dot(N, normalize(uLightDir)), 0.0);
    vec3 normalColor = 0.5 * (N + vec3(1.0));
    vec3 lit = normalColor * (0.2 + 0.8 * ndl);
    FragColor = vec4(lit, 1.0);
}
