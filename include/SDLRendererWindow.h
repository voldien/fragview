#ifndef FRAGVIEW_SDLRENDERERWINDOW_H_
#define FRAGVIEW_SDLRENDERERWINDOW_H_ 1
#include <Renderer/RendererWindow.h>
#include <SDL2/SDL_video.h>

/**
 * SDL Window.
 */
class FVDECLSPEC SDLRendererWindow : public fragcore::RendererWindow {
public:
	virtual ~SDLRendererWindow(void);

	void closeWindow(void) override;

	virtual void showWindow(void);

	virtual void hideWindow(void);

	void focus(void) override;

	void restore(void) override;

	void maximize(void) override;

	void minimize(void) override;

	virtual void swapBuffer(void);

	virtual void setPosition(int x, int y);

	virtual void setSize(int width, int height);

	virtual void vsync(bool state) override;

	virtual void getPosition(int *x, int *y) const;

	virtual void getSize(int *width, int *height) const;

	void resizable(bool resizable) override;


	void setIcon(void *pVoid) override;

	void *getIcon(void) const override;

	virtual fragcore::Display* getCurrentDisplay(void) const;

	void setFullScreen(bool fullscreen) override;

	bool isFullScreen(void) const;

	void setBordered(bool bordered) override;

	void setMinimumSize(int width, int height) override;

	void setMaximumSize(int width, int height) override;

	virtual float getGamma(void) const;

	virtual void setGamma(float);

	virtual void setTitle(const char *title);
	virtual const char *getTitle(void) const;

	intptr_t getNativePtr(void) const override;

protected:
	virtual void createWindow(int x, int y, int width, int height, const char *api) override;
	virtual bool assertConfigAttributes(const fragcore::IConfig *iConfig) override;
	virtual void useWindow(void *pdata) override;

private:
	SDL_Window *window;
	unsigned int apiBinded;
};

#endif