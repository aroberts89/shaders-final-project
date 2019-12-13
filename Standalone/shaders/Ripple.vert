#version 330 
#extension GL_ARB_explicit_attrib_location : require 
#extension GL_ARB_explicit_uniform_location : require 

/* Strict Binding for Cross-hardware Compatability */
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec4 tangent;
layout(location = 3) in vec3 textureCoordinate;
layout(location = 4) in vec3 color;

uniform float time;
uniform float amplitude;
uniform float velocity;
uniform float noise;

/* Uniform variables for Camera and Light Direction */ 
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

/* 
 * Light model information interpolated between each vertex. This information is 
 * used to compute the light model within the fragment shader based on the 
 * interpolated vector values. 
 */ 
out vec3 interpSurfaceNormal;
out vec3 interpVertexPosition;
out vec3 interpLightPosition;
out vec2 interpTextureCoord;

/* Light direction in tangent space */
out vec3 lightVector;

out float heightRatio;

/* Reflection and refraction directions to be interpolated across the surface. */
out vec3 interpReflectDir;
out vec3 interpRefractDir;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

/* Displacement Mapping */
void main(void) {
	vec4 vPosition = vec4(position, 1.0f);
	vec4 lPosition = vec4(lightPosition, 1.0f);

	vec3 cameraPosition = vec3(modelViewMatrix * vec4(cameraPosition, 1.0f));

	//---------------------------------------------------------------------------- 
	// ADS Interpolated Light Model Vectors 
	//---------------------------------------------------------------------------- 
	interpLightPosition = vec3(modelViewMatrix * lPosition);
    interpVertexPosition = vec3(modelViewMatrix * vPosition);       
    interpSurfaceNormal = normalize(normalMatrix * normal);
	interpTextureCoord = vec2(textureCoordinate);

	//---------------------------------------------------------------------------- 
	// Determine the direction of the light from this vertex.
	//---------------------------------------------------------------------------- 
	vec3 lightDirection = vec3(interpLightPosition - interpVertexPosition);
	
	//---------------------------------------------------------------------------- 
	// TBN Matrix. The matrix is left as a set of orthogonal vectors. The vector
	// product with the matrix is the same as using three dot products.
	//---------------------------------------------------------------------------- 
	vec3 n = interpSurfaceNormal;
	vec3 t = normalize(normalMatrix * vec3(tangent.xyz));
	vec3 b = cross(n, t) * tangent.w;
	
	//---------------------------------------------------------------------------- 
	// Determine the light direction in tangent space.
	//---------------------------------------------------------------------------- 
	vec3 v;
	v.x = dot(lightDirection, t);
	v.y = dot(lightDirection, b);
	v.z = dot(lightDirection, n);
	lightVector = normalize(v);

	//-------------------------------------------------------------------------- 
	// The reflection and refraction directions are view dependent.
	//-------------------------------------------------------------------------- 
	vec3 view = normalize(cameraPosition - interpVertexPosition);
	
	//-------------------------------------------------------------------------- 
	// Calculate the reflection and refraction directions to be usedas
	// cubemap texture coordinates. The constant 'eta' values is based on the 
	// index of refraction for a particular material (these values can be 
	// looked up using any physics textbook or on the internet).
	//-------------------------------------------------------------------------- 
	interpReflectDir = reflect(-view, interpSurfaceNormal);
	float eta = 1.0 / 1.5f;
	interpRefractDir = refract(-view, interpSurfaceNormal, eta);
	
	// Translate the y coordinate
	// Special cases to handle the 0,0 coord
	float x = (position.x == 0.0f) ? 0.2f : position.x;
	float z = (position.z == 0.0f) ? 0.2f : position.z;

	float r = -sqrt((x * x) + (z * z));
	r = min(r, -0.40f);
	float disp = amplitude * sin(r + time * velocity) / r;


	// Add noise
	disp += noise * rand(vec2(position.x, position.z));

	vec4 dispVector = vec4(normal * disp, 0.0f);
	vec4 dispPosition = vec4(position, 1.0f) + dispVector;

	heightRatio = (disp + amplitude) / (2 * amplitude);
	
	//-------------------------------------------------------------------------- 
	// Transform the vertex for the fragment shader. 
	//-------------------------------------------------------------------------- 
    gl_Position = projectionMatrix * modelViewMatrix * dispPosition;
}