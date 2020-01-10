#version 330 core

in vec2 TexCoords;
out vec4 fragColor;

uniform sampler2D screenTexture;
uniform sampler2D bloomBlur;
uniform sampler2D velocityTexture;

uniform float exposure;
uniform bool bloom;
uniform bool blur;

void main()
{
	const float gamma = 2.2;
	
	//get color value from texture
	vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
	if(bloom) {
		vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
		hdrColor += 0.4 * bloomColor;
	}
	
	//calculate velocities for motion blur
	//get current viewport position from gbuffer and view projection matrices
	//get last viewport position the same way, but using previous view matrix
	//calculate the velocity
	vec2 texelSize = 1.0 / vec2(textureSize(screenTexture, 0));
	//vec2 tex = vec2(TexCoords * texelSize);
	vec2 tex = vec2(TexCoords);
	//vec3 velocity = texture(velocityTexture, TexCoords).rgb;
	vec3 velocity = texture(velocityTexture, tex).rgb;
		
	//apply the motion blur using the calculated velocities
	if(blur){
		int sampleCount = 5;
		for(int i = 1; i < sampleCount; ++i){
			vec2 offset = velocity.xy * (float(i)/float(sampleCount - 1) - 0.5);
			vec3 currentColor = texture(screenTexture, tex - offset).rgb;
			if(bloom) {
				vec3 bloomColor = texture(bloomBlur, tex - offset).rgb;
				currentColor += 0.4 * bloomColor;
			}
			hdrColor += currentColor;
			//tex -= vec2(velocity);
		}
		hdrColor /= sampleCount;
	}
	
	//apply hdr tone mapping and gamma correct
	vec3 result = vec3(1.0) - exp(-hdrColor * exposure);
	result = pow(result, vec3(1.0 / gamma));
	
	fragColor = vec4(result, 1.0);
	//fragColor = vec4(hdrColor, 1.0);
}