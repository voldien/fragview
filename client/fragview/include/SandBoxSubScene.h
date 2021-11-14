/**
	FrameView for rendering shaders in screen space.
	Copyright (C) 2018  Valdemar Lindberg

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
#ifndef _FRAGVIEW_GLSLSANDBOXSUBSCENE_H_
#define _FRAGVIEW_GLSLSANDBOXSUBSCENE_H_ 1
#include <FragCore.h>
#include <RenderPrerequisites.h>
#include <Scene/Scene.h>
#include <map>
#include <vector>
using namespace fragcore;

namespace fragview {
	/**
	 *
	 */
	typedef struct uniform_location_t {
		int resolution; /*	resolution. */
		int position;	/*	resolution. */
		int time;		/*	time in seconds as float.	*/
		int deltatime;	/*	delta time.	*/
		int mouse;		/*	mouse.	*/
		int movement;	/*	mouse.	*/
		int offset;		/*	offset.	*/
		int backbuffer; /*	previous buffer.	*/
		int stdin;		/*	stdin data.	*/
		int wheel;		/*				*/
		int pipe;		/*              */
		int pointer;	/*  */
	} UniformLocation;

	/**
	 *
	 */
	typedef struct input_uniform_t {
		float x, y;	  /*  */
		float ax, ay; /*  */
		float wheel, wheelacc;
	} InputUniform;

	/**
	 *
	 */
	typedef struct window_uniform_t {
		float width, height; /*  */
		float x, y;			 /*  */
	} WindowUniforms;

	/**
	 *
	 */
	typedef struct time_uniform_t {
		float time;		 /*  */
		float deltaTime; /*  */
	} TimeUniform;

	/**
	 *
	 */
	typedef struct graphic_uniform_t {
		InputUniform inputs;
		WindowUniforms window;
		TimeUniform time;
	} FragGraphicUniform;

	/*  Composition sequence configuration.    */
	typedef struct composition_t {

	} Composition;

	// TODO add polymorphism,
	// TODo add event model.
	// TODO relocate to the sandbox that will be part of the scene rather than a seperated object.
	/**
	 *
	 */
	class SandBoxSubScene : public SmartReference {
		friend class Scene;

		friend class SceneFactory;

	  private:
		// TODO add reference instead.
		std::vector<Texture *> textures;
		std::map<unsigned int, UniformLocation> cachedUniforms;

		std::map<ProgramPipeline::ShaderType, std::vector<ProgramPipeline *>> sequences;

		/*  Compute shader attributes.  */
		std::vector<Texture::MapTarget> computeMapTargets;
		std::vector<Texture::Format> computeFormats;
		Ref<Buffer> indirectBuffer;
		std::vector<int[3]> computeLocal; /*  Cache all the */
		/*  */
		FragGraphicUniform uniforms;

		// Main Rendering pipeline information.
		// Queue<Composition> compositions;

	  protected: /*  */
		void updateUniformLocations(ProgramPipeline *programPipeline, UniformLocation *location,
									ProgramPipeline::ShaderType shaderStage);

	  public:
		/*  Add elements.   */
		void addTexture(Texture *texture);

		void addShader(ProgramPipeline *shader);

		void addCompute(ProgramPipeline *compute);

		void setDisptchBuffer(Ref<Buffer> dispatchBuffer);

		const Ref<Buffer> &getDispatchBuffer(void) const;

		/*  Get elements.   */
		Texture *getTexture(int index);

		ProgramPipeline *getShader(int index);

		ProgramPipeline *getCompute(int index);

		const std::vector<Texture *> &getTextures(void) const;

		/*  Get element count. */
		int getNumTextures(void) const;

		int getNumShaders(void) const;

		int getNumCompute(void) const;

		/*  Get compute bind attributes.    */
		const std::vector<Texture::MapTarget> &getComputeMapTargets(void) const;

		const std::vector<Texture::Format> &getComputeFormats(void) const;

		const int *getComputeLocalWorkGroup(int index);

		/*  */
		const UniformLocation *getUniformLocation(int uid) const;

		UniformLocation *getUniformLocation(int uid);

		FragGraphicUniform *getFragUniform(void);

		/**
		 * Update all shader cached uniforms.
		 */
		void updateAllUniformLocations(void);

	  public:
		~SandBoxSubScene(void);
	};
} // namespace fragview

#endif
