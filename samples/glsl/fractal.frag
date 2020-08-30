#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_enhanced_layouts : enable
#ifdef GL_ARB_explicit_attrib_location
layout(location = 0) out vec4 fragColor;
layout(location = 1) in vec2 uv;
#else
out vec4 fragColor;
in vec2 uv;
#endif

/*  */
uniform float time;
uniform vec2 resolution;
uniform vec2 mouse;

const float PI = 3.14159265358979;
const int samples = 16;

void main(void) {
    vec2 c, z;

    c = z = uv + 4.0 * ((mouse * vec2(1.0, -1.0)) / resolution) - vec2(2, -1.5);

    for (int x = 0; x < samples; x++){
        z = vec2(z.x * z.x - z.y * z.y, 2.0 * z.x * z.y) + (c * cos(time) + 0.5 * sin(time));

        if (dot(z, z) > 8.0f){
            fragColor = vec4(-cos(time * PI));
            break;
        }
    }

    fragColor = vec4(1.0f -2.0f * cos(z.x), 0.0f - 2.0f * cos(z.y * PI), 0.2 * cos(time), 1.0f);
}
