#version 450

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vUV;

out Surface {
    vec2 UV;
    vec3 WorldPosition;
    vec3 WorldNormal;
} vs_out;

uniform mat4 _Model;
uniform mat4 _ViewProjection;

uniform float uTime;
uniform float _SurfaceSpeed;

void main() {
    vs_out.UV = vUV;

    
    float timeSpeed = -uTime * _SurfaceSpeed;
    //UV coordinates
    vec2 animatedUV = vUV + vec2(timeSpeed, timeSpeed);
    vs_out.UV = animatedUV;

    
    vec4 modelPos = _Model * vec4(vPos, 1.0);
    vs_out.WorldPosition = modelPos.xyz;
    

    vs_out.WorldNormal = transpose(inverse(mat3(_Model))) * vNormal;
    
    // Combine transformations
    gl_Position = _ViewProjection * modelPos;
}
