#pragma once

class Filter
{
public:
	Filter() {
		// w[0] = 0; w[1] = 0, w[2] = 0;
	};
	float update(float x) {
		w[0] = x - a[1] * w[1] - a[2] * w[2];
		x = b[0] * w[0] + b[1] * w[1] + b[2] * w[2];
		w[2] = w[1];
		w[1] = w[0];
		return x;
	};
	float w[3] = {0, 0, 0};
	float a[3] = {1.0000,   -1.3329,    0.5053};
	float b[3] = { 0.0431,    0.0862,    0.0431};
};
