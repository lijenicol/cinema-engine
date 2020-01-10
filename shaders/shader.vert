#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 prevView;
uniform mat4 prevModel;

out vec3 FragNormal;
out vec3 FragVert;
out vec2 TexCoords;
out float visibility;
out vec4 currentPos;
out vec4 prevPos;

void main()
{
	//gBuffer work
	TexCoords = texCoords;
	FragNormal = mat3(transpose(inverse(model))) * normal;
	FragVert = vec3(model * vec4(position, 1.0));
    gl_Position = projection * view * model * vec4(position, 1.0);
	
	//fog calculations
	vec4 positionRelativeToCam = view * model * vec4(position, 1.0);
	float distance = length(positionRelativeToCam.xyz);
	visibility = exp(-pow(distance*0.05,1.5));
	visibility = clamp(visibility, 0.0, 1.0);
	
	//for velocity buffer
	currentPos = projection * view * model * vec4(position, 1.0);
	prevPos = projection * prevView * prevModel * vec4(position, 1.0);
}