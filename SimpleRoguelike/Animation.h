#pragma once

class Animation
{
public:

	static struct FloatPosition LinearEase(struct FloatPosition a, struct FloatPosition b, float time);
	static struct FloatPosition SinEase(struct FloatPosition a, struct FloatPosition b, float time);
	static struct FloatPosition BounceEaseOut(struct FloatPosition a, struct FloatPosition b, float time);
	static struct FloatPosition QuadEaseOut(struct FloatPosition a, struct FloatPosition b, float time);
};

