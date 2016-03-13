/* Krzysztof Abramowicz (http://krzysztofabramowicz.com/, k.j.abramowicz@gmail.com)
 *     project: HyperVision — Architectonic Rendering Engine
 *               └─Development stage one (DS1)
 *                  └─Embedded resources
 *     toolset: OpenGL Shading Language 3.3
 *     content: Fragment Shader with pixel-level Phong lighting (application/x-glsl)
 *     version: 1.0 OS
 */

#version 330

smooth in vec3 es_Position;
flat   in vec3 es_Normal;

uniform vec3 hv_Color;
uniform mat4 ViewMatrix;

out vec4 ex_Color;

/* Virtual light source */
vec3 Ls = vec3(1.0, 1.0, 1.0); // specular
vec3 Ld = vec3(0.7, 0.7, 0.7); // diffuse
vec3 La = vec3(0.2, 0.2, 0.2); // ambient
vec3 ws_LightPosition = vec3(10.0, 10.0, 10.0);

/* Material reflectance */
vec3 Ks = vec3(1.0, 1.0, 1.0); // specular
vec3 Kd = hv_Color;            // diffuse
vec3 Ka = vec3(1.0, 1.0, 1.0); // ambient
float Ks_exp = 100.0;

void main(void)
{
	// ambient intensity
	vec3 Ia = La * Ka;

	// diffuse intensity
	vec3 es_LightPosition = vec3(ViewMatrix * vec4(ws_LightPosition	, 1.0));
	vec3 es_LightDirection = normalize(es_LightPosition - es_Position);
	float Fd = max(dot(es_LightDirection, es_Normal), 0.0);
	vec3 Id = Ld * Kd * Fd;

	// specular intensity
	vec3 es_LightReflection = reflect(-es_LightDirection, es_Normal);
	vec3 es_CameraDirection = normalize(-es_Position);
	float Fs = pow(max(dot(es_LightReflection, es_CameraDirection), 0.0), Ks_exp);
	vec3 Is = Ls * Ks * Fs;
	
	// final colour
	ex_Color = vec4 (Is + Id + Ia, 1.0);
}

