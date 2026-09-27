shader_type canvas_item;

// Custom configuration uniforms
uniform vec3 tri_color : source_color = vec3(0.1, 0.1, 0.1);
uniform vec3 bg_color : source_color = vec3(0.0, 0.0, 0.0);
uniform float scale = 10.0;
uniform float gap = 0.01;

// Grid Scrolling Configuration
uniform vec2 scroll_dir = vec2(-1.0, -1.0);
uniform float scroll_speed_val = 0.004;

// Sine Ripple Configuration
uniform float ripple_speed = 5.0;
uniform float ripple_frequency = 8.0;
uniform float max_shrink : hint_range(0.0, 1.0) = 0.92;

// Linear Wave Direction
uniform vec2 wave_dir = vec2(-1.0, 0.5);

void fragment() {
    // Replicate Shadertoy iResolution.y normalizing
    vec2 resolution = 1.0 / SCREEN_PIXEL_SIZE;
    vec2 uv = (FRAGCOORD.xy - 0.5 * resolution) / resolution.y;

    // Calculate scroll vector using Godot's built-in TIME variable
    vec2 scroll_speed = normalize(scroll_dir) * scroll_speed_val;
    vec2 p = (uv + scroll_speed * TIME) * scale;

    // Fixed Inverse Matrix: Matching GLSL column-major mat2(1.0, 0.0, -0.57735026919, 1.15470053838)
    mat2 inv_simplex = mat2(
        vec2(1.0, 0.0),
        vec2(-0.57735026919, 1.15470053838)
    );
    vec2 tri_coord = inv_simplex * p;

    // Separate into integer grid IDs and fractional cell coordinates
    vec2 i = floor(tri_coord);
    vec2 f = fract(tri_coord);

    // Fixed Forward Matrix: Matching GLSL column-major mat2(1.0, 0.0, 0.5, 0.86602540378)
    mat2 simplex = mat2(
        vec2(1.0, 0.0),
        vec2(0.5, 0.86602540378)
    );

    // Determine triangle ID and calculate cell-centric coordinates
    float dist = 0.0;
    vec2 cell_center_id = i;

    if (f.x + f.y < 1.0) {
        // Lower-left triangle
        dist = min(f.x, min(f.y, 1.0 - f.x - f.y));
        cell_center_id += vec2(1.0 / 3.0, 1.0 / 3.0);
    } else {
        // Upper-right triangle
        dist = min(1.0 - f.x, min(1.0 - f.y, f.x + f.y - 1.0));
        cell_center_id += vec2(2.0 / 3.0, 2.0 / 3.0);
    }

    // Convert cell center back to static screen space
    vec2 cell_world_pos = (simplex * cell_center_id) / scale;

    // --- Linear Sine Wave Front Math ---
    vec2 normalized_wave_dir = normalize(wave_dir);
    float position_along_wave = dot(cell_world_pos, normalized_wave_dir);

    // Continuous Sine Wave
    float wave_phase = position_along_wave * ripple_frequency - TIME * ripple_speed;
    float sine_val = sin(wave_phase);

    // Map range [-1.0, 1.0] -> [0.0, 1.0]
    float ripple_intensity = sine_val * 0.5 + 0.5;

    // Scale factor smoothly oscillates between 1.0 and max_shrink
    float scale_factor = mix(1.0, max_shrink, ripple_intensity);

    // Uniformly scale triangle inward from its center
    dist = dist - 0.33333 * (1.0 - scale_factor);

    // Antialiasing / Smoothing
    float pixel_size = scale / resolution.y;
    float mask = smoothstep(0.0, pixel_size * 1.5, dist - gap);

    // Render final color
    vec3 final_color = mix(bg_color, tri_color, mask);
    COLOR = vec4(final_color, 1.0);
}