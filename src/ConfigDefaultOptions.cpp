#include"ConfigDefaultOptions.h"
#include"Config.h"
#include <Renderer/RendererFactory.h>
#include "Core/IO/FileSystem.h"
#include "OptionDicSet.h"

using namespace fragcore;
using namespace fragview;

//TODO REMove
Config *ConfigDefaultOptions::createDefaultConfig(void){

	Config *_global = new IConfig();
	_global->setName("global");
	Config &global = *_global;
#if defined(_DEBUG)
	global.set("debug", true); // Enable global debug for the application.
#else
	global.set("debug", false);
#endif

	/*  Sandbox fragview.   */
	global.set("sandbox", false);
	/*  Behavior functions.   */
	global.set("notify-file", true);
	global.set("drag-and-drop", true);
	
	/*  Internal.   */
	global.set("cache-shaders", true);
	global.set("use-cache-shaders", true);

	/*	Default interfaces configuration.	*/
	global.set("renderer-dynamicInterface",
			   fragcore::RenderingFactory::getInterfaceLibraryPath(fragcore::RenderingFactory::eOpenGL));
	global.set<int>("SIMD", Hpm::eHPM_DEFAULT);

	/*TODO determine if to relocate.    */
	/*	Engine default configuration.	*/
	IConfig &renderingConfig = global.getSubConfig("render-driver");
	renderingConfig.set("extensions", "");
	renderingConfig.set("layers", "");
	// OpenGL configuration.
	renderingConfig.set("opengl", -1);
	renderingConfig.set("core", 1);
	// Vulkan configuration.
	renderingConfig.set("vulkan-version", 110);
	// DirectX configuration.
	renderingConfig.set("direct-x-version", 0);
	//
	renderingConfig.set("alpha", false);
	renderingConfig.set("v-sync", true);
	renderingConfig.set("gamma-correction", true);
	renderingConfig.set("debug", false);
	renderingConfig.set("debug-tracer", true);
	renderingConfig.set("anti-aliasing-samples", 0);
	renderingConfig.set("anti-aliasing", false);
	renderingConfig.set("version", "auto"); //TODO check if a better name is better.

	/*  Global main window settings.    */
	IConfig &renderWindowSetting = global.getSubConfig("render-window-settings");
	renderWindowSetting.set("fullscreen", 0);
	renderWindowSetting.set("isFullscreen", 0);
	renderWindowSetting.set("screen-width", -1);
	renderWindowSetting.set("screen-height", -1);
	renderWindowSetting.set("screen-max-width", -1);
	renderWindowSetting.set("screen-max-height", -1);
	renderWindowSetting.set("screen-min-width", -1);
	renderWindowSetting.set("screen-min-height", -1);
	renderWindowSetting.set("screen-x", -1);
	renderWindowSetting.set("screen-y", -1);
	renderWindowSetting.set("background-rendering", false);
	renderWindowSetting.set("window-resizable", true);
	renderWindowSetting.set("window-bordered", true);

	/*  */
	IConfig &render3DSettingConfig = global.getSubConfig("render-graphic-settings");
	/*  Anti aliasing.  */
	render3DSettingConfig.set("anti-aliasing-samples", 0);
	render3DSettingConfig.set("anti-aliasing", false);
	render3DSettingConfig.set("anti-aliasing-technique", 0);
	/*  */
	render3DSettingConfig.set("texture-quality", 4);
	render3DSettingConfig.set("resolution-scale", 1.0f);
	/*  Shadow. */
	render3DSettingConfig.set("shadow", true);
	render3DSettingConfig.set("shadow-distance", 100.0f);
	render3DSettingConfig.set("shadow-resolution", 2048);
	render3DSettingConfig.set("shadow-cascaded", 4);
	//TODO change to percentage.
	render3DSettingConfig.set("shadow-cascaded-0", 20.0f);
	render3DSettingConfig.set("shadow-cascaded-1", 100.0f);
	render3DSettingConfig.set("shadow-cascaded-2", 250.0f);
	render3DSettingConfig.set("shadow-cascaded-3", 800.0f);
	/*  Light.  */

	IConfig &sceneConfig = global.getSubConfig("default-scene-settings");
	/*  Camera default. */
	IConfig &cameraConfig = sceneConfig.getSubConfig("main-camera");
	cameraConfig.set("camera-fov", 70.0f);
	cameraConfig.set("camera-near", 0.15f);
	cameraConfig.set("camera-far", 1000.0f);
	cameraConfig.set("camera-aspect", -1.0f);
	cameraConfig.set("enable-free-camera", false);
	cameraConfig.set("camera-", 0);

	/*  Scene properties.   */
	sceneConfig.set("free-rotation", true);
	sceneConfig.set("free-rotation-speed", 3.14f / 10.0f);
	sceneConfig.set("default-focus", true);

	/*  */
	IConfig &resourceConfig = global.getSubConfig("resource-settings");
	/*	Resources default configuration.	*/
	resourceConfig.set("shaddir", "/usr/share/fragview");
	resourceConfig.set("settings", "");
	resourceConfig.set("dump-configuration", "fragview.xml");
	resourceConfig.set("save-configuration", false);
	resourceConfig.set("fragview-internal-shaders-files", "fragview-internal-shaders.zip");
	resourceConfig.set("cache-directory", ".");

	IConfig &sandboxConfig = global.getSubConfig("sandbox");
	/*  Assets import.  */
	//sandboxConfig.set("texture0", 0);
	//sandboxConfig.set("shader0", 0);
	//sandboxConfig.set("compression", false);
	/*  Variables.  */
	sandboxConfig.set("num_textures", 0);
	sandboxConfig.set("num_shaders", 0);
	sandboxConfig.set("num_compute", 0);
	sandboxConfig.set("num_program_shaders", 0);

	/*  sandbox rendering settings. */
	IConfig &renderSandboxSettingsConfig = global.getSubConfig("render-sandbox-graphic-settings");

	/*	Rendering default configuration.	*/
	renderSandboxSettingsConfig.set("anti-aliasing-samples", 0);
	renderSandboxSettingsConfig.set("anti-aliasing", false);
	renderSandboxSettingsConfig.set("texture-quality", 4);
	renderSandboxSettingsConfig.set("resolution-scale", 1.0f);
	renderSandboxSettingsConfig.set("coverage", 0);

	IConfig &sceneAsset = global.getSubConfig("scene-asset");

	return _global;
}