#include <cmath>
#include "fftw3.h"

#include "FFT.hpp"
#include "ScopedStopwatch.hpp"
#include "AudioFile.hpp"

template<typename T>
std::pair<std::vector<T>, std::vector<T>> averageBins(const std::vector<T>& x, const std::vector<T>& y, size_t bin_size)
{
	std::vector<T> x_avg, y_avg;
	x_avg.resize(x.size() / bin_size);
	y_avg.resize(y.size() / bin_size);
	for (size_t i = 0; i < x.size(); i += bin_size) {
		T sumX = 0, sumY = 0;
		size_t count = std::min(bin_size, x.size() - i);
		for (size_t j = 0; j < count; ++j) {
			sumX += x[i + j];
			sumY += y[i + j];
		}
		x_avg.push_back(sumX / count);
		y_avg.push_back(sumY / count);
	}
	return {x_avg, y_avg};
}

ResultFFT FFT::getBins(AudioFile* audio)
{
	ScopedStopwatch t("getBins");

	const int channels  = audio->channels();
	const size_t frames = audio->frames();
	const size_t bins   = frames / 2;
	const double invFrames  = 1.0 / static_cast<double>(frames);
	const double freqFactor = static_cast<double>(audio->rate()) * invFrames;

	std::vector<fftwf_complex*> fft_out(channels);
	std::vector<fftwf_plan> plans(channels);

	// Split audio data into channels if necessary, and prepare fftw.
	{
		ScopedStopwatch t2("fftw_init");
		for (int ch = 0; ch < channels; ch++) {
			float *fft_in = audio->channel(ch).data();
			fft_out[ch] = fftwf_alloc_complex(bins + 1);
			plans[ch] = fftwf_plan_dft_r2c_1d(frames, fft_in, fft_out[ch], FFTW_ESTIMATE);
		}
	}
	{
		ScopedStopwatch t2("fftw_execute");
		#pragma omp parallel for schedule(static)
		for (int ch = 0; ch < channels; ch++)
			fftwf_execute(plans[ch]);
	}
	// Calculate the magnitude for each frequency bin.
	ResultFFT result;
	for (auto& v : result) v.resize(bins - 1);

	#pragma omp parallel for schedule(static)
	for (int i = 1; i < bins; i++) {
		const double freq = i * freqFactor;
		for (int ch = 0; ch < channels; ch++) {
			result[ch][i - 1] = freq;
			result[2 + ch][i - 1] = std::hypot(fft_out[ch][i][0], fft_out[ch][i][1]) * invFrames;
		}
	}

	// Clean
	{
		ScopedStopwatch cl("cleanup");
		for (auto& p : plans) fftwf_destroy_plan(p);
		for (auto& o : fft_out) fftwf_free(o);
	}
	// Limit data to draw
	constexpr int bin_size = 100;
	{
		ScopedStopwatch t2("limit data");
		auto [ch1x_avg, ch1y_avg] = averageBins(result[0], result[2], bin_size);
		auto [ch2x_avg, ch2y_avg] = averageBins(result[1], result[3], bin_size);
		return { ch1x_avg, ch2x_avg, ch1y_avg, ch2y_avg };
	}
}
