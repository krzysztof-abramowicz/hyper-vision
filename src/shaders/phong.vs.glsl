/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Embedded resources
 *     toolset: OpenGL Shading Language 3.3
 *     content: Vertex Shader with rotation and flat Normal output (application/x-glsl)
 *     version: 1.0 OS
 */

#version 330

layout(location=0) in vec3 ms_Position;
layout(location=1) in vec3 ms_Normal;

uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;

smooth out vec3 es_Position;
flat   out vec3 es_Normal;

void main(void)
{
	es_Position = vec3(ViewMatrix * ModelMatrix * vec4(ms_Position, 1.0));
	es_Normal   = vec3(ViewMatrix * ModelMatrix * vec4(ms_Normal, 0.0));

	gl_Position = ProjectionMatrix * vec4(es_Position, 1.0);
}

