//vertex
#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoord;
layout(location = 3) in vec3 normal;

out vec4 vColor;
out vec2 vTexCoord;
out vec3 vSurfaceNormal;
out vec3 vToLightVector;
out vec3 vToCameraVector;

uniform mat4 uTransformationMatrix;
uniform mat4 uProjectionMatrix;
uniform mat4 uViewMatrix;
uniform vec3 uLightPosition = vec3(10.0, 10.0, 10.0);
uniform vec3 uColor = vec3(1,1,1);
uniform int  uTextureRepeatCount = 1;

void main()
{
	vec4 worldPosition = uTransformationMatrix * vec4(position, 1.0f);
	vec4 positionRelativeToCamera = uViewMatrix * worldPosition;
	gl_Position = uProjectionMatrix * positionRelativeToCamera;
	vColor = vec4(uColor, 1.0);

	vTexCoord = texCoord * uTextureRepeatCount;
	vSurfaceNormal = mat3(uTransformationMatrix) * normal;
	vToLightVector = uLightPosition - worldPosition.xyz;
	vToCameraVector = (inverse(uViewMatrix) * vec4(0, 0, 0, 1)).xyz - worldPosition.xyz;
}

//fragment
#version 330 core

in vec4 vColor;
in vec2 vTexCoord;
in vec3 vSurfaceNormal;
in vec3 vToLightVector;
in vec3 vToCameraVector;

layout(location = 0) out vec4 fColor;

uniform sampler2D uTextureSampler;
uniform vec3 uLightColor= vec3(1.0f, 1.0f, 1.0f);
uniform vec3 uFogColor;
uniform float uHasTexture = 1.0;
uniform float uSpecularExponent = 10.0;
uniform float uSpecularStrength = 1;
uniform float uDiffuseStrength = 1;
uniform float uAmbient = 0.3;

void main()
{
	fColor = vColor;
	if (uHasTexture > 0.5)
	{
		fColor *= texture(uTextureSampler, vTexCoord);
		if (fColor.a < 0.1)
			discard;
	}
	vec3 unitNormal = normalize(vSurfaceNormal);
	vec3 unitLightVector = normalize(vToLightVector);
	vec3 unitVectorToCamera = normalize(vToCameraVector);
	vec3 lightDirection = -unitLightVector;

	float diffuse = uDiffuseStrength * max(dot(unitNormal, unitLightVector), 0.0);
	float specular = uSpecularStrength * pow(max(dot(reflect(lightDirection, unitNormal), unitVectorToCamera), 0.0f), uSpecularExponent);

	fColor *= vec4((uAmbient + (diffuse) * uLightColor), 1.0f);
}
