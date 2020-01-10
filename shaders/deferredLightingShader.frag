#version 330 core

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;
uniform sampler2D gGlow;
uniform sampler2D gSky;

struct Light {
	vec3 Position;
	vec3 Color;
	float attenuation;
	int directional;
	vec3 direction;
};
const int NR_LIGHTS = 102;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform bool fog;

void main()
{
    //get all data from the gbuffer
	vec3 FragPos = texture(gPosition, TexCoords).rgb;
	vec3 Normal = texture(gNormal, TexCoords).rgb;
	vec3 Albedo = texture(gAlbedoSpec, TexCoords).rgb;
	float Specular = texture(gAlbedoSpec, TexCoords).a;
	
	// then calculate lighting as usual
    vec3 lighting = Albedo * 0.1; // hard-coded ambient component
    vec3 viewDir = normalize(viewPos - FragPos);

    // diffuse
	for(int i = 0; i < NR_LIGHTS; i++){
		//point light calculation
		if(lights[i].directional == 0) {
			vec3 lightDir = normalize(lights[i].Position - FragPos);
			vec3 lC = lights[i].Color / (pow(length(lights[i].Position - FragPos), 2) * lights[i].attenuation);
			vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Albedo * lights[i].Color;
			lighting += diffuse * lC;
		}
		
		//directional light calculation
		else if(lights[i].directional == 1){
			vec3 diffuse = max(dot(Normal, lights[i].direction), 0.0) * Albedo * lights[i].Color;
			lighting += diffuse;
		}
	}
    
    vec3 glowColor = texture(gGlow, TexCoords).rgb;
	vec3 skyColor = texture(gSky, TexCoords).rgb;
	FragColor = vec4(lighting + skyColor, 1.0);
	if(fog) {
		FragColor = mix(vec4(0.08,0.08,0.08,1.0), FragColor, Specular);
	}
	//FragColor = vec4(texture(gNormal, TexCoords).rgb,1.0);
	
	float brightness = dot(FragColor.rgb + glowColor, vec3(0.2126, 0.7152, 0.0722));
	//float brightness = glowColor;
    if(brightness > 1.0)
        BrightColor = vec4(FragColor.rgb + Albedo * glowColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}