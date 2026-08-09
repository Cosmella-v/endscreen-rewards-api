
uniform sampler2D u_texture;
uniform float u_fadeSize;

varying vec2 vTexCoords;

void main() {
    vec4 color = texture2D(u_texture, vTexCoords);
    float fade = smoothstep(0.0, u_fadeSize, vTexCoords.y)
               * smoothstep(0.0, u_fadeSize, 1.0 - vTexCoords.y);
    gl_FragColor = vec4(color.rgb, color.a * fade);
}