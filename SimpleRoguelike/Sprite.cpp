#include <stdio.h>

#include "Sprite.h"

Sprite::Sprite() {
	index = 0;
}
Sprite::Sprite(int i) {
	index = i;
}
Sprite::~Sprite() {

}

int Sprite::GetIndex() {
	return index;
}

void Sprite::SetIndex(int i) {
	index = i;
}