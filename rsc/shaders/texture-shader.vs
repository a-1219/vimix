#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;
layout (location = 2) in vec3 texCoord;

out vec3 vertexColor;
out vec3 vertexUV;

uniform mat4 modelview;
uniform mat4 projection;

void main()
{
    vec4 pos = modelview * vec4(position.xyz, 1.0);

    // output
    gl_Position = projection * pos;
    vertexColor = color;
    vertexUV = texCoord;
}
