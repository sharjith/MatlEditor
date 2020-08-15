#version 400

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
in vec2 texCoord2d;

uniform mat4 modelViewMatrix;
uniform mat3 normalMatrix;
uniform mat4 projectionMatrix;
uniform vec4 clipPlaneX;
uniform vec4 clipPlaneY;
uniform vec4 clipPlaneZ;

out float clipDistX;
out float clipDistY;
out float clipDistZ;

out vec3 v_normal;
out vec3 v_position;
out vec2 v_texCoord2d;	
out mat4 MVP;

void main()
{
    v_normal     = normalize(normalMatrix * vertexNormal);                       // normal vector              
    v_position   = vec3(modelViewMatrix * vec4(vertexPosition, 1));              // vertex pos in eye coords   
    v_texCoord2d = texCoord2d;

    MVP = projectionMatrix * modelViewMatrix;
    gl_Position = projectionMatrix * modelViewMatrix * vec4(vertexPosition, 1);

    clipDistX = dot(clipPlaneX, modelViewMatrix* vec4(vertexPosition, 1));
    clipDistY = dot(clipPlaneY, modelViewMatrix* vec4(vertexPosition, 1));
    clipDistZ = dot(clipPlaneZ, modelViewMatrix* vec4(vertexPosition, 1));

    gl_ClipDistance[0] = clipDistX;
    gl_ClipDistance[1] = clipDistY;
    gl_ClipDistance[2] = clipDistZ;
}

