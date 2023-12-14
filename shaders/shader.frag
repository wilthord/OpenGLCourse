#version 330

in vec4 vCol;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

out vec4 colour;

const int MAX_POINT_LIGHTS = 3;

struct Light
{
	vec3 colour;
	float ambientIntensity;
	float diffuseIntensity;
};

struct DirectionalLight {
	Light base;
	vec3 direction;
};

struct PointLight {
	Light base;
	vec3 position;
	float constant;
	float linear;
	float exponent;
};

struct Material {
	float specularIntensity;
	float shininess;
};

uniform int pointLightCount;

uniform DirectionalLight directionalLight;
uniform PointLight pointLights[MAX_POINT_LIGHTS];

uniform sampler2D theTexture;
uniform Material material;

uniform vec3 eyePosition;

vec4 calcLightByDirection(Light light, vec3 direction) {
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;
	
	float diffuseFactor = max(dot(normalize(normal), normalize(direction)), 0.0f);
	vec4 diffuseColour = vec4(light.colour, 1.0f) * light.diffuseIntensity * diffuseFactor;
	
	vec4 specularColour = vec4(0, 0, 0, 0);
	
	if(diffuseFactor > 0.0f) {
		vec3 fragToEye = normalize(eyePosition - fragPos);
		vec3 reflectedVertex = normalize(reflect(direction, normalize(normal)));
		
		float specularFactor = dot(fragToEye, reflectedVertex);
		if(specularFactor > 0.0f) {
			specularFactor = pow(specularFactor, material.shininess);
			specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);
		}
	}
	
    return (ambientColour + diffuseColour + specularColour);
}

vec4 calcDirectionalLight(){
	return calcLightByDirection(directionalLight.base, directionalLight.direction);
}

vec4 calcPointLights(){
	vec4 totalColour = vec4(0, 0, 0, 0);
	for(int i = 0; i < pointLightCount; i++){
		vec3 direction = fragPos - pointLights[i].position;
		float distance = length(direction);
		direction = normalize(direction);
		
		//We calculate the point light the same way as the directional light
		// The difference here is that the direction is calculated based on 
		//the vector formed by the {fragmentPos} and the {PointLightPos}
		vec4 colour1 = calcLightByDirection(pointLights[i].base, direction);
		
		float attenuation = pointLights[i].exponent * distance * distance +
							pointLights[i].linear * distance +
							pointLights[i].constant;
							
		totalColour += (colour1 / attenuation);
	}
	
	return totalColour;
}

void main() {
	vec4 finalColour = calcDirectionalLight();
	finalColour += calcPointLights();
    colour = texture(theTexture, texCoord) * finalColour;
}