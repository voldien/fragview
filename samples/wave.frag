#extension GL_ARB_enhanced_layouts : enable
#extension GL_ARB_shading_language_420pack : enable
#extension GL_ARB_shader_image_load_store : enable

layout (local_size_x =128, local_size_y = 128) in;

layout( std140, binding=3 ) buffer Vertexes {
    vec3 Positions[ ];

};

void main(void) {

    // Get output fragcolor texture coordinate.
    ivec2 TexCoord = ivec2(gl_GlobalInvocationID.xy);
    ivec2 fragSize = imageSize(fragColor);
    ivec2 TexSize = imageSize(texture0);

    // Compute sample coordinate.
    vec2 TexDiff = vec2(TexSize) / vec2(fragSize);
    ivec2 sampleTexCoord = ivec2(TexDiff * TexCoord);

    vec4 p00 = imageLoad(texture0, sampleTexCoord);

    // Compute sepia effect.
    const vec4 constFactor = vec4(112.0 / 255.0, 66.0 / 255.0, 20.0 / 255.0, 1.0);
    const vec4 pixel = p00 * constFactor;

    imageStore(fragColor, TexCoord, pixel);
}


