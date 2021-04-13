#include "FragView.h"
#include "OptionDicSet.h"
#include "RenderPipeline/RenderPipelineForward.h"
#include "RenderPipeline/RenderPipelineSettings.h"
#include "RenderPipelineSandBox.h"
#include "Renderer/RenderDesc.h"
#include "Renderer/RendererFactory.h"
#include "Renderer/ViewPort.h"
#include "SandBoxSubScene.h"
#include "Scene/Scene.h"
#include "ShaderLoader.h"
#include "UserEvent.h"
#include <Asset/AssetHandler.h>
#include <Core/IO/FileIO.h>
#include <Core/IO/FileSystem.h>
#include <Core/IO/ZipFileSystem.h>
#include <Core/SystemInfo.h>
#include <Exception/InvalidArgumentException.h>
#include <Exception/NotSupportedException.h>
#include <FileNotify.h>
#include <FontFactory.h>
#include <HpmCpp.h>
#include <Renderer/Query.h>
#include <SDL2/SDL.h>
#include <Scene/Scene.h>
#include <Scene/Scene.h>
#include <Scene/SceneFactory.h>
#include <Utils/ShaderUtil.h>
#include <Utils/StringUtil.h>
#include <Utils/TextureUtil.h>
#include <Video/VideoFactory.h>
#include <audio/AudioFactory.h>
#include <cassert>

using namespace fragcore;
using namespace fragengine;
using namespace fragview;

FragView::FragView(int argc, const char **argv) {

	init(argc, argv);

	/*  Assert dependent variables that should not be null at this point. */
	assert(this->config && *this->renderer && this->rendererWindow && *this->sch);

	loadDefaultSceneAsset();

	/*  Start the notify change.   */
	this->notify->start();

	/*  Print debug.    */
	if (config->get<bool>("debug")) {
		Log::log(Log::Debug, "Configuration internal value tree\n");
		config->printTable();
	}
}

FragView::~FragView(void) {

	const IConfig &resourceConfig = this->config->getSubConfig("resource-settings");
	if (this->config && resourceConfig.get<bool>("save-configuration")) {
		IO *fileIO =
			FileSystem::getFileSystem()->openFile(resourceConfig.get<const char *>("dump-configuration"), IO::WRITE);
		Ref<IO> ref_io = Ref<IO>(fileIO);
		/*  TODO  */

		/*  */
		this->config->save(ref_io, Config::JSON);
		fileIO->close();
		delete fileIO;
	}

	/*  Release all assets.    */
	this->notify->unRegisterAllAsset();

	/*  Terminate scheduler.    */
	this->sch->terminate();
	delete *this->sch;
	this->logicSch->terminate();
	delete *this->logicSch;

	this->rendererWindow->close();

	/*  Reduce reference and delete resources.  */
	if (this->renderer->deincreemnt())
		delete *this->renderer;
	// delete this->scene->getGLSLSandBoxScene();
	delete this->scene;
	delete this->notify;
	delete this->config;
	delete this->rendererWindow;

	/*  Release event and timer.    */
	SDL_QuitSubSystem(SDL_INIT_EVENTS | SDL_INIT_TIMER);
}

