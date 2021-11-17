#ifndef _FRAGVIEW_SANDBOX_H_
#define _FRAGVIEW_SANDBOX_H_ 1
#include "Node.h"
#include <Renderer/Prerequisites.h>
#include <Scene/SandBoxSubScene.h>

namespace fragview {
	/**
	 *
	 */
	class FVDECLSPEC Sandbox : public Node { // TODO add sandbox as a component rendering object.
	  public:
		Sandbox();

	  private:
		// TODO add reference instead.
		std::vector<Texture *> textures;
		std::map<unsigned int, UniformLocation> cachedUniforms;

		std::map<Shader::ShaderType, std::vector<Shader *>> sequences;

		/*  Compute shader attributes.  */
		std::vector<Texture::MapTarget> computeMapTargets;
		std::vector<Texture::Format> computeFormats;
		Ref<Buffer> indirectBuffer;
		std::vector<int[3]> computeLocal; /*  Cache all the.  */
		/*  */
		FragGraphicUniform uniforms;

		// Main Rendering pipeline information.
		// Queue<Composition> compositions;

	  protected: /*  */
		void updateUniformLocations(Shader *Shader, UniformLocation *location,
									Shader::ShaderType shaderStage);

	  public:
		/*  Add elements.   */
		void addTexture(Texture *texture);
		void addShader(Shader *shader);
		void addCompute(Shader *compute);

		void setDisptchBuffer(Ref<Buffer> dispatchBuffer);
		const Ref<Buffer> &getDispatchBuffer() const;

		/*  Get elements.   */
		Texture *getTexture(int index);
		Shader *getShader(int index);
		Shader *getCompute(int index);
		const std::vector<Texture *> &getTextures() const;

		/*  Get element count. */
		int getNumTextures() const;
		int getNumShaders() const;
		int getNumCompute() const;

		/*  Get compute bind attributes.    */
		const std::vector<Texture::MapTarget> &getComputeMapTargets() const;
		const std::vector<Texture::Format> &getComputeFormats() const;
		const int *getComputeLocalWorkGroup(int index);

		/*  */
		const UniformLocation *getUniformLocation(int uid) const;
		UniformLocation *getUniformLocation(int uid);
		FragGraphicUniform *getFragUniform();

		/**
		 * Update all shader cached uniforms.
		 */
		void updateAllUniformLocations();
	};
} // namespace fragview

#endif
