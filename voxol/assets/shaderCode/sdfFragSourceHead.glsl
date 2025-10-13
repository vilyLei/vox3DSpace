#version 330 core
precision     mediump float;
uniform vec4      u_color;
in vec2           v_uv;
out vec4          fragColor;

const float PI = 3.14159265359;
const float factor = 0.5 / 255.0;
const float c = 43758.5453;
const vec2 uvFactor = vec2( 12.9898 ,78.233 );
float randUV( vec2 uv ) {
    float dt = dot( uv.xy, uvFactor );
    float sn = mod( dt, PI );
    return fract(sin(sn) * c);
}
const vec4 dither_fector = vec4( factor, -factor, factor, factor);
vec4 dithering( vec4 color, vec2 puv ) {
    float grid_position = randUV( puv );
    
    return color + mix( 2.0 * dither_fector, -2.0 * dither_fector, grid_position );
    //color = pow(color, vec4(2.2));
    //color = color + mix( 2.0 * dither_fector, -2.0 * dither_fector, grid_position );
    //return pow(color, vec4(1.0/2.2));
}
float aa(float d) {
    float factor = fwidth(d);
    //return clamp(0.5 - d / factor, 0.0, 1.0);
    return smoothstep(0.0, factor, -d);
}