#include "Animation.h"
#include "Components.h"

FloatPosition Animation::LinearEase(FloatPosition a, FloatPosition b, float time) {
	return (a * (1.0 - time)) + (b * time);
}

FloatPosition Animation::SmoothEase(FloatPosition a, FloatPosition b, float time) {
	float val = (cos(time * M_PI) + 1) * 0.5f;
	return (a * val) + (b * (1.0 - val));
}