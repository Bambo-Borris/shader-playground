// uniform vec2 u_resolution; 
// uniform vec2 u_mouse;
// uniform float u_elapsedTime;
// uniform float u_deltaTime;
// uniform int u_frames;
// uniform sampler2D u_texture0;

// Textures the quad
void main() { 
	vec2 st = gl_FragCoord.xy / u_resolution.xy;
	st.y = 1.0 - st.y;
	gl_FragColor = texture2D(u_texture0, st);//vec4(vec3(rnd), 1.0);
}