#include "Animation.h"
#include "Components.h"

FloatPosition Animation::LinearEase(FloatPosition a, FloatPosition b, float time) {
	return (a * (1.0 - time)) + (b * time);
}

FloatPosition Animation::SinEase(FloatPosition a, FloatPosition b, float time) {
	float val = (cos(time * M_PI) + 1) * 0.5f;
	return (a * val) + (b * (1.0 - val));
}

FloatPosition Animation::BounceEaseOut(FloatPosition a, FloatPosition b, float time) {
	float n1 = 7.5625;
	float d1 = 2.75;
	float x = time;
	float val = 0;

	if (x < 1 / d1) {
		val = n1 * x * x;
	}
	else if (x < 2 / d1) {
		val = n1 * (x -= 1.5 / d1) * x + 0.75;
	}
	else if (x < 2.5 / d1) {
		val = n1 * (x -= 2.25 / d1) * x + 0.9375;
	}
	else {
		val = n1 * (x -= 2.625 / d1) * x + 0.984375;
	}
	val = 1.0 - val;
	return (a * val) + (b * (1.0 - val));
}

FloatPosition Animation::QuadEaseOut(FloatPosition a, FloatPosition b, float time) {
	float val = 1 - (1 - time) * (1 - time);
	val = 1.0 - val;
	return (a * val) + (b * (1.0 - val));
}