/**
    Copyright (C) 2019  Valdemar Lindberg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_explicit_attrib_location : enable
#extension GL_ARB_enhanced_layouts : enable
#extension GL_ARB_shader_clock : enable
#extension GL_ARB_gpu_shader_int64 : enable

#if defined(GL_ARB_explicit_attrib_location)
layout(location = 0) in vec3 vertex;
#else
attribute vec3 vertex;
#endif

//#if __VERSION__ > 120
//layout(location = 1) out vec2 uv;
//#else
//varying vec2 uv;
//#endif

layout(location = 1) out vec2 uv;
layout(location = 2) out float time;
uniform sampler2D tex0;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main(void) {
	gl_Position = vec4(vertex,1.0);
	uv = (vertex.xy + vec2(1.0)) / 2.0;
	time = float(clock2x32ARB().x / 1000000000);
}


