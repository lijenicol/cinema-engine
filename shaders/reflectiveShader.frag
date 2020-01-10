#version 330 core

in vec3 FragNormal;
in vec3 FragVert;

uniform vec3 camPos;
uniform samplerCube skybox;

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 FragColor;

void main()
{
	vec3 I = normalize(FragVert - camPos);
    vec3 R = reflect(I, normalize(FragNormal));
	vec4 Rcolor = vec4(texture(skybox, R).rgb, 1.0);
	
	float ratio = 1.00/1.52;
	vec3 Re = refract(I, normalize(FragNormal), ratio);
	vec4 Recolor = vec4(texture(skybox, Re).rgb, 1.0);
    //FragColor = 0.7 * Recolor + 0.3 * Rcolor;
    FragColor = 1.0 * Rcolor;
}