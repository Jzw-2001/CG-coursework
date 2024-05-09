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

uniform struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float transparency;
} material;

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

    // Combine results
    vec3 result = ambient + diffuse + specular;
    if(has_alpha) {
        FragColor = vec4(result, texture(texture_alpha, TexCoords).r);
    }
    else {
        FragColor = vec4(result, texture(texture_diffuse, TexCoords).a);
	}
    
    // test with diffuse only
    // FragColor = vec4(texture(texture_diffuse, TexCoords).rgb, 1.0f);
}
