#version 120

uniform vec3 lightPos;
uniform vec3 ka;
uniform vec3 kd;
uniform vec3 ks;
uniform float s;
uniform float i1;
uniform vec3 CamPos;
uniform sampler2D texture;

varying vec3 normal;
varying vec3 vertexPos;
varying vec2 vTex0;

void main()
{
	vec3 color;
	vec3 grass = texture2D(texture, vTex0).rgb;

    // need to normalize since barycentric interpolation makes interpolated vectors at the fragments shorter
	// than their values at the vertices
	vec3 n = normalize(normal);

	// computes the light vector given the light position(in camera space) and interpolated fragment position
	vec3 lightVec = normalize(lightPos - vertexPos);

	// computes diffuse component
	vec3 Diff = kd * max(0.0, dot(n,lightVec));

	// computes eye vector
	vec3 eyeVec = normalize(-vertexPos);

	// computes half vector (Blinn variation of Phong Shading)
	vec3 halfVec = normalize(lightVec + eyeVec);

	// computes specular component
	vec3 Spec = ks * pow(max(0.0, dot(halfVec,n)), s);

	// color = ambient + diffuse + specular
	color = i1 * (ka + Diff + Spec);
	//color = color + grass;
	
	gl_FragColor = vec4(color, 1.0);
}
