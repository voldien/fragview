
const sampler_t sampler = CLK_ADDRESS_CLAMP_TO_EDGE | CLK_FILTER_NEAREST;

#define SIZE 1

__kernel void main(write_only image2d_t fragColor, int w, int h, read_only image2d_t tex0){

	int x = (int)get_global_id(0);
	int y = (int)get_global_id(1);

	uint groupx = get_local_size(0) * SIZE;
	uint groupy = get_local_size(1) * SIZE;

	x *= groupx;
	y *= groupy;

    if (x >= get_image_width(tex0) || y >= get_image_height(tex0)) {
        return;
    }

	float4 p00 = read_imagef(tex0, sampler, (int2)(x - 1, y - 1));
	float4 p10 = read_imagef(tex0, sampler, (int2)(x ,    y - 1));
	float4 p20 = read_imagef(tex0, sampler, (int2)(x + 1, y - 1));

	float4 p01 = read_imagef(tex0, sampler, (int2)(x - 1, y));
	float4 p21 = read_imagef(tex0, sampler, (int2)(x + 1, y));

	float4 p02 = read_imagef(tex0, sampler, (int2)(x - 1, y + 1));
	float4 p12 = read_imagef(tex0, sampler, (int2)(x    , y + 1));
	float4 p22 = read_imagef(tex0, sampler, (int2)(x + 1, y + 1));

	float3 gx = -p00.xyz + p20.xyz + 2.0f * (p21.xyz - p01.xyz) - p02.xyz + p22.xyz;
	float3 gy = -p00.xyz - p20.xyz + 2.0f * (p12.xyz - p10.xyz) + p02.xyz + p22.xyz;

	float3 g = native_sqrt(gx * gx + gy * gy);

	write_imagef(fragColor, (int2)(x,y), (float4)(g.x, g.y, g.z, 1.0f));
}