void FragView::init(int argc, const char **argv) {
	int status;

	/*  Create task scheduler.  */
	this->sch = Ref<IScheduler>(new TaskScheduler(2, 48));
	this->logicSch = Ref<IScheduler>(new TaskScheduler(SystemInfo::getCPUCoreCount(), 128));

	/*	Initilize the fileystem with scheduler.	*/
	FileSystem::createFileSystem(this->sch);

	/*  Read first options.   */
	this->config = Config::createConfig(argc, argv, Config::getConfigFilePath(argc, argv));

	/*  Verbose information.    */
	Log::log(Log::Verbose, "Platform: %s\n", SystemInfo::getOperatingSystemName(SystemInfo::getOperatingSystem()));
	Log::log(Log::Verbose, "CPU Cores: %d\n", SystemInfo::getCPUCoreCount());
	Log::log(Log::Verbose, "Memory: %d MB\n", SystemInfo::systemMemorySize());
	Log::log(Log::Verbose, "Cache line: %d bytes\n", SystemInfo::getCPUCacheLine());
	Log::log(Log::Verbose, "page size: %d bytes\n", SystemInfo::getPageSize());

	/*	TODO determine how much shall be handle by the window manager once supported.	*/
	status = SDL_Init(SDL_INIT_EVENTS | SDL_INIT_TIMER);
	if (status != 0)
		throw RuntimeException(fvformatf("failed to initialize SDL library : %d - %s", status, SDL_GetError()));

	/*  Initialize HPM.  */
	if (!Hpm::init((Hpm::HPMSIMD)this->config->get<int>("SIMD")))
		throw RuntimeException("Failed to initialize the hpm library.");
	Log::log(Log::Verbose, "HPM SIMD using: %s\n", hpm_get_simd_symbol(hpm_get_simd()));

	/*  Create rendering interface. */
	const IConfig &renderConfig = config->getSubConfig("render-driver");
	this->renderer = Ref<IRenderer>(
		RenderingFactory::createRendering(config->get<const char *>("renderer-dynamicInterface"), &renderConfig));
	Log::log(Log::Verbose, "Loading Renderer: %s-%s\n", (*this->renderer)->getName().c_str(),
			 (*this->renderer)->getVersion());
	Log::log(Log::Verbose, "API Internal API version: %s\n", (*this->renderer)->getAPIVersion());
	(*this->renderer)->setVSync(renderConfig.get<bool>("v-sync"));

	/*  Create file notify.    */
	if (this->config->get<bool>("notify-file"))
		this->notify = new FileNotify(this->sch);

	/*  Create window - default position and size.  */
	const IConfig &windowConfig = this->config->getSubConfig("render-window-settings");
	this->createWindow(windowConfig.get<int>("screen-x"), windowConfig.get<int>("screen-y"),
					   windowConfig.get<int>("screen-width"), windowConfig.get<int>("screen-height"));
}

