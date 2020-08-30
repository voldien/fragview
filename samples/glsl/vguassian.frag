#extension ARB_shader_texture_lod : enable
layout(location = 0) out vec4 fragColor;
in vec2 uv;

uniform sampler2D texture0;
uniform vec2 resolution;


#define e 2.7182818284590
#define pi 3.14159265358979
#define RADIUS 20.1
#define THETA 5.7

float getGuas(float x, float theta){
	return (1.0 / ( theta * sqrt(2.0 * pi) ) ) * pow(e, -0.5 * ((x * x) / (theta * theta))  );
}

vec4 blur(float radius, float theta){
	int y;
	const float offsetdivx = resolution.x;
	const float offsetdivy = resolution.y;
	const  vec2 offset = vec2(resolution.x / offsetdivx, resolution.y / offsetdivy);	/*	offset in order to maintain the blur for any given resolution.	*/
	vec4 color1 = vec4(0.0);
	const int num = 2 * int(ceil(radius)) + 1;	/*	needs to be odd.	*/
	const int start = -((num - 1) / 2);
	const int end = ((num - 1) / 2);
	float total = 0.0;

	for(y = start; y <= end; y++){
		const vec2 uv = (gl_FragCoord.xy + vec2(0,y) * offset ) / resolution;
		const float guas = getGuas(y, theta);
		color1 += texture(texture0, uv) * vec4(vec3(guas), 1.0);
		total += guas;
	}
	return color1 / total;
}

void main(void){
	//vec2 uv = gl_FragCoord.xy / resolution;
	//vec4 tex = texture2D(texture0, uv);
	//fragColor = tex.bgra;
	fragColor = blur(RADIUS, THETA).rgba;
	fragColor.a = texture(texture0, uv).a;
}
