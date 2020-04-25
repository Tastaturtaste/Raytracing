#pragma once


struct RenderParams {
	unsigned int numUSamples{ 4 };
	unsigned int numVSamples{ 4 };
	bool preview{ false };
	unsigned int max_depth{ 2 };
	unsigned int samplesPerPixel{ 10 };
};