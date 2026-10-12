shader_type canvas_item;

uniform vec4 circle_color : source_color = vec4(1.0, 1.0, 1.0, 1.0);
uniform float radius : hint_range(0.0, 0.5) = 0.50; // Leaves room for anti-aliasing

void fragment() {
    // Center UV coordinates to [-0.5, 0.5]
    vec2 center_uv = UV - vec2(0.5);
    
    // Calculate Euclidean distance from center
    float dist = length(center_uv);
    
    // Compute screen-space derivative for scale-independent 1-pixel AA
    float delta = fwidth(dist);
    
    // Smooth transition across the circle boundary
    float alpha = 1.0 - smoothstep(radius - delta, radius + delta, dist);
    
    // Multiply by the incoming color so node modulate still applies
    COLOR *= vec4(circle_color.rgb, circle_color.a * alpha);
}