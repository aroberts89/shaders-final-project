#version 330 
#extension GL_ARB_explicit_attrib_location : require 
#extension GL_ARB_explicit_uniform_location : require 

/* Cubemap sampler (6) images composed into a 3D texture */
uniform samplerCube cubeMap;

/*
 * The interpolated refelction and refraction directions provide two different
 * 3D texture coordinates: one to look up the reflected color value for this
 * fragment and the other provides the refracted color value for this fragment.
 * These values are then both combined to provide the illusion of a transparent
 * object surface.
 */
in vec3 interpReflectDir;
in vec3 interpRefractDir;

/* Refraction Shader */
void main(void) {

	//--------------------------------------------------------------------------
	// Look up both the reflection and refraction color values based on the 
	// same cubemap. The resulting fragment color will be a combination of these
	// color values based on a simple ratio.
	//--------------------------------------------------------------------------
	vec4 reflectColor = texture(cubeMap, interpReflectDir);
	vec4 refractColor = texture(cubeMap, interpRefractDir);

	//--------------------------------------------------------------------------
	// The final result is defined by the relationship between the reflection
	// and refraction ratio (how much of the cubemap is reflected and how much
	// is refracted).
	//--------------------------------------------------------------------------
	float reflectFrac = 0.9f;
	vec4 c = vec4(0.0f);
	c += reflectFrac * refractColor;
	c += (1.0f - reflectFrac) * reflectColor;
	
	c.a = 0.8f;
	gl_FragColor = c;
}
