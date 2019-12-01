R"(
#version 330 core
uniform sampler2D noiseTex;

in vec3 vposition;
in vec2 vtexcoord;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

out vec2 uv;
out vec3 fragPos;

float random (vec2 co) {
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    /// TODO: Get height h at uv
    float h = 0.0f;

    uv = vtexcoord;
    fragPos = vposition.xyz + vec3(0,0,h);
    gl_Position = P*V*M*vec4(vposition.x, vposition.y, vposition.z + h, 1.0);
}
)"
