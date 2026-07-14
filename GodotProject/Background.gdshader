shader_type canvas_item;

// If you want to pass textures or extra uniforms, define them here
// uniform sampler2D my_texture;

void fragment() {
    // 1. Get UV coordinates (0.0 to 1.0)
    vec2 uv = UV;

    // 2. Adjust for aspect ratio (equivalent to Shadertoy's normalization)
    // Godot gives us the screen/rect resolution in pixels via SCREEN_PIXEL_SIZE
    vec2 resolution = 1.0 / SCREEN_PIXEL_SIZE;
    vec2 fragCoord = uv * resolution;

    // Standard Shadertoy centering and aspect ratio correction
    vec2 uv_corrected = (fragCoord - 0.5 * resolution.xy) / resolution.y;

    // 3. Time variable
    // Godot provides TIME out of the box (equivalent to iTime)
    float time = TIME;

    // --- YOUR SHADERTOY LOGIC GOES HERE ---
    // Example: A simple animated psychedelic gradient
    vec3 color = 0.5 + 0.5 * cos(time + uv_corrected.xyx + vec3(0.0, 2.0, 4.0));

    // 4. Output the final pixel color
    COLOR = vec4(color, 1.0);
}