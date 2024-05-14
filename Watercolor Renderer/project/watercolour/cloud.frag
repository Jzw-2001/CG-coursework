#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D texture1;
uniform float time;

// Function to generate 2D noise based on a given position
float noise(vec2 p) {
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// Smooth interpolation
float smoothNoise(vec2 p) {
    vec2 i = floor(p);
    vec2 f = fract(p);

    float a = noise(i);
    float b = noise(i + vec2(1.0, 0.0));
    float c = noise(i + vec2(0.0, 1.0));
    float d = noise(i + vec2(1.0, 1.0));

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

void main()
{
    // Generate noise based on the texture coordinates and time for animation
    float n = smoothNoise(TexCoord * 10.0 + vec2(time * 0.1, time * 0.1));

    // Apply the noise to the alpha value to create the cloud effect
    float alpha = mix(0.4, 0.7, n);

    // Set the cloud color (white) and use the alpha value
    vec4 cloudColor = vec4(1.0, 1.0, 1.0, alpha);

    FragColor = cloudColor;
}
