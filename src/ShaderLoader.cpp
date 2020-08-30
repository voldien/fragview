#include"ShaderLoader.h"
#include<Utils/ShaderUtil.h>
#include<Renderer/ProgramPipeline.h>
using namespace fragview;
using namespace fragcore;

const char *vertexs[] = {
	NULL,						  /*  None shader language.   */
	//gc_shader_display_vs,		  /*  GLSL.   */
	//(const char *)vertex_display, /*  SPIRV.  */
	NULL,						  /*  CLC.    */
	NULL,						  /*  HLSL.   */
};
const unsigned int vSizes[] = {
	NULL, /*  None shader language.   */
	//gc_shader_display_vs_size,
	NULL,
	NULL,
	NULL,
};
const unsigned int vShaderType[] = {
	eNoShaderType, /*  None shader language.   */
	eSourceCode,   /*  GLSL.   */
	eBinary,	   /*  SPIRV.  */
	eSourceCode,   /*  CLC.    */
	eSourceCode,   /*  HLSL.   */
};

const char *fragments[] = {
	NULL,							/*  None shader language.   */
	//gc_shader_display_fs,			/*  GLSL.   */
	//(const char *)fragment_display, /*  SPIRV.  */
	NULL,							/*  CLC.    */
	NULL,							/*  HLSL.   */
};
const unsigned int fSizes[] = {
	NULL, /*  None shader language.   */
	//gc_shader_display_fs_size,
	NULL,
	NULL,
	NULL,
};
const unsigned int fShaderType[] = {
	eNoShaderType, /*  None shader language.   */
	eSourceCode,   /*  GLSL.   */
	eBinary,	   /*  SPIRV.  */
	eSourceCode,   /*  CLC.    */
	eSourceCode,   /*  HLSL.   */
};

//TODO relocate to fragview.
void ShaderLoader::loadFragmentProgramPipeline(IO *fragIO, ShaderLanguage language, IRenderer *renderer,
											 ProgramPipeline **pshader)
{

	char *fragment;

	unsigned int LangI = 1;
	ShaderUtil::ShaderObject shaderObjectV = {
		//.buf = gc_shader_display_vs,
		.size = vSizes[LangI],
		.language = language,
		.type = vShaderType[LangI],
	};

	/*  Load fragment source file.  */
	long int size = 0;
	//IOUtil::loadStringMem(fragIO, &fragment);
	ShaderUtil::ShaderObject shaderObjectF = {
		.buf = fragment,
		.size = size,
		.language = language,
		.type = eSourceCode,
	};

	ShaderUtil::loadProgramPipeline(&shaderObjectV, &shaderObjectF, NULL, NULL, NULL, renderer, pshader);
	/*  */
	free(fragment);
}

void ShaderLoader::loadDisplayShader(IRenderer *renderer, ProgramPipeline **pProgramPipeline)
{

	ShaderLanguage langauge = renderer->getShaderLanguage();

	/*  Check if renderer is supported. */
	if (langauge & ~(GLSL | CLC | HLSL | SPIRV))
		throw InvalidArgumentException(
			fvformatf("None supported language for the display shader, %s.", renderer->getName()));

	/*  TODO extract only one language. */
	/*  TODO select the first valid language.   */
	ShaderLanguage chosenLanguage = GLSL; //(ShaderLanguage)sntLog2MutExlusive32(langauge);

	/*  TODO get the expoonent as the index.    */
	unsigned int index = chosenLanguage;

	/*  Select shader.  */
	const char *vertex = vertexs[index];
	const char *fragment = fragments[index];

	/*  */
	const uint32_t vSize = vSizes[index];
	const uint32_t fSize = fSizes[index];

	/*  */
	const ShaderCodeType shVType = (ShaderCodeType)vShaderType[index];
	const ShaderCodeType shFType = (ShaderCodeType)fShaderType[index];

	/*  Make sure the shaders are not null. */
	assert(vertex && fragment);
	assert(vSize > 0 && fSize > 0);

	ShaderUtil::ShaderObject shaderObjectF = {
		.buf = fragment,
		.size = fSize,
		.language = chosenLanguage,
		.type = shFType,
	};

	ShaderUtil::ShaderObject shaderObjectV = {
		.buf = vertex,
		.size = vSize,
		.language = chosenLanguage,
		.type = shVType,
	};

	ShaderUtil::loadProgramPipeline(&shaderObjectV, &shaderObjectF, NULL, NULL, NULL, renderer, pProgramPipeline);
}

void ShaderLoader::defaultUniformMap(ProgramPipeline *programPipeline)
{

	/*	Texture location.   */
	/*  TODO relocate the constants to a seperate header.    */
	static const ShaderLoader::DefaultTextureLocation texloc[] = {
		{"texture0", 0},
		{"texture1", 1},
		{"texture2", 2},
		{"texture3", 3},
		{"texture4", 4},
		{"texture5", 5},
		{"texture6", 6},
		{"texture7", 7},
		{"texture8", 8},
		{"texture9", 9},
		{"texture10", 10},
		{"texture11", 11},
		{"texture12", 12},
		{"texture13", 13},
		{"texture14", 14},
		{"texture15", 15},
		{"diffuse0", 0},
		{"world0", 1},
		{"normal0", 2},
		{"depth0", 3},
		{"backbuffer", 10},
		{0, 0}};

	/*  Iterate through each default uniforms.  */
	DefaultTextureLocation *uniloc = (DefaultTextureLocation *)texloc;
	while (uniloc->texname)
	{
		if (programPipeline->getLocation(uniloc->texname) >= 0)
		{
			programPipeline->setInt(programPipeline->getLocation(uniloc->texname), uniloc->loc);
		}
		uniloc++;
	}
}