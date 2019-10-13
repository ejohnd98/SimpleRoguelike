#include <stdio.h>

#include "Sprite.h"


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