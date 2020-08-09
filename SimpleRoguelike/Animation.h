#pragma once

class Animation
{
public:

	static struct FloatPosition LinearEase(struct FloatPosition a, struct FloatPosition b, float time);
	static struct FloatPosition SmoothEase(struct FloatPosition a, struct FloatPosition b, float time);

};

