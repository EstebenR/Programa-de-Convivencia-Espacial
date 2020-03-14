#include "Health.h"
#include <iostream>

Health::Health(int l) : Component(ComponentType::Health)
{
	lives_ = livesMax_ = l;
}

Health::~Health()
{
}

bool Health::subtractLife(int damage)
{
	lives_ -= damage;

	if (lives_ > 0) return true;
	else return false;
}

void Health::addLife(int sum)
{
	if (lives_ + sum > livesMax_) lives_ = livesMax_;
	else lives_ += sum;
}