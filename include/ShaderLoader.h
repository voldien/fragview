#ifndef _FRAGVIEW_SHADER_LOADER_
#define _FRAGVIEW_SHADER_LOADER_ 1
#include<FragCore.h>

using namespace fragcore;
namespace fragview
{
	/**
	 * 
	 */
	class FVDECLSPEC ShaderLoader{
		public:
		/*	*/
		typedef struct default_texture_location_t
		{
			const char *texname;  /*  */
			int loc;			  /*  */
		} DefaultTextureLocation; //TODO relocate to to the engine.

		//TODO relocate to the engine.
		static void defaultUniformMap(ProgramPipeline *programPipeline);
		//TODO relocate to be part of the zip!
		static void loadDisplayShader(IRenderer *renderer, ProgramPipeline **pProgramPipeline);

		// TODO relocate and .
		static void
		loadFragmentProgramPipeline(IO *fragIO, ShaderLanguage language, IRenderer *renderer,
									ProgramPipeline **pshader);
	};
}

#endif