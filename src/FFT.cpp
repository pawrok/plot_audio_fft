#include "fftw3.h"
#include "FFT.hpp"
#include "settings.hpp"
#include "ScopedStopwatch.hpp"
#include "AudioFile.hpp"

#include <qmath.h>
#include <cmath>

extern GlobalSettings SETTINGS;

void average_bins(const std::vector<double>& x, const std::vector<double>& y, size_t bin_size,
				  std::vector<double>& x_out, std::vector<double>& y_out)
{
	int out_idx = 0;
	for (size_t i = 0; i < x.size(); i += bin_size) {
		double sumX = 0, sumY = 0;
		size_t count = std::min(bin_size, x.size() - i);
		for (size_t j = 0; j < count; ++j) {
			sumX += x[i + j];
			sumY += y[i + j];
		}
		x_out[out_idx] = sumX / count;
		y_out[out_idx] = sumY / count;
		++out_idx;
	}
}

void apply_hann_window(std::span<float> data)
{
	ScopedStopwatch t2("hann window");
	const size_t len = data.size();
	for (int n = 0; n < len; ++n)
		data[n] *= 0.5 * (1.0f - std::cos(2 * M_PI * n / (len - 1)));
}

std::shared_ptr<FFTResult> FFT::getBins(AudioFile* audio)
{
	ScopedStopwatch t("getBins");

	const int channels  = audio->channels();
	const sf_count_t frames = audio->frames();
	const size_t bins   = frames / 2;
	const double invFrames  = 1.0 / static_cast<double>(frames);
	const double freqFactor = static_cast<double>(audio->rate()) * invFrames;

	std::vector<fftwf_complex*> fft_out(channels);
	std::vector<fftwf_plan> plans(channels);

	// Split audio data into channels if necessary, and prepare fftw.
	for (int ch = 0; ch < channels; ch++) {
		if (SETTINGS.apply_hann_window) {
			apply_hann_window(audio->channel(ch));
		}
		float *fft_in = audio->channel(ch).data();
		fft_out[ch] = fftwf_alloc_complex(bins + 1);
		plans[ch] = fftwf_plan_dft_r2c_1d(frames, fft_in, fft_out[ch], FFTW_ESTIMATE);
	}

	ScopedStopwatch fft_exec("fft_exec");
	#pragma omp parallel for schedule(static)
	for (int ch = 0; ch < channels; ch++)
		fftwf_execute(plans[ch]);

	fft_exec.stop();

	// Calculate the magnitude for each frequency bin.
	ScopedStopwatch ss_calc("calc");
	FFTResult result(bins - 1);

	#pragma omp parallel for schedule(static)
	for (int i = 1; i < bins; i++) {
		const double freq = i * freqFactor;
		for (int ch = 0; ch < channels; ch++) {
			const float re = fft_out[ch][i][0];
			const float im = fft_out[ch][i][1];
			const float mag = std::hypot(re, im) * invFrames;
			const float mag_db = 20.0f * std::log10(mag + 1e-20f); // dB, safe log

			result.frequency[ch][i - 1] = freq;
			result.magnitude[ch][i - 1] = mag_db;
		}
	}
	ss_calc.stop();

	// Clean
	for (auto& p : plans) fftwf_destroy_plan(p);
	for (auto& o : fft_out) fftwf_free(o);

	// Limit data to draw
	ScopedStopwatch t2("limit data");
	auto result_avg = std::make_shared<FFTResult>(result.size / SETTINGS.fft_bin_size + 1);
	average_bins(result.frequency[0], result.magnitude[0], SETTINGS.fft_bin_size,
			result_avg->frequency[0], result_avg->magnitude[0]);
	average_bins(result.frequency[1], result.magnitude[1], SETTINGS.fft_bin_size,
			result_avg->frequency[1], result_avg->magnitude[1]);
	return result_avg;
}
