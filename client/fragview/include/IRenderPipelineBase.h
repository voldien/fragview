

#ifndef _FRAG_ENGINE_IRENDER_PIPELINE_BASE_H_
#define _FRAG_ENGINE_IRENDER_PIPELINE_BASE_H_ 1
#include "Node.h"
#include "Prerequisites.h"
#include "Scene/Scene.h"
#include <Core/Ref.h>
#include <Core/SmartReference.h>
#include <Renderer/Geometry.h>
#include <Renderer/Prerequisites.h>

namespace fragview {
	using namespace fragcore;
	/**
	 *	Responsible for rendering scene to default
	 *	framebuffer.
	 */
	class FVDECLSPEC IRenderPipelineBase : public SmartReference {
		friend class RenderPipelineFactory;

	  public:
		/**
		 * Draw scene, iterate through each camera
		 * and renderer the scene using the selected
		 * rendering interface in respect to the current settings.
		 *
		 * @param scene
		 * @param render
		 */
		virtual void draw(Node *node, FrameBuffer *frame, IRenderer *render) = 0;

		virtual void setRenderer(Ref<IRenderer> &renderer) = 0;

		virtual const Ref<IRenderer> &getRenderer(void) const = 0;

		virtual Ref<IRenderer> getRenderer(void) = 0;

		virtual void setViewport(int width, int height, IRenderer *render) = 0;

	  protected:
		/*  Basic internal components.  */
		Ref<IRenderer> renderer; /*  Main rendering API interface.  */
		Ref<Sync> syncObject;	 /*  Sync object.    */
		ViewPort *viewPort;		 // TODO determine what to do

		Ref<fragcore::Geometry> quadDisplay; /*  Quad display.   */
		Ref<Buffer> quadDisplayIndirect;	 /*  Quad indirect buffer.    */
		Ref<ProgramPipeline> displayShader;	 /*  Quad display shader.    */
	};
} // namespace fragview

#endif
