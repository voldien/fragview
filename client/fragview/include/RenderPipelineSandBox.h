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
#ifndef _FRAGVIEW_RENDERPIPELINE_SANDBOX_H_
#define _FRAGVIEW_RENDERPIPELINE_SANDBOX_H_ 1
#include <FragCore.h>

#include <RenderPipeline/IRenderPipelineBase.h>

namespace fragview {
	using namespace fragcore;
	using namespace fragengine;
	/**
	 * @brief
	 *
	 */
	class FVDECLSPEC RenderPipelineSandBox : public fragengine::IRenderPipelineBase {
	  public:
		RenderPipelineSandBox(Ref<IRenderer> &renderer);

		virtual ~RenderPipelineSandBox();

		virtual void setRenderer(Ref<IRenderer> &renderer) override;

		virtual const Ref<IRenderer> &getRenderer() const override;

		virtual Ref<IRenderer> getRenderer() override;

		virtual void setViewport(int width, int height, IRenderer *render) override;

		virtual void draw(Scene *scene, FrameBuffer *frame, IRenderer *render) override;

		virtual RenderQueue getSupportedQueue() const override;

	  protected:
		virtual void createFrameTexture(IRenderer *renderer, unsigned int width, unsigned height);

		virtual void init(Ref<IRenderer> &renderer, const IConfig *config);
		// virtual void updateUniform();

	  private:
		// Ref<Geometry> quadDisplay;
		Ref<Buffer> quadDisplayIndirect;
		Ref<Shader> displayShader;

		/*  Compute shader objects. */
		Ref<Texture> compute;
		Ref<Texture> computeView;
		Ref<Buffer> computeIndirect;
		bool supportComputeShader;

		Ref<FrameBuffer> frameBuffer;
		ViewPort *viewPort;
		Capability capability;
	};
} // namespace fragview

#endif
