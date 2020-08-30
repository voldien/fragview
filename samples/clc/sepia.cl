
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

	float4 p00 = read_imagef(tex0, sampler, (int2)(x , y ));
	const float4 constFactor = (float4)(112.0f / 255.0f, 66.0f / 255.0f, 20.0f / 255.0f, 1.0f);
	float4 sepia = p00 * constFactor;

	write_imagef(fragColor, (int2)(x,y), (float4)(sepia.x, sepia.y, sepia.z, 1.0f));
}
