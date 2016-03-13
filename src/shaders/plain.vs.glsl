/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Embedded resources
 *     toolset: OpenGL Shading Language 3.3
 *     content: Vertex Shader for basic rotation (application/x-glsl)
 *     version: 1.0 OS
 */

#version 330

layout(location=0) in vec3 ms_Position;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

void main(void)
{
	gl_Position = ProjectionMatrix * ViewMatrix * ModelMatrix * vec4(ms_Position, 1.0);
}

