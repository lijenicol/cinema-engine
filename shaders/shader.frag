#version 330 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;
layout (location = 3) out vec3 gGlow;
layout (location = 4) out vec3 gVelocity;

in vec3 FragNormal;
in vec3 FragVert;
in vec2 TexCoords;
in float visibility;
in vec4 currentPos;
in vec4 prevPos;

uniform sampler2D backgroundTexture;
uniform sampler2D texR;
uniform sampler2D texG;
uniform sampler2D texB;
uniform sampler2D blendMap;
uniform float deltaTime;

uniform float glowIntensity;

void main()
{
	//output position
    gPosition = FragVert;
	
	//output normal
	gNormal = normalize(FragNormal);
	
	//output diffuse colour from texture
	vec4 blendMapColour = texture(blendMap, TexCoords);
	float backTextureAmount = 1 - (blendMapColour.r + blendMapColour.g + blendMapColour.b);
	vec2 tiledCoords = TexCoords * 120.0;
	vec4 backgroundTextureColour = texture(backgroundTexture, tiledCoords) * backTextureAmount;
	vec4 rTextureColour = texture(texR, tiledCoords) * blendMapColour.r;
	vec4 gTextureColour = texture(texG, tiledCoords) * blendMapColour.g;
	vec4 bTextureColour = texture(texB, tiledCoords) * blendMapColour.b;
	vec4 totalColour = backgroundTextureColour + rTextureColour + gTextureColour + bTextureColour;
	//gAlbedoSpec.rgb = texture(texR, tiledCoords).rgb;
	gAlbedoSpec.rgb = totalColour.rgb;
	
	//output specular component
	gAlbedoSpec.a = visibility;
	
	gGlow = vec3(glowIntensity,glowIntensity,glowIntensity);
	
	vec3 a = vec3((currentPos.xy / currentPos.w) * 0.5 + 0.5, 0.0);
	vec3 b = vec3((prevPos.xy / prevPos.w) * 0.5 + 0.5, 0.0);
	gVelocity = (a - b) / 2.0;
	gVelocity *= (1/deltaTime)/30.0;
}