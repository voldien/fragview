#ifndef _FRAGVIEW_SANDBOX_H_
#define _FRAGVIEW_SANDBOX_H_ 1
#include<Renderer/Prerequisites.h>
#include<Scene/SandBoxSubScene.h>
#include"Node.h"

namespace fragview {
	/**
	 *
	 */
	class FVDECLSPEC Sandbox : public Node {   //TODO add sandbox as a component rendering object.
	public:
		Sandbox(void);
	private:

		//TODO add reference instead.
		std::vector<Texture *> textures;
		std::map<unsigned int, UniformLocation> cachedUniforms;

		std::map<ProgramPipeline::ShaderType, std::vector<ProgramPipeline*>> sequences;

		/*  Compute shader attributes.  */
		std::vector<Texture::MapTarget> computeMapTargets;
		std::vector<Texture::Format> computeFormats;
		Ref<Buffer> indirectBuffer;
		std::vector<int[3]> computeLocal;               /*  Cache all the.  */
		/*  */
		FragGraphicUniform uniforms;

		// Main Rendering pipeline information.
		//Queue<Composition> compositions;

	protected:  /*  */
		void updateUniformLocations(ProgramPipeline* programPipeline, UniformLocation* location, ProgramPipeline::ShaderType shaderStage);
	public:

		/*  Add elements.   */
		void addTexture(Texture* texture);
		void addShader(ProgramPipeline* shader);
		void addCompute(ProgramPipeline* compute);

		void setDisptchBuffer(Ref<Buffer> dispatchBuffer);
		const Ref<Buffer>& getDispatchBuffer(void)const;

		/*  Get elements.   */
		Texture* getTexture(int index);
		ProgramPipeline* getShader(int index);
		ProgramPipeline *getCompute(int index);
		const std::vector<Texture *>& getTextures(void) const;

		/*  Get element count. */
		int getNumTextures(void) const;
		int getNumShaders(void) const;
		int getNumCompute(void) const;

		/*  Get compute bind attributes.    */
		const std::vector<Texture::MapTarget>& getComputeMapTargets(void)const;
		const std::vector<Texture::Format>& getComputeFormats(void)const;
		const int* getComputeLocalWorkGroup(int index);

		/*  */
		const UniformLocation* getUniformLocation(int uid) const;
		UniformLocation* getUniformLocation(int uid);
		FragGraphicUniform* getFragUniform(void);

		/**
		 * Update all shader cached uniforms.
		 */
		void updateAllUniformLocations(void);
	};
}

#endif