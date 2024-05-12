#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D texture_ambient;
uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;
uniform sampler2D texture_specularHighlight;
uniform bool has_alpha;
uniform bool useTexture;
uniform sampler2D texture_alpha;
uniform sampler2D texture_bump;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 lightColor;

uniform sampler2D shadowMap;
uniform mat4 lightSpaceMatrix;


uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float transparency;
} material;


float ShadowCalculation(vec4 fragPosLightSpace) {
    // Transform fragment position to light space
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // Transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // Get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // Get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // Check whether current frag pos is in shadow
    float shadow = currentDepth > closestDepth + 0.005 ? 1.0 : 0.0;
    return shadow;
}



void main() {
    
    // Ambient
    vec3 ambient = texture(texture_ambient, TexCoords).rgb * material.ambient;
    if(!useTexture) ambient = material.ambient;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(texture_diffuse, TexCoords).rgb * material.diffuse;
    if(!useTexture) diffuse = diff * material.diffuse;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = spec * texture(texture_specular, TexCoords).rgb * material.specular;
    if(!useTexture) specular = spec * material.specular;


    // Shadow
    vec4 fragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    float shadow = ShadowCalculation(fragPosLightSpace);

    // Combine results
    // vec3 result = ambient + diffuse + specular;
    vec3 result = (1.0 - shadow) * (ambient + diffuse + specular);

    if(has_alpha) {
        FragColor = vec4(result, texture(texture_alpha, TexCoords).r);
    }
    else {
        FragColor = vec4(result, texture(texture_diffuse, TexCoords).a);
	}
    
}