void FragView::loadDefaultSceneAsset(void) {
	/*	*/
	Capability capability;
	(*this->renderer)->getCapability(&capability);
	ShaderLanguage supportedLanguages = this->renderer->getShaderLanguage();

	IConfig &global = *this->config;
	bool useSandBox = this->config->get<bool>("sandbox"); // Fragment sandbox.
	IConfig &resourceConfig = this->config->getSubConfig("resource-settings");

	/*  Display information.    */
	Display::DPI dpi;
	this->rendererWindow->getCurrentDisplay()->getDPI(&dpi);
	TextureFormat format = this->rendererWindow->getCurrentDisplay()->getFormat();

	ZipFileSystem *internalAsset = NULL;
	Ref<IO> internal_zip_io;

	/*	Search for the file.	*/
	const char *internal_asset_filename = resourceConfig.get<const char *>("fragview-internal-shaders-files");
	Log::log(Log::Verbose, "Reading asset file: %s\n", internal_asset_filename);
	const char *apppath =
		fvformatf("%s/%s", resourceConfig.get<const char *>("shaddir"), internal_asset_filename).c_str();
	std::string fullPath = FileSystem::getAbsolutePath(apppath);
	if (FileSystem::getFileSystem()->exists(internal_asset_filename))
		internal_zip_io = Ref<IO>(FileSystem::getFileSystem()->openFile(internal_asset_filename, IO::READ));
	else if (FileSystem::getFileSystem()->exists(fullPath.c_str()))
		internal_zip_io = Ref<IO>(FileSystem::getFileSystem()->openFile(fullPath.c_str(), IO::READ));
	else
		throw RuntimeException(
			fvformatf("Could not find internal resources for default shaders : %s", internal_asset_filename));
	internalAsset = ZipFileSystem::createZipFileObject(internal_zip_io, this->sch);

	// TODO add support.
	// Ref<IO> fontIO = Ref<IO>(internalAsset->openFile("DroidSansFallback.ttf", IO::READ));
	// fontIO->seek(0, IO::Seek::SET);
	// FontFactory::createSDFFont(this->renderer, fontIO, 10);

	if (useSandBox) {
		IConfig &sandBoxSettings = this->config->getSubConfig("render-sandbox-graphic-settings");
		IConfig &sandboxConfig = this->config->getSubConfig("sandbox");

		RenderPipelineSettings setting(sandBoxSettings);
		this->renderpipeline = Ref<IRenderPipelineBase>(new RenderPipelineSandBox(this->renderer));

		/*  Create Scene.   */
		this->scene = SceneFactory::createScene(*this->renderer, SceneFactory::eSandBox);
		SandBoxSubScene *sandBoxSubScene = NULL;
		// 		scene->createGLSLSandBoxScene();
		// 		SandBoxSubScene *sandbox = scene->getGLSLSandBoxScene();
		// 		FragGraphicUniform *fragGraphicUniform = sandbox->getFragUniform();

		// 		const int nrInDispatch = 256;
		// 		BufferDesc computeDesc = {};
		// 		computeDesc.type = BufferDesc::eIndirectDispatch;
		// 		computeDesc.data = NULL;
		// 		computeDesc.size = sizeof(IndirectDispatch) * nrInDispatch;
		// 		computeDesc.hint = (BufferDesc::BufferHint)(BufferDesc::eRead | BufferDesc::eStream);
		// 		computeDesc.marker.markerName = "Compute indirect dispatch";
		// 		scene->getGLSLSandBoxScene()->setDisptchBuffer(Ref<Buffer>(rendering->createBuffer(&computeDesc)));

		// 		/*  Initialize the fragment.  */
		// 		fragGraphicUniform->inputs.x = 0.0f;
		// 		fragGraphicUniform->inputs.y = 0.0f;
		// 		fragGraphicUniform->inputs.ax = 0.0f;
		// 		fragGraphicUniform->inputs.ay = 0.0f;
		// 		fragGraphicUniform->inputs.wheel = 0.0f;
		// 		fragGraphicUniform->inputs.wheelacc = 0.0f;

		// 		int view[2];
		// //		rendering->getView(0)-> NULL, NULL, &view[0], &view[1]);
		// //		fragGraphicUniform->window.width = view[0];
		// //		fragGraphicUniform->window.height = view[1];
		// //		fragGraphicUniform->window.x = 0.0f;
		// //		fragGraphicUniform->window.y = 0.0f;

		// 		fragGraphicUniform->time.time = 0.0f;
		// 		fragGraphicUniform->time.deltaTime = 0.0f;

		// this->scene->getGLSLSandBoxScene();

		/*  TODO add internal verification of asset if enabled.    */

		bool internalShaderNotLoaded = false;
		const char *cache_directory = resourceConfig.get<const char *>("cache-directory");
		// TODO determine if directory or filepath.
		std::string shader_cache_filepath = fvformatf("%s/shader-cache.json", cache_directory);
		if (this->config->get<bool>("use-cache-shaders")) {
			IConfig shaderCache;
			try {
				// TODO
				// shaderCache.parseConfigFile(Ref<IO>(NULL), IConfig::JSON);
				internalShaderNotLoaded = true;
			} catch (const IException &ex) {
				internalShaderNotLoaded = false;
			}
		}

		/*  */
		ASyncHandle displayFragV = 0;
		ASyncHandle displayFragF = 0;
		if (!internalShaderNotLoaded) {
			/*  Load default display shader.    */
			if (supportedLanguages & GLSL) {
				displayFragV = internalAsset->openASyncFile("shaders/glsl/display.vert", IO::READ);
				displayFragF = internalAsset->openASyncFile("shaders/glsl/display.frag", IO::READ);
			} else if (supportedLanguages & SPIRV) {
				displayFragV = internalAsset->openASyncFile("shaders/spirv/displayV.spv", IO::READ);
				displayFragF = internalAsset->openASyncFile("shaders/spirv/displayF.spv", IO::READ);
			} else
				throw NotSupportedException(fvformatf("Non-Supported shader for language %d", supportedLanguages));

			/*  Invoke async load.  */
			internalAsset->asyncWriteFile(displayFragV, NULL, 0, NULL);
			internalAsset->asyncWriteFile(displayFragF, NULL, 0, NULL);
		}

		// ShaderUtil::loadShader

		/*  */
		// Get local share directory. TODO
		//"binary-program"
		// for(int i = 0; i < sandboxConfig.get())

		/*  Create shaders. */
		const int nShadersInSandBox = sandboxConfig.get<int>("num_shaders");
		for (int i = 0; i < sandboxConfig.get<int>("num_shaders"); i++) {
			ProgramPipeline *shader;

			/*  Check if fragment shader is supported.  */
			if (!capability.sFragmentShader)
				throw RuntimeException(
					fvformatf("Fragment shader is not support for %s\n", (*this->renderer)->getName()));

			std::string path = sandboxConfig.get<std::string>(fvformatf("shader%d", i)).c_str();
			IO *ref = FileSystem::getFileSystem()->openFile(path.c_str(), IO::Mode::READ);

			/*  Load fragment program.  */
			ShaderLoader::loadFragmentProgramPipeline(ref, GLSL, (*this->renderer), &shader);
			// scene->getGLSLSandBoxScene()->addShader(shader);
			Log::log(Log::Verbose, "Loaded Shader: %s\n", path.c_str());

			// this->notify->registerAsset(path.c_str(), shader, eShader);

			delete ref;
		}

		/*  Create compute shaders.    */
		const int nComputeInSandBox = sandboxConfig.get<int>("num_compute");
		for (int i = 0; i < sandboxConfig.get<int>("num_compute"); i++) {
			ProgramPipeline *compute;

			/*  Check if compute shader is supported.  */
			std::string path = sandboxConfig.get<std::string>(fvformatf("compute%d", i)).c_str();
			if (!capability.sComputeShader)
				throw RuntimeException(
					fvformatf("Compute shader is not support for %s\n", (*this->renderer)->getName()));

			IO *ref = FileSystem::getFileSystem()->openFile(path.c_str(), IO::Mode::READ);
			/*  */
			// ShaderUtil::loadComputeShader(ref, *this->renderer, &compute);
			// scene->getGLSLSandBoxScene()->addCompute(compute);
			Log::log(Log::Verbose, "Loaded Compute Shader: %s\n", path.c_str());

			// this->notify->registerAsset(path.c_str(), compute, eShader);

			delete ref;
		}

		/*  Create textures.    */
		// TODO add video texture support.
		const int nTexturesInSandBox = sandboxConfig.get<int>("num_textures");
		for (int i = 0; i < capability.sMaxTextureUnitActive; i++) {
			Texture *texture;
			const char *path = NULL;
			const std::string tex_key = fvformatf("texture%d", i);
			if (sandboxConfig.isSet(tex_key.c_str())) {
				path = sandboxConfig.get<const char *>(tex_key);
				if (path) {
					/*  Determine file type.    */
					const char *ext = FileSystem::getFileExtension(path);

					TextureUtil::loadTexture(path, *this->renderer, &texture);
					// scene->getGLSLSandBoxScene()->addTexture(texture);
					Log::log(Log::Verbose, "Loaded texture: %s\n", path);
				}

				// this->notify->registerAsset(path, texture, eTexture);
			}
		}

		if (!internalShaderNotLoaded) {
			/*  Close async.    */
			internalAsset->asyncClose(displayFragV);
			internalAsset->asyncClose(displayFragF);

			/*  Load display shaders.   */
			//		internalShader->asyncWait(displayFragV);
			//		internalShader->asyncWait(displayFragF);

			/*  Load shaders.   */
		}

		// TODO add support for caching shaders. -- as json files.
		if (this->config->get<bool>("cache-shaders")) {
			IConfig internaCache;
			internaCache.setName("shader-cache");
			for (int i = 0; i < sandBoxSubScene->getNumShaders(); i++) {
				Shader *shader = sandBoxSubScene->getShader(i)->getShader(ProgramPipeline::VERTEX_SHADER);

				IConfig &shaderCache = internaCache.getSubConfig(shader->getName());
				long int bsize;
				unsigned int format;
				const void *pbinary = shader->getBinary(&bsize, &format);
				shaderCache.setBlob("shader", pbinary, bsize);
				shaderCache.set<int>("format", bsize);
			}

			Ref<IO> f = Ref<IO>(FileSystem::getFileSystem()->openFile(shader_cache_filepath.c_str(), IO::WRITE));
			internaCache.save(f, IConfig::JSON);
		}
	} else {
		IConfig &sceneConfig = this->config->getSubConfig(CONFIG_SCENE);

		/*  Create 3D view scene.   */
		// this->scene = SceneFactory::createScene(*this->renderer, SceneFactory::eWorldSpace);
		this->renderpipeline = Ref<IRenderPipelineBase>(new RenderPipelineForward(this->renderer));

		// Read from options for loading the scene.
		ASyncHandle displayFragV = internalAsset->openASyncFile("shaders/glsl/skybox.vert", IO::READ);
		ASyncHandle displayFragF = internalAsset->openASyncFile("shaders/glsl/skybox.frag", IO::READ);

		/*	Load asset.	*/

		/*	Load shaders.	*/

		/*	Create camera.	*/
	}

	internalAsset->deincreemnt();
	delete internalAsset;

	// Assert the variables
	// assert(this->scene && *this->renderpipeline);

	loadCachedShaders();
	loadShaders();
	cacheShaders();
}
void FragView::cacheShaders(void) {}
void FragView::loadCachedShaders(void) {}
void FragView::loadShaders(void) {}

