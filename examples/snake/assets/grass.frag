#version 330 core

uniform float iMaxTipDeviation;
uniform float iTime;
uniform vec4 iTip;

out vec4 FragColor;

#define BLADES 210

vec4 grass(vec2 p, float x) {
	float s = mix(0.7, 2.0, 0.5 + sin(x * 12.0) * 0.5);
	p.x += pow(1.0 + p.y, 2.0) * 0.1 * cos(x * 0.5 + iTime); // sway
	p.x *= s;
	p.y = (1.0 + p.y) * s - 1.0;
	float m = 1.0 - smoothstep(0.0, clamp(1.0 - p.y * 1.5, 0.01, 0.6) * 0.2 * s, pow(abs(p.x) * 9.0, 1.5) + p.y - 0.6);
	// brighter green at the tips, browner at the bottom
    vec3 base = mix(vec3(0.1, 0.2, 0.0), vec3(0.25, 0.25, 0.15), cos(x * 29.0));
    vec3 tip = mix(iTip.xyz, vec3(0.0, 0.35, 0.0), min(iMaxTipDeviation, (cos(x * 17.0)+1.0)/4.0));
    return vec4(mix(base, tip, (p.y + 1.0) * 0.5 + abs(p.x)), m * smoothstep(-1.0, -0.9, p.y));
}

vec3 backg(vec3 ro, vec3 rd) {
	float t = ro.y / rd.y;
	vec2 tc = ro.xz + rd.xz * t;
	vec3 horiz = vec3(0.0, 0.2, 0.2);
	vec3 sky = mix(horiz, vec3(0.1, 0.13, 0.25), max(0.0, rd.y));
    // fog effect, blending the ground into the horizon
	vec3 ground = mix(horiz, vec3(0.2, 0.28, 0.0) * 0.6, pow(max(0.0, -rd.y), 0.2));
	return mix(ground, sky, step(0.0, t));
}

void main() {
	vec3 ct = vec3(0.0, 1.0, 1.0);
	vec3 cp = vec3(0.0, 2.5, 0.0);
	vec3 cw = normalize(ct - cp);
	vec3 cu = normalize(cross(cw, vec3(0.0, 1.0, 0.0)));
	vec3 cv = normalize(cross(cu, cw));
	
	mat3 rm = mat3(cu, cv, cw);
	vec2 iResolution = vec2(800, 800); // invalidated on resize/reshape
	vec2 uv = (gl_FragCoord.xy / iResolution.xy) * 2.0 - vec2(1.0);
	vec2 t = uv;
	t.x *= iResolution.x / iResolution.y;

	vec3 ro = cp, rd = rm * vec3(t, 1.0);
	vec3 fcol = backg(ro, rd);

	for(int i = 0; i < BLADES; i += 1) {
        // Start rendering at the back and move forward
		float t = (float(BLADES - i) * 0.05);
		vec2 tc = ro.xy + rd.xy * t;
		
        // "Randomize" horizontal grass position
        tc.x += cos(float(i));
		float cell = floor(tc.x);
        // tc.x in [-0.5, 0.5), grass periodically repeats now
		tc.x = (tc.x - cell) - 0.5;
		
		vec4 c = grass(tc, float(i) + cell * 10.0);
		fcol = mix(fcol, c.rgb, c.w);
	}
	FragColor.rgb = fcol * 1.8;
	FragColor.a = 1.0;
}
