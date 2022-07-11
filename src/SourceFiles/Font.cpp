#include "Font.h"

#include <iostream>

using namespace std;

Font::Font() :
	font_(nullptr) {
}

Font::Font(const string& fileName, int size) {
	load(fileName, size);
}

Font::~Font() {
	close();
}

bool Font::load(const string& fileName, int size) {
	font_ = TTF_OpenFont(fileName.c_str(), size);
	if (font_ == nullptr) {
		throw std::runtime_error("Couldn't load font: " + fileName);
	}
	return font_;
}

void Font::close() {
	if (font_) {
		TTF_CloseFont(font_);
		font_ = nullptr;
	}
}

SDL_Surface* Font::renderText(const string& text, SDL_Color color, Uint32 wrapLength) const {
	if (font_) {
		return TTF_RenderText_Blended_Wrapped(font_, text.c_str(), color, wrapLength);
	}
	else {
		return nullptr;
	}
}