void FragView::createWindow(int x, int y, int width, int height) {
	assert(*this->renderer);

	/*  Start loading the icon. */
	std::string iconpath;
	void *iconBuffer;
	bool foundIcon = false;
	const IFileSystem *fileSystem = FileSystem::getFileSystem();

	/*  Search for the file path for the icon. Top bottom on most common location  */
	const char *iconFileName = "fragview.png";
	IConfig &resourceConfig = this->config->getSubConfig("resource-settings");
	/*  Local directory of the executable.  */
	if (fileSystem->exists(iconFileName)) {
		iconpath = iconFileName;
		foundIcon = true;
	} else {
		// TODO determine based on platform for which directory to search.
		/*  Share directory.    */
		iconpath = fvformatf("%s/%s", resourceConfig.get<const char *>("shaddir"), iconFileName);
		if (fileSystem->exists(iconpath.c_str())) {
			foundIcon = true;
		}
	}

	/*  */
	// TODO determine if async supported.
	ASyncHandle iconHandle = NULL;
	if (foundIcon) {
		iconHandle = fileSystem->openASyncFile(iconpath.c_str(), IO::READ);
		fileSystem->asyncReadFile(iconHandle, NULL, -1, NULL);
	}

	/*  Primary display.    */
	Display *display = WindowManager::getInstance()->getDisplay(0);

	/*  Compute screen size and location if default value provided.   */
	if (width == -1)
		width = display->width() / 2;
	if (height == -1)
		height = display->height() / 2;
	if (x == -1)
		x = width / 2;
	if (y == -1)
		y = height / 2;

	/*  Create window.  */
	this->rendererWindow = renderer->createWindow(x, y, width, height);

	// this->rendererWindow = new SDLRendererWindow();
	// this->rendererWindow = (*this->renderer)->createWindow(x, y, width, height, rendererWindow);
	assert(this->rendererWindow);

	//	(*this->renderer)->setCurrentWindow(this->rendererWindow);

	/*  Update default viewport.    */
	(*this->renderer)->getView(0)->setDimensions(0, 0, width, height);

	/*  Set current window title.   */
	std::string title = fvformatf("FragView - %s : %s - %s", FragView::getVersion(),
								  (*this->renderer)->getName().c_str(), (*this->renderer)->getAPIVersion());
	this->rendererWindow->setTitle(title.c_str());

	/*  Set window properties.  */
	const IConfig &windowConfig = this->config->getSubConfig("render-window-settings");
	this->rendererWindow->resizable(windowConfig.get<bool>("window-resizable"));
	this->rendererWindow->setFullScreen(windowConfig.get<bool>("fullscreen"));
	this->rendererWindow->setBordered(windowConfig.get<bool>("window-bordered"));
	if (windowConfig.get<int>("screen-max-width") > 0 || windowConfig.get<int>("screen-max-height") > 0) {

		const int max_w = windowConfig.get<int>("screen-max-width");
		const int max_h = windowConfig.get<int>("screen-max-height");
		this->rendererWindow->setMaximumSize(max_w, max_h);
	}
	if (windowConfig.get<int>("screen-min-width") > 0 || windowConfig.get<int>("screen-min-height") > 0) {
		const int min_w = windowConfig.get<int>("screen-min-width");
		const int min_h = windowConfig.get<int>("screen-min-height");
		this->rendererWindow->setMinimumSize(min_w, min_h);
	}

	/*  Load icon.  */
	if (foundIcon) {
		unsigned int iconw, iconh, len;
		try {
			// TODO extract IO status.
			fileSystem->asyncWait(iconHandle);
			ASync::IOStatus status = fileSystem->getIOStatus(iconHandle);
			fileSystem->asyncClose(iconHandle);

			void *icon = TextureUtil::loadTextureData(iconBuffer, status.nbytes, &iconw, &iconh);
			if (icon && iconw > 0 && iconh > 0) {
				// TODO change type once it has been decided.
				rendererWindow->setIcon(icon);
			}
			free(icon);
			free(iconBuffer);
		} catch (InvalidArgumentException &ex) {
			Log::error("Failed to load icon on path: %s\n", iconpath.c_str());
		}
	}

	/*  Display window. */
	this->rendererWindow->setSize(width, height); // Force resize event to be poll on start.
	this->rendererWindow->show();
}

