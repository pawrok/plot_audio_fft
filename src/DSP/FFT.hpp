#pragma once
#include <vector>
#include <array>
#include "AudioFile.hpp"

// x1[], x2[] y1[], y2[]
typedef std::array<std::vector<double>, 4> ResultFFT;

class FFT
{
public:
	static ResultFFT getBins(AudioFile* audio);
};
