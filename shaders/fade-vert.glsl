attribute vec2 aPosition;
attribute vec2 aTexCoords;

uniform mat4 u_MVPMatrix;

varying vec2 vTexCoords;

void main() {
    vTexCoords = aTexCoords;
    gl_Position = u_MVPMatrix * vec4(aPosition, 0.0, 1.0);
}