void FragView::run(void) {
	SDL_Event event = {};

	/*	*/
	unsigned int width;
	unsigned int height;

	/*	*/
	bool isAlive = true; /*	*/
	bool visible = true; /*	*/

	/*  */
	IConfig &windowConfig = config->getSubConfig("render-window-settings");
	bool renderInBackground = true;
	const bool renderOutOfFoucs = windowConfig.get<bool>("background-rendering");

	// Once everything loaded. Focus the program.
	// this->scene->getTime()->start();
	this->rendererWindow->focus();

	FragGraphicUniform tmp;
	/*  Main logic loop.    */
	while (isAlive) {
		// Scene *currentScene = this->scene;
		SandBoxSubScene *sandbox = NULL;
		// currentScene->getGLSLSandBoxScene();
		FragGraphicUniform *uniform = &tmp; // TODO resolve sandbox->getFragUniform();

		while (SDL_PollEvent(&event)) {

			switch (event.type) {
			case SDL_WINDOWEVENT:
				switch (event.window.event) {
				case SDL_WINDOWEVENT_SIZE_CHANGED:
					// TODO remove , changed so that it passes throught the event controller.
					width = event.window.data1;
					height = event.window.data2;
					uniform->window.width = (float)width;
					uniform->window.height = (float)height;
					Log::log(Log::Debug, "viewport resized: %dx%d\n", width, height);
					// this->renderpipeline->setViewport(width, height, *this->renderer);
					break;
				case SDL_WINDOWEVENT_MOVED:
					uniform->window.x = (float)event.window.data1;
					uniform->window.y = (float)event.window.data2;
					break;
				case SDL_WINDOWEVENT_HIDDEN:
					visible = false;
					break;
				case SDL_WINDOWEVENT_EXPOSED:
				case SDL_WINDOWEVENT_SHOWN:
					visible = true;
					break;
				case SDL_WINDOWEVENT_ENTER:
					renderInBackground = renderOutOfFoucs;
					break;
				case SDL_WINDOWEVENT_LEAVE:
					renderInBackground = renderOutOfFoucs;
					break;
				case SDL_WINDOWEVENT_CLOSE:
					return; /*	Quit application.	*/
				case SDL_WINDOWEVENT_HIT_TEST:
					break;
				default:
					break;
				}
				break;
			case SDL_QUIT:
				isAlive = SDL_FALSE;
				break;
			case SDL_KEYDOWN:
				// TODO redidect to command buffer.
				if (event.key.keysym.sym == SDLK_RETURN && (event.key.keysym.mod & SDLK_LCTRL)) {
					if (!windowConfig.get<int>("isFullscreen")) {
						this->rendererWindow->setFullScreen(1);
						windowConfig.set<int>("isFullscreen", 1);
					} else {
						this->rendererWindow->setFullScreen(0);
						windowConfig.set<int>("isFullscreen", 0);
					}
				} else if (event.key.keysym.sym == SDLK_F12) {
					/* Save current framebuffer.  */
					FrameBuffer *def = (*this->renderer)->getDefaultFramebuffer(this->rendererWindow);
					TextureUtil::saveTexture(fvformatf("screen - %s.png", Time::getDate()).c_str(), *this->renderer,
											 def->getAttachment(0));

				} else {
				}
				break;
			case SDL_KEYUP:
				break;
			case SDL_MOUSEBUTTONDOWN:
				break;
			case SDL_MOUSEMOTION:
				uniform->inputs.x = event.motion.x;
				uniform->inputs.y = event.motion.y;
				uniform->inputs.ax = event.motion.xrel;
				uniform->inputs.ay = event.motion.yrel;
				break;
			case SDL_MOUSEWHEEL: {
				const float delta = 0.01f;
				uniform->inputs.wheel += (float)event.wheel.y * delta;
				uniform->inputs.wheelacc = (float)event.wheel.y * delta;
			} break;
			case SDL_JOYAXISMOTION:
				uniform->inputs.ax = event.jaxis.value;
				break;
			case SDL_JOYBUTTONDOWN:
				break;
			case SDL_FINGERMOTION:
				uniform->inputs.x = event.tfinger.x;
				uniform->inputs.y = event.tfinger.y;
				uniform->inputs.ax = event.tfinger.dx;
				uniform->inputs.ay = event.tfinger.dy;
				break;
			case SDL_DROPFILE: {
				/*  Determine what file.  */
				/*  Send to task scheduler to deal with.    */
				// AssetHandler::handleAssetDrop(event.drop.file);//TODO improve.
				event.drop.file;
				this->sch;
			} break;
			case SDL_USEREVENT: {
				if (event.user.code == ASSET_UPDATE) {
					/*  Asset notification handle current graphic context. */
					FileNotificationEvent *objectEvent = (FileNotificationEvent *)event.user.data1;
					try {
						// AssetHandler::handleAssetEvent(objectEvent);
					} catch (RuntimeException &err) {
						Log::log(Log::Error, err.what());
					}
					Log::log(Log::Verbose, "Updating %s\n", objectEvent->path);
					this->notify->eventDone(objectEvent);
					sandbox->updateAllUniformLocations();
				}
			} break;
			default:
				break;
			}
		}

		/*  */
		// scene->getTime()->internalUpdate();
		// uniform->time.time = scene->getTime()->timef();

		// Update Object

		// Update animation

		// Finalize animation

		// Update graph and its spatail

		// Final graph and spatil.

		// Update Video texture

		// Update skinned.
		this->logicSch->wait();
		//		schWaitTask(*this->logicSch);

		/*	render only if visible of configure too.	*/
		if (visible || renderInBackground) {

			/*  Draw current display on the current scene.  */
			// this->renderpipeline->draw(this->scene, this->renderer->getDefaultFramebuffer(this->rendererWindow),
			//                            *this->renderer);
			/*  Draw debug. */

			/*  Swap buffer.    */
			(*this->renderer)->swapBuffer();
		}
	}
}

const char *FragView::getVersion(void) { return FV_VERSION; }
