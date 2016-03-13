/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Embedded resources
 *     toolset: OpenGL Shading Language 3.3
 *     content: Fragment Shader for solid colouring (application/x-glsl)
 *     version: 1.0 OS
 */

#version 330

uniform vec3 hv_Color;

out vec4 ex_Color;

void main(void)
{
	ex_Color = vec4(hv_Color, 1.0);
}

