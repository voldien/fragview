#include <Utils/StringUtil.h>
#include <SDLDisplay.h>
#include <Exception/NotSupportedException.h>
#include <SDL2/SDL_syswm.h>
#include <Exception/RuntimeExecption.h>
#include <Exception/NotImplementedException.h>
#include"SDLRendererWindow.h"

using namespace fragcore;


void SDLRendererWindow::showWindow() {
	SDL_ShowWindow(this->window);
}

void SDLRendererWindow::hideWindow() {
	SDL_HideWindow(this->window);
}

void SDLRendererWindow::closeWindow(void) {
	SDL_DestroyWindow(this->window);
}

void SDLRendererWindow::swapBuffer() {
	switch(this->apiBinded){
		case 1:
			SDL_GL_SwapWindow(this->window);
			break;
		default:
			break;
	}
}

bool SDLRendererWindow::isFullScreen(void) const
{
	return false;
}

void SDLRendererWindow::setPosition(int x, int y) {
	SDL_SetWindowPosition(this->window, x, y);
}

void SDLRendererWindow::setSize(int width, int height) {
	SDL_SetWindowSize(this->window, width, height);
}

void SDLRendererWindow::vsync(bool state) {
	SDL_GL_SetSwapInterval(state);
}

bool SDLRendererWindow::assertConfigAttributes(const fragcore::IConfig *iConfig) {
	//	for (int i = 0; i < numReqConfigKeys; i++){
//	if(!iConfig->isSet(reqConfigKey[i]))
//
//	}
	return false;
}


float SDLRendererWindow::getGamma(void) const {
	ushort ramp[256 * 3];
	int err = SDL_GetWindowGammaRamp(this->window, &ramp[256 * 0], &ramp[256 * 1], &ramp[256 * 2]);
	if (err == -1)
		throw NotSupportedException(SDL_GetError());

	return this->computeGammaExponent(ramp);
}

void SDLRendererWindow::setGamma(float gamma) {
	Uint16 ramp[256 * 3] = { 0 };

	this->calculateGammaLookupTable(gamma, ramp);
	int err = SDL_SetWindowGammaRamp(this->window, &ramp[256 * 0], &ramp[256 * 1], &ramp[256 * 2]);
	if (err == -1)
		throw NotSupportedException(fvformatf("Failed to set window gamma %f: %s", gamma, SDL_GetError()));
}

SDLRendererWindow::~SDLRendererWindow(void) {

}

void SDLRendererWindow::getPosition(int *x, int *y) const {
	SDL_GetWindowPosition(this->window, x, y);
}

void SDLRendererWindow::getSize(int *width, int *height) const {
	SDL_GetWindowSize(this->window, width, height);
}

fragcore::Display *SDLRendererWindow::getCurrentDisplay(void) const {
	int index;
	index = SDL_GetWindowDisplayIndex(this->window);
	return new SDLDisplay(index);
}

void SDLRendererWindow::createWindow(int x, int y, int width, int height, const char *api) {

	/*  */
	if (strcmp(api, "opengl") == 0)
		window = SDL_CreateWindow("", x, y, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_HIDDEN);
	else if (strcmp(api, "vulkan") == 0)
		window = SDL_CreateWindow("", x, y, width, height, SDL_WINDOW_VULKAN | SDL_WINDOW_HIDDEN);
	else if (strcmp(api, "directX") == 0)
		window = SDL_CreateWindow("", x, y, width, height, SDL_WINDOW_HIDDEN);
	else
		throw RuntimeException(fvformatf("Invalid API support - %s.", api));

	/*  */
	if(window == NULL)
		throw RuntimeException(fvformatf("Failed to create window %s for API %s",SDL_GetError(), api));
	this->api = api;
}

void SDLRendererWindow::useWindow(void *pdata) {
	if (strcmp(api, "opengl") == 0) {
		if (SDL_GL_MakeCurrent(this->window, (SDL_GLContext *) pdata) != 0) {
			throw RuntimeException(::fvformatf("%s", SDL_GetError()));
		}
		apiBinded = 1;
	}
}

void SDLRendererWindow::setTitle(const char *title) {
	SDL_SetWindowTitle(this->window, title);
}

const char *SDLRendererWindow::getTitle(void) const {
	return SDL_GetWindowTitle(this->window);
}

void SDLRendererWindow::resizable(bool resizable) {
	SDL_SetWindowResizable(this->window, (SDL_bool) resizable);
}

void SDLRendererWindow::setFullScreen(bool fullscreen) {
	//TODO add option for using either of the modes.
	if (fullscreen)
		SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else
		SDL_SetWindowFullscreen(this->window, 0);
}

void SDLRendererWindow::setBordered(bool bordered) {
	SDL_SetWindowBordered(this->window, (SDL_bool) bordered);
}

void SDLRendererWindow::setMinimumSize(int width, int height) {
	SDL_SetWindowMinimumSize(this->window, width, height);
}

void SDLRendererWindow::setMaximumSize(int width, int height) {
	SDL_SetWindowMaximumSize(this->window, width, height);
}

void SDLRendererWindow::focus(void) {
	SDL_SetWindowInputFocus(this->window);
}

void SDLRendererWindow::restore(void) {
	SDL_RestoreWindow(this->window);
}

void SDLRendererWindow::maximize(void) {
	SDL_MaximizeWindow(this->window);
}

void SDLRendererWindow::minimize(void) {
	SDL_MinimizeWindow(this->window);
}

intptr_t SDLRendererWindow::getNativePtr(void) const {
	SDL_SysWMinfo info;

	SDL_VERSION(&info.version); /* initialize info structure with SDL version info */

	if (SDL_GetWindowWMInfo(window, &info)) { /* the call returns true on success */
		/* success */
		switch (info.subsystem) {
			case SDL_SYSWM_UNKNOWN:
				return 0;
			case SDL_SYSWM_WINDOWS:
#if defined(SDL_VIDEO_DRIVER_WINDOWS)
				return info.info.win.window;
#endif
			case SDL_SYSWM_X11:
#if defined(SDL_VIDEO_DRIVER_X11)
				return info.info.x11.window;
#endif
#if SDL_VERSION_ATLEAST(2, 0, 3)
			case SDL_SYSWM_WINRT:
#endif
			case SDL_SYSWM_DIRECTFB:
			case SDL_SYSWM_COCOA:
			case SDL_SYSWM_UIKIT:
#if SDL_VERSION_ATLEAST(2, 0, 2)
			case SDL_SYSWM_WAYLAND:
#if defined(SDL_VIDEO_DRIVER_WAYLAND)
			return (intptr_t)info.info.wl.surface;
#endif
			case SDL_SYSWM_MIR:
#if defined(SDL_VIDEO_DRIVER_MIR)
			return (intptr_t)info.info.mir.surface;
#endif
#endif
#if SDL_VERSION_ATLEAST(2, 0, 4)
			case SDL_SYSWM_ANDROID:
#endif
#if SDL_VERSION_ATLEAST(2, 0, 5)
			case SDL_SYSWM_VIVANTE:
				break;
#endif
		}
	} else
		throw RuntimeException(fvformatf("%s", SDL_GetError()));
	throw NotImplementedException("Window format not implemented");
}

void SDLRendererWindow::setIcon(void *pVoid) {

}

void *SDLRendererWindow::getIcon(void) const {
	return nullptr;
}
