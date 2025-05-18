#pragma once
#include <vector>
#include <array>
#include "AudioFile.hpp"
#include <memory>

// x1[], x2[] y1[], y2[]
// typedef std::array<std::vector<float>, 4> ResultFFT;

struct FFTResult {
	FFTResult() = delete;
	explicit FFTResult(int size)
			: size(size) {
		frequency[0].resize(size);
		frequency[1].resize(size);
		magnitude[0].resize(size);
		magnitude[1].resize(size);
	}
	// [left, right]
	std::array<std::vector<double>, 2> frequency;
	std::array<std::vector<double>, 2> magnitude;
	int size;
};

class FFT
{
public:
	static std::shared_ptr<FFTResult> getBins(AudioFile* audio);

private:

};
