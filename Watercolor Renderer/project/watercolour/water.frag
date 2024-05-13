#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform bool has_alpha;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;
uniform float time;  // 动态效果的时间参数

uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float transparency;
} material;

float ShadowCalculation(vec4 fragPosLightSpace) {
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
    return shadow;
}

void main() {
    // 动态改变法线来模拟水波
    vec3 disturbedNormal = Normal;
    disturbedNormal.x += sin(4.0 * FragPos.x + 2.0 * time) * 0.1;
    disturbedNormal.z += sin(4.0 * FragPos.z + 2.0 * time) * 0.1;
    disturbedNormal = normalize(disturbedNormal);

    // Ambient
    vec3 ambient = material.ambient;

    // Diffuse
    vec3 norm = normalize(disturbedNormal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * material.diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * material.specular;

    // 菲涅耳效应
    float fresnel = pow(1.0 - dot(viewDir, norm), 3.0);

    // 简单的环境反射和折射模拟
    vec3 reflectionColor = vec3(0.6, 0.7, 0.8); // 假设的天空颜色
    vec3 refractionColor = vec3(0.1, 0.3, 0.5); // 假设的水下颜色
    vec3 finalColor = mix(refractionColor, reflectionColor, fresnel);

    // Shadow
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    // Combine results
    vec3 result = (1.0 - shadow) * (diffuse + specular) + ambient;
    result = result * finalColor; // 将反射和折射的颜色应用到最终结果

    FragColor = vec4(result, material.transparency);
}
