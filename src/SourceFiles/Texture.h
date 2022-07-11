#pragma once

#include <SDL.h>
#include "Font.h"
#include <string>
#include "checkML.h"


using namespace std;

class Texture {
public:
	Texture();
	Texture(SDL_Renderer* renderer, const string& fileName, unsigned short nHorFrames, unsigned short nVerFrames, unsigned char texId);
	Texture(SDL_Renderer* renderer, const string& text, const Font* font,
		const SDL_Color& color, unsigned char tag);
	Texture(SDL_Renderer* renderer, const string& text, const Font* font,
		const SDL_Color& color);

	virtual ~Texture();

	inline int getWidth() const {
		return width_;
	}

	inline int getFrameWidth() const {
		return width_ / nHorizontalFrames_;
	}

	inline int getHeight() const {
		return height_;
	}


	inline int getFrameHeight() const {
		return height_ / nVerticalFrames_;
	}

	inline bool isReady() const {
		return texture_ != nullptr;
	}

	inline unsigned short int getNumFramesX() const {
		return nHorizontalFrames_;
	}

	inline unsigned short int getNumFramesY() const {
		return nVerticalFrames_;
	}

	// load from image or text, in both cases we should provide a rendered
	bool loadFromImg(SDL_Renderer* renderer, const string& fileName);
	bool loadFromText(SDL_Renderer* renderer, const string& text, const Font* font,
		const SDL_Color& color = { 0, 0, 0, 255 }, const Uint32 = 2000);

	// render the complete texture at position (x,y).
	void render(int x, int y) const;

	// render part of the texture (clip) to a destination rectangle, if no
	// clip provided it renders all texture (scaled to dest).
	void render(const SDL_Rect& dest) const;
	void render(const SDL_Rect& dest, const SDL_Rect& clip) const;

	// render part of the texture (clip) to a destination rectangle with a rotation,
	// if no clip provided it renders all texture (scaled to dest).
	void render(const SDL_Rect& dest, double angle, const SDL_Rect& clip) const;
	void render(const SDL_Rect& dest, double angle, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
	void render(const SDL_Rect& dest, double angle, const SDL_Rect& clip, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
	void render(const SDL_Rect& dest, double angle, unsigned short frameX, unsigned short frameY = 0, SDL_RendererFlip flip = SDL_FLIP_NONE) const;
	void render(const SDL_Rect& dest, double velX, double velY, double rotVel, double angle, unsigned short frameX, unsigned short frameY = 0, SDL_RendererFlip flip = SDL_FLIP_NONE) const;

	void render(const SDL_Rect& dest, double velX, double velY, double rotVel, double angle, const SDL_Rect& clip, SDL_RendererFlip flip, unsigned short frameX = 0, unsigned short frameY = 0) const;

	//Set color modulation
	void setColor(const SDL_Color& color);

	void close();

private:
	SDL_Texture* texture_;
	SDL_Renderer* renderer_;
	int width_;
	int height_;
	unsigned short nHorizontalFrames_ = 1;
	unsigned short nVerticalFrames_ = 1;
	int frameWidth_;
	int frameHeight_;
	unsigned char texId_;
};
