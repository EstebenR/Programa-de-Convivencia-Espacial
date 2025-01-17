#include "Texture.h"
#include <SDL_image.h>
#include "MultiplayerHost.h"
#include "SDL_Game.h"
#include <iostream>

using namespace std;

Texture::Texture() :
	texture_(nullptr), renderer_(nullptr), width_(0), height_(0), frameWidth_(0), frameHeight_(0) {
}

Texture::Texture(SDL_Renderer* renderer, const string& fileName, unsigned short nHorFrames, unsigned short nVerFrames, unsigned char texId) :
	texture_(nullptr), width_(0), height_(0), nHorizontalFrames_(nHorFrames), nVerticalFrames_(nVerFrames), texId_(texId) {
	loadFromImg(renderer, fileName);
}

Texture::Texture(SDL_Renderer* renderer, const string& text, const Font* font,
	const SDL_Color& color, unsigned char tag) :
	texture_(nullptr), width_(0), height_(0), texId_(tag) {
	loadFromText(renderer, text, font, color);
}

Texture::Texture(SDL_Renderer* renderer, const string& text, const Font* font,
	const SDL_Color& color) :
	texture_(nullptr), width_(0), height_(0) {
	loadFromText(renderer, text, font, color);
}

Texture::~Texture() {
	close();
}

void Texture::setColor(const SDL_Color& color)
{
	//Modulate texture
	if (texture_ != nullptr)
		SDL_SetTextureColorMod(texture_, color.r, color.g, color.b);
}

void Texture::close() {
	if (texture_ != nullptr) {
		SDL_DestroyTexture(texture_); // delete current texture
		texture_ = nullptr;
		width_ = 0;
		height_ = 0;
	}
}

bool Texture::loadFromImg(SDL_Renderer* renderer, const string& fileName) {
	SDL_Surface* surface = IMG_Load(fileName.c_str());
	if (surface != nullptr) {
		close(); // destroy current texture
		texture_ = SDL_CreateTextureFromSurface(renderer, surface);
		if (texture_ != nullptr) {
			width_ = surface->w;
			height_ = surface->h;
			frameWidth_ = width_ / nHorizontalFrames_;
			frameHeight_ = height_ / nVerticalFrames_;
		}
		SDL_FreeSurface(surface);
	}
	else {
		throw "Couldn't load image: " + fileName;
	}
	renderer_ = renderer;
	return texture_ != nullptr;
}

bool Texture::loadFromText(SDL_Renderer* renderer, const string& text, const Font* font,
	const SDL_Color& color, Uint32 wrapLength) {
	SDL_Surface* textSurface = font->renderText(text, color, wrapLength);
	if (textSurface != nullptr) {
		close();
		texture_ = SDL_CreateTextureFromSurface(renderer, textSurface);
		if (texture_ != nullptr) {
			width_ = textSurface->w;
			height_ = textSurface->h;
			frameWidth_ = width_ / nHorizontalFrames_;
			frameHeight_ = height_ / nVerticalFrames_;
		}
		SDL_FreeSurface(textSurface);
	}
	else {
		throw "Couldn't create text: " + text;
	}
	renderer_ = renderer;
	return texture_ != nullptr;
}

void Texture::render(int x, int y) const {
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = width_;
	dest.h = height_;
	render(dest);
}

void Texture::render(const SDL_Rect& dest) const {
	SDL_Rect clip = { 0, 0, width_, height_ };
	render(dest, clip);
}

void Texture::render(const SDL_Rect& dest, const SDL_Rect& clip) const {
	if (texture_) {
		SDL_RenderCopy(renderer_, texture_, &clip, &dest);
		if (SDL_Game::instance()->isHosting())
			SDL_Game::instance()->getHost()->sendTexture({ 'S',texId_, (short)dest.x,(short)dest.y, (short)dest.w, (short)dest.h });
	}
}

void Texture::render(const SDL_Rect& dest, double angle, const SDL_Rect& clip, SDL_RendererFlip flip) const
{
	if (texture_) {
		SDL_RenderCopyEx(renderer_, texture_, &clip, &dest, angle, nullptr,
			flip);
	}
}

void Texture::render(const SDL_Rect& dest, double angle, const SDL_RendererFlip& flip) const
{
	SDL_Rect clip = { 0, 0, width_, height_ };
	render(dest, angle, clip, flip);
}

void Texture::render(const SDL_Rect& dest, double angle,
	const SDL_Rect& clip) const {
	render(dest, angle, clip, SDL_FLIP_NONE);
}

// this overloaded function gets the clip for you. frameY and flip are optional parameters
void Texture::render(const SDL_Rect& dest, double angle, unsigned short frameX, unsigned short frameY, SDL_RendererFlip flip) const {
	SDL_Rect clip = { frameWidth_ * frameX, frameHeight_ * frameY, frameWidth_, frameHeight_ };
	if (SDL_Game::instance()->isHosting())
		SDL_Game::instance()->getHost()->sendTexture({ 'S',texId_,(short)dest.x,(short)dest.y, (short)dest.w, (short)dest.h, (short)angle, (unsigned char)frameX, (unsigned char)frameY, (unsigned char)flip });
	render(dest, angle, clip, flip);
}

void Texture::render(const SDL_Rect& dest, double angle) const {
	SDL_Rect clip = { 0, 0, width_, height_ };
	if (SDL_Game::instance()->isHosting())
		SDL_Game::instance()->getHost()->sendTexture({ 'S',texId_,(short)dest.x,(short)dest.y, (short)dest.w, (short)dest.h, (short)angle });
	render(dest, angle, clip);
}