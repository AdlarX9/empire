uniform vec3  cameraPosition;
uniform vec4  cameraRotation;
uniform vec4  cameraRotationConjugate;
uniform float cameraFov;
uniform int   screenWidth;
uniform int   screenHeight;

vec4 quaternionMultiply(vec4 q1, vec4 q2) {
	return vec4(                                                // multiplication de quaternions
	    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,  // x
	    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,  // y
	    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w,  // z
	    q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z   // w
	);
}

void main() {
	vec4 position = vec4(gl_Vertex.xy, gl_MultiTexCoord0.x, 1.);
	position.xyz *= 1000.;
	position.xyz -= cameraPosition;
	position = quaternionMultiply(quaternionMultiply(cameraRotationConjugate, position), cameraRotation);
	// position.xy *= position.z; // Applique la perspective
	position.z = 1.;
	position.w = 1.;
    position.xy += vec2(screenWidth, screenHeight) / 2.;

	gl_Position = gl_ModelViewProjectionMatrix * position;
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
	gl_FrontColor = gl_Color;
}
