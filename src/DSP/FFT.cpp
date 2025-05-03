#include <cmath>

#include "FFT.hpp"
#include "ScopedStopwatch.hpp"
#include "AudioFile.hpp"

template<typename T>
std::pair<std::vector<T>, std::vector<T>> averageBins(const std::vector<T>& x, const std::vector<T>& y, size_t bin_size) {
	std::vector<T> x_avg, y_avg;
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

ResultFFT FFT::getBins(std::string_view file_name)
{
	ScopedStopwatch t("getBins");
	AudioFile audio(file_name);

	std::vector<fftw_plan> plans;
	std::vector<fftw_complex*> fft_out;
	plans.resize(audio.getChannelCount());
	fft_out.resize(audio.getChannelCount());

	// Split audio data into channels if necessary
	for (int ch = 0; ch < audio.getChannelCount(); ch++) {
		double *fft_in = audio.getChannelData(ch);
		fft_out[ch] = fftw_alloc_complex(audio.getFrameCount() / 2 + 1);
		plans[ch] = fftw_plan_dft_r2c_1d(audio.getFrameCount(), fft_in, fft_out[ch], FFTW_ESTIMATE);
	}

#pragma omp parallel for
	for (auto& plan : plans)
		fftw_execute(plan);

	// Calculate the magnitude for each frequency bin
	ResultFFT result;
	for (auto& a : result)
		a.reserve(audio.getFrameCount());

#pragma omp parallel for
	for (int ch = 0; ch < audio.getChannelCount(); ch++) {
		for (int i = 1; i < audio.getFrameCount() / 2; i++) {
			double magnitude = 2.0 * std::sqrt(pow(fft_out[ch][i][0], 2) + pow(fft_out[ch][i][1], 2)) / audio.getFrameCount();
			double frequency = static_cast<double>(i) / audio.getFrameCount() * audio.getSampleRate();
			result[0 + ch].push_back(frequency);
			result[2 + ch].push_back(magnitude);
		}
	}

	for (auto& plan : plans)
		fftw_destroy_plan(plan);

	for (auto& out : fft_out)
		fftw_free(out);

	// Limit data to draw for performance reasons.
	constexpr int bin_size = 1000;
	auto [ch1x_avg, ch1y_avg] = averageBins(result[0], result[2], bin_size);
	auto [ch2x_avg, ch2y_avg] = averageBins(result[1], result[3], bin_size);

	return { ch1x_avg, ch2x_avg, ch1y_avg, ch2y_avg };
}
