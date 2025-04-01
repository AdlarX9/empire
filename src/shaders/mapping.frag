void main() {
	gl_FragColor = vec4(cos(gl_FragCoord.x / 50.), 0., sin(gl_FragCoord.y / 10.), 1.);
}