#pragma once
#include <fftw3.h>

#include <string_view>
#include <vector>
#include <array>

// x1[], x2[] y1[], y2[]
typedef std::array<std::vector<double>, 4> ResultFFT;

class FFT
{
public:
	static ResultFFT getBins(std::string_view file_name);
};
