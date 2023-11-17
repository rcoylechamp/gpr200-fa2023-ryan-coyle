#version 450
#define MAX_LIGHTS 4

out vec4 FragColor;

in Surface{
    vec2 UV;
    vec3 WorldPosition, WorldNormal;
} fs_in;

struct Light {
    vec3 position, color;
};

uniform float _Ambient;
uniform float _Diffuse;
uniform float _Specular;
uniform float _Shininess;

uniform Light _Lights[MAX_LIGHTS];
uniform sampler2D _Texture;
uniform vec3 _LightColor, _CameraPos;
uniform int _NumLights;
void main() {
    vec3 normal = normalize(fs_in.WorldNormal);
    vec3 cameraDir = normalize(_CameraPos - fs_in.WorldPosition);
    vec3 ambient = _Ambient * _LightColor;
    vec3 dNs;

    for (int i = 0; i < _NumLights; ++i) {
        vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);
        float diffuseFactor = max(dot(normal, lightDir), 0.0);

        vec3 halfwayDir = normalize(lightDir + cameraDir);
        float specAngle = max(dot(normal, halfwayDir), 0.0);
        float specularFactor = pow(specAngle, _Shininess);

        vec3 diffuse = _Diffuse * _Lights[i].color * diffuseFactor;
        vec3 specular = _Specular * _Lights[i].color * specularFactor;

        dNs += diffuse + specular;
    }

 
    vec4 textureColor = texture(_Texture, fs_in.UV);
    vec3 finalColor = (ambient + dNs) * textureColor.rgb;

    FragColor = vec4(finalColor, textureColor.a);
}