#include "Transform.h"

void Transform::init() {
	SDL_Rect collRect = coll_->getRectRender();

	renderRect_ = SDL_Rect{
		//Primero alineamos los centros de los sprites, y luego les a�adimos el offset que teniamos
		(int)(-(renderRect_.w - collRect.w) * 0.5f - renderRect_.x), (int)(-(renderRect_.h - collRect.h) * 0.5f - renderRect_.y),
		renderRect_.w, renderRect_.h
	};
}