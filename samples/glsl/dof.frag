#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shader_precision : enable

precision highp float;
layout(location = 0) out vec4 fragColor;

uniform sampler2D texture0;	/*	diffuse texture	*/
uniform sampler2D texture1; 	/*	depth texture.	*/

uniform
uniform float time;
uniform vec2 resolution;


#define e 2.7182818284590
#define pi 3.14159265358979

#define RADIUS 0.5

float getGuas2D(float x, float y, float theta){
	return (1.0 / (2 * pi * theta * theta) ) * pow(e, -0.5 *  ( ( x * x) + (y * y) ) / (theta * theta) ); 
}


float getExpLinear(float near, float far, float exp){
	return ((2.0f * near) / (far + near  - exp * (far - near)));
}

vec4 blur(float radius, float inDepth){
	int x;
	int y;
	const float offsetdivx = 80.0;
	const float offsetdivy = 80.0;
	vec4 color1 = vec4(0.0);
	vec2 offset = vec2(resolution.x / offsetdivx, resolution.y / offsetdivy) * radius;
	int num = 5;
	int start = -((num - 1) / 2);
	int end = ((num - 1) / 2);
	float theta = 1.3;
	float total = 0;
	float guas;
	const float near = 0.15;
	const float far = 1000.0;
	float depth = getExpLinear(near, far, inDepth);

	for(x = start; x <= end; x++){
		for(y = start; y <= end; y++){
			vec2 uv = (gl_FragCoord.xy + vec2(x,y) * offset ) / resolution;
			guas = getGuas2D(x , y , theta) * depth;
			color1 += texture(texture0, uv) * guas;
			total += guas;
		}
	}

	return color1 / total;
}


vec4 dof(void){
	vec2 uv = (gl_FragCoord.xy / resolution);
	float inDepth = texture(texture1, uv).r;
	vec4 colorblur = blur(RADIUS, 1.0);
	return colorblur;
}

void main(){
	fragColor = dof();
}
