#version 450
#define MAX_LIGHTS 4

out vec4 FragColor;

in Surface {
    vec2 UV;
    vec3 WorldPosition;
    vec3 WorldNormal;
} fs_in;

struct Light {
    vec3 position, color;
};


uniform sampler2D _WaterTexture;

uniform Light _Lights[MAX_LIGHTS];
uniform int _NumLights;
uniform float _Ambient;
uniform float _Diffuse;
uniform float _Specular;
uniform float _Shininess;

// collect lighting
vec3 allAmbient = vec3(0.0);
vec3 allDiffuse = vec3(0.0);
vec3 allSpecular = vec3(0.0);
vec3 allReflection = vec3(0.0);

uniform vec3 _CameraPos;
uniform float uTime;
uniform float _SurfaceStrength;
uniform float _ReflectAmt;
uniform float _WaveStrength;
uniform float _WaveSpeed;
uniform float _WaveFrequency;

void main() {
    vec3 normal = normalize(fs_in.WorldNormal);

    
    vec3 normalText = texture(_WaterTexture, fs_in.UV).xyz * 2.0 - 1.0;
    normal = normalize(normal * (1.0 - _Specular) + normalText * _Specular * _SurfaceStrength);

    vec3 viewDir = normalize(_CameraPos - fs_in.WorldPosition);

    // Sine wave
    float waveDisplacement = sin(fs_in.UV.x * _WaveFrequency + uTime * _WaveSpeed) * _WaveStrength;
    vec2 distortedUV = fs_in.UV + vec2(waveDisplacement, 0.0);

  

    // Calculate lighting
    for (int i = 0; i < _NumLights; i++) {
        vec3 lightDir = normalize(_Lights[i].position - fs_in.WorldPosition);

        // Diffuse lighting
        float diff = max(dot(normal, lightDir), 0.0);
        vec3 diffuse = _Lights[i].color * _Diffuse * diff;

        // Specular lighting
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(normal, halfwayDir), 0.0), _Shininess * _Specular);
        vec3 specular = _Lights[i].color * spec;

        // Ambient lighting
        vec3 ambient = _Lights[i].color * _Ambient;

        // collecting lighting values
        allAmbient += ambient;
        allDiffuse += diffuse;
        allSpecular += specular;

        // Calculate reflection direction using the incident light direction and surface normal
        vec3 reflection = reflect(-lightDir, normal);
        vec3 reflectionColor = texture(_WaterTexture, reflection.xy).rgb;
        allReflection += reflectionColor * diff;
    }

    // Base color from texture with distorted UV coordinates
    vec3 baseColor = texture(_WaterTexture, distortedUV).rgb;

    // Combine ambient, diffuse, and specular lighting components
    vec3 ambDiffSpec = (allAmbient + allDiffuse + allSpecular);

    // Mix base color with reflection based on reflection blend factor
    vec3 finalColor = mix(baseColor * ambDiffSpec, allReflection, _ReflectAmt);

    FragColor = vec4(finalColor, 1.0);
}
