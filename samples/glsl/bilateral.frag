layout(location = 0) out vec4 fragColor;

uniform sampler2D texture0;
uniform vec2 resolution;
uniform float time;

#define e 2.7182818284590
#define pi 3.14159265358979

#define RADIUS 0.5

vec4 bilateral(float a, float b, vec2 uv){
	vec4 color;

	return color;
}


void main(void){
	vec2 uv = (gl_FragCoord.xy / resolution);
	

	fragColor = vec4(1.0);
}
