#ifndef _FRAGVIEW_SHADER_LOADER_
#define _FRAGVIEW_SHADER_LOADER_ 1
#include <FragCore.h>

using namespace fragcore;
namespace fragview {
	/**
	 *
	 */
	class FVDECLSPEC ShaderLoader {
	  public:
		/*	*/
		typedef struct default_texture_location_t {
			const char *texname;  /*  */
			int loc;			  /*  */
		} DefaultTextureLocation; // TODO relocate to to the engine.

		// TODO relocate to the engine.
		static void defaultUniformMap(Shader *Shader);
		// TODO relocate to be part of the zip!
		static void loadDisplayShader(IRenderer *renderer, Shader **pShader);

		// TODO relocate and .
		static void loadFragmentShader(IO *fragIO, ShaderLanguage language, IRenderer *renderer,
												Shader **pshader);
	};
} // namespace fragview

#endif
