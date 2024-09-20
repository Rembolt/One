#version 450

//Vertex shader: proccess incoming vertex(world position, color, normal, texture cordinates)
//output is the position in clip cordinates(screen) and atributes for fragment shader(color, texture cordinates)
//clip cordinates are 4d, the last item divides all others to form a normalized coordinate that fits:
//[-1,-1]       [1,-1]
//
//[-1, 1]       [1, 1]
layout(location = 0) out vec3 fragColor;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    fragColor = colors[gl_VertexIndex];
}