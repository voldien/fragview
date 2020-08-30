#extension GL_ARB_enhanced_layouts : enable
#extension GL_ARB_separate_shader_objects : enable
#extension ARB_shading_language_420pack : enable
layout(location = 0) out vec4 fragColor;
layout(location = 1) in vec2 uv;

uniform sampler2D texture0;

void main(void) {
    const vec4 constFactor = vec4(112.0 / 255.0, 66.0 / 255.0, 20.0 / 255.0, 1.0);
	fragColor = texture(texture0, uv ) * constFactor;
}


