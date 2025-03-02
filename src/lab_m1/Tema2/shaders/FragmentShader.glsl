#version 330

// Input from vertex shader
in vec3 frag_position;
in vec3 frag_normal;
in vec3 frag_color;
in vec3 frag_text;
in float frag_noise;

// Output
layout(location = 0) out vec4 out_color;

void main()
{
    // Use the original color for non-terrain objects
    if (frag_noise < 0.0) {
        out_color = vec4(frag_color, 1);
        return;
    }
    
    // Colors for terrain
    vec3 grass = vec3(0.2, 0.5, 0.1);
    vec3 rock = vec3(0.5, 0.5, 0.5);
    vec3 snow = vec3(0.9, 0.9, 0.9);
    
    vec3 color;
    if (frag_noise < 0.33) {
        color = mix(grass, rock, frag_noise * 3.0);
    } else if (frag_noise < 0.66) {
        color = mix(rock, snow, (frag_noise - 0.33) * 3.0);
    } else {
        color = snow;
    }
    
    out_color = vec4(color, 1.0);
}