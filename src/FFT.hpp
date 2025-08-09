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

// Skips DC and N/2
struct STFTResult {
	int channels;
	int windows;
	int bins;
	std::vector<float> time;								// time[w]
	std::vector<float> frequency;							// freq[f] (bins - 1)
	// std::vector<std::vector<std::vector<float>>> magnitude; // magnitude[ch][w][f]
	std::vector<float> magnitude; // magnitude[(ch * windows + w) * (bins - 1) + f]
	STFTResult(int ch_num, int w, int bins)
		: channels(ch_num),
		  windows(w),
		  bins(bins),
		  time(w),
		  frequency(bins - 1),
		  magnitude(ch_num * w * (bins - 1)) {}
	int magIndex(int ch, int w, int f) const { return (ch * windows + w) * (bins - 1) + f; }
};

class FFT {
public:
	static std::shared_ptr<FFTResult> getBins(AudioFile* audio);
	static std::shared_ptr<STFTResult> getShortTimeFFT(AudioFile *audio, double window_sec, double overlap);
};
