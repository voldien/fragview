layout(location = 0) out vec4 fragColor;
in vec2 uv;

uniform sampler2D texture0;    /*	diffuse texture	*/
uniform vec2 resolution;

#define e 2.7182818284590
#define PI 3.14159265358979

#define RADIUS 10.0
#define THETA 1.2
#define INTENSITY 1.4


float getGuas2D(float x, float y, float theta){
    return (1.0 / (2 * PI * theta * theta)) * pow(e, -0.5 *  ((x * x) + (y * y)) / (theta * theta));
}

vec4 blur(float radius, float theta){
    int x, y;
    const float offsetdivx = resolution.x;
    const float offsetdivy = resolution.y;
    vec2 offset = vec2(resolution.x / offsetdivx, resolution.y / offsetdivy);    /*	offset in order to maintain the blur for any given resolution.	*/
    vec4 color1 = vec4(0.0);
    int num = 2 * int(ceil(radius)) + 1;    /*	needs to be odd.	*/
    int start = -((num - 1) / 2);
    int end = ((num - 1) / 2);
    float total = 0;

    for (x = start; x <= end; x++){
        for (y = start; y <= end; y++){
            const  vec2 uv = (gl_FragCoord.xy + vec2(x, y) * offset) / resolution;
            const float guas = getGuas2D(x, y, theta    );
            color1 += texture(texture0, uv).rgba * vec4(vec3(guas), 1.0);
            total += guas;
        }
    }

    // Normalize color.
    return color1 / total;
}


vec4 glow(void){
    vec4 src = blur(RADIUS, THETA) * INTENSITY;
    vec2 uv = (gl_FragCoord.xy) / resolution;
    vec4 dst = texture(texture0, uv);
    return (src + dst) - (src * dst);
}

void main(void){
    fragColor = glow().rgba;
}
