#version 330 
#extension GL_ARB_explicit_attrib_location : require 
#extension GL_ARB_explicit_uniform_location : require 

/* Uniform variables for Camera and Light Direction */ 
in vec3 interpSurfaceNormal;
in vec3 interpVertexPosition;
in vec3 interpLightPosition;
in vec2 interpTextureCoord;

in float heightRatio;

/* Displacement Mapping */
void main(void) {
	//-------------------------------------------------------------------------- 
	// Wireframe.
	//-------------------------------------------------------------------------- 
	const float scale = 64.0f;
	bvec2 toDiscard = greaterThan(fract(interpTextureCoord * scale), vec2(0.1f, 0.1f));
	if ( all(toDiscard) ) discard;
		
	//-------------------------------------------------------------------------- 
	// Light, camera, and reflection direction calculations.
	//-------------------------------------------------------------------------- 
	vec3 l = normalize(interpLightPosition - interpVertexPosition);
	vec3 c = normalize(-interpVertexPosition);
	vec3 r = normalize(-reflect(l, interpSurfaceNormal));
	
	//-------------------------------------------------------------------------- 
	// Light and material properties. 
	//-------------------------------------------------------------------------- 
	vec4 Ia = vec4(0.6f, 0.6f, 0.8f, 1.0f); 
	vec4 Id = vec4(0.9f, 0.9f, 0.9f, 1.0f); 
	vec4 Is = vec4(1.0f, 1.0f, 1.0f, 1.0f);
	
	//--------------------------------------------------------------------------
	// Material Properties
	//--------------------------------------------------------------------------
	vec4 Ka = vec4(0.1f, 0.1f, 0.1f, 1.0f);
	vec4 Kd = vec4(0.2f, 0.6f, 0.8f, 1.0f);
	Kd.x = heightRatio;
	Kd.z = 1 - heightRatio;
	Kd.y = 2 * (0.5f - abs(heightRatio - 0.5f));
	vec4 Ks = vec4(0.6f, 0.7f, 1.0f, 1.0f);
	float shininess = 16.0f;
	
	vec4 Iambient = vec4(0.0f);
	vec4 Idiffuse = vec4(0.0f);
	vec4 Ispecular = vec4(0.0f);
        
	//-------------------------------------------------------------------------- 
	// Assign the vertex color as the ambient color. 
	//--------------------------------------------------------------------------
	Iambient = Ia * Ka;
 
	//-------------------------------------------------------------------------- 
	// Calculate the diffuse component based on the surface normal and the light 
	// direction and add it to the vertex color. 
	//-------------------------------------------------------------------------- 
	float lambertComponent = max(0.0f, dot(interpSurfaceNormal, l)); 
	Idiffuse = (Id * Kd) * lambertComponent;
	
	//-------------------------------------------------------------------------- 
	// Calculate the final ADS light value for this vertex. 
	//--------------------------------------------------------------------------
	gl_FragColor = Iambient + Idiffuse;  
}