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
#ifndef _FRAG_VIEW_H_
#define _FRAG_VIEW_H_ 1
#include <Renderer/IRenderer.h>
#include <FileNotify.h>
#include "IRenderPipelineBase.h"
#include "Config.h"
#include <Core/RefPtr.h>
#include <Renderer/RendererWindow.h>

namespace fragview {

	/**
	 * Main class for the fragment viewer
	 * executable program.
	 */
	class FVDECLSPEC FragView {
	public:
		FragView(int argc, const char** argv);
		~FragView(void);

		/**
		 * Run the application.
		 */
		void run(void);

		/**
		 * Get version of fragview program.
		 * @return non-null terminated string.
		 */
		static const char* getVersion(void);

	protected:

		void init(int argc, const char** argv);
		void loadDefaultSceneAsset(void);
		void cacheShaders(void);
		void loadCachedShaders(void);
		void loadShaders(void);

		/**
		 * Create window for displaying.
		 */
		void createWindow(int x, int y, int width, int height);

	private:    /*  */

		fragcore::Ref<fragcore::IRenderer> renderer;            /*  Low level rendering API interface.  */
		fragcore::Ref<IRenderPipelineBase> renderpipeline;
	//    EventController* controller;
		fragcore::Ref<fragcore::IScheduler> sch;            /*  */
		fragcore::Ref<fragcore::IScheduler> logicSch;       /*  */
		Config* config;                                     /*  Current config.    */   //TODO add suport for reference.
		fragcore::FileNotify* notify;                       /*  Notify Asset changes.  */
		//fragengine::Scene *scene;                             /*  Current scene.  */
		fragcore::RendererWindow *rendererWindow;           /*  */
		fragcore::Ref<fragcore::IFileSystem> fileSystem;    /*  */
	};
}

#endif