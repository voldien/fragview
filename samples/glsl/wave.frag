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

uniform vec2 resolution;
uniform float time;
uniform vec2 mouse;

void main(void) {

    float mx = max(resolution.x, resolution.y);
    vec2 uv = (gl_FragCoord.xy + mouse) / mx;
    vec3 color = vec3(uv, 0.25 + 0.5 * sin(time));

    vec3 hole = vec3(sin(1.5 - distance(uv, mouse.xy /mx)));
    const vec4 destColor = vec4(hole * color, 1.0);
    #if defined(GL_ARB_explicit_attrib_location)
    fragColor = destColor;
    #else
    gl_fragColor = destColor;
    #endif
}
