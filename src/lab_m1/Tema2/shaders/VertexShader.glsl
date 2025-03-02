#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 3) in vec3 v_normal;
layout(location = 2) in vec3 v_text;
layout(location = 1) in vec3 v_color;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform float time;

// Output
out vec3 frag_position;
out vec3 frag_normal;
out vec3 frag_color;
out vec3 frag_text;
out float frag_noise;

float noise2D(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);
    
    float a = fract(sin(dot(i, vec2(12.9898, 78.233))) * 43758.5453123);
    float b = fract(sin(dot(i + vec2(1.0, 0.0), vec2(12.9898, 78.233))) * 43758.5453123);
    float c = fract(sin(dot(i + vec2(0.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453123);
    float d = fract(sin(dot(i + vec2(1.0, 1.0), vec2(12.9898, 78.233))) * 43758.5453123);

    vec2 u = f * f * (3.0 - 2.0 * f);

    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

void main()
{
    vec3 position = v_position;
    
    if (abs(position.y) < 0.001) {
        float frequency = 0.3;
        vec2 noiseCoord = position.xz * frequency;
        
        // Create flat zones using a step function
        float flatZone = 0.0;
        
        // Define multiple flat zones
        vec2 zone1 = vec2(10.0, 10.0);   // Coordinates of the center of the first zone
        vec2 zone2 = vec2(-15.0, -5.0);  // Coordinates of the center of the second zone
        vec2 zone3 = vec2(0.0, -20.0);   // Coordinates of the center of the third zone
        
        // Check if we are in a flat zone (radius of 5 units)
        if (length(position.xz - zone1) < 2.0 || 
            length(position.xz - zone2) < 2.0 || 
            length(position.xz - zone3) < 2.0) {
            flatZone = 1.0;
        }
        
        float n = noise2D(noiseCoord) * 0.5 +
                  noise2D(noiseCoord * 2.0) * 0.25 +
                  noise2D(noiseCoord * 4.0) * 0.125;
                  
        // Apply height only outside the flat zones
        position.y = mix(n * 2.0, 0.2, flatZone);  // 0.2 is the height of the flat zones
        frag_noise = mix(n, 0.3, flatZone);  // Use a fixed value for the color of the flat zones
    } else {
        frag_noise = -1.0;
    }

    frag_position = position;
    frag_normal = v_normal;
    frag_color = v_color;
    frag_text = v_text;

    gl_Position = Projection * View * Model * vec4(position, 1.0);
}