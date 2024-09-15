//
// Created by dreph on 17/03/2024.
//

#include "FFT.h"

#include <cmath>
#include <iostream>


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
	ResultFFT result;

	// Load audio file using libsndfile
	SNDFILE *file;
	SF_INFO info;
	file = sf_open(file_name.data(), SFM_READ, &info);
	if (!file)
		throw std::runtime_error("Error opening audio file.");

	auto num_frames = info.frames;
	auto num_channels = info.channels;
	auto sample_rate = info.samplerate;

	// Allocate memory for audio data and FFTW input/output arrays
	auto audio_data = new double[num_frames * num_channels];
	auto fft_in = fftw_alloc_real(num_frames);
	auto fft_out = fftw_alloc_complex(num_frames / 2 + 1);

	// Read audio data from file
	int num_frames_read = sf_readf_double(file, audio_data, num_frames);
	if (num_frames_read != num_frames)
		throw std::runtime_error("Error reading audio data.");

	// Close audio file
	sf_close(file);

	for (auto& a : result)
		a.reserve(num_frames);

	// Split audio data into channels if necessary
	for (int channel = 0; channel < num_channels; channel++) {
		// Copy channel data to FFTW input array
		for (int i = 0; i < num_frames; i++)
			fft_in[i] = audio_data[i * num_channels + channel];

		// Create FFTW plan for FFT
		fftw_plan plan = fftw_plan_dft_r2c_1d(num_frames, fft_in, fft_out, FFTW_ESTIMATE);

		// Execute FFT
		fftw_execute(plan);

		// Calculate magnitude for each frequency bin
		for (int i = 1; i < num_frames / 2; i++) {
			double magnitude = 2.0 * std::sqrt(pow(fft_out[i][0], 2) + pow(fft_out[i][1], 2)) / num_frames;
			double frequency = double(i) / num_frames * sample_rate;
			result[0 + channel].push_back(frequency);
			result[2 + channel].push_back(magnitude);
		}

		// Destroy FFTW plan
		fftw_destroy_plan(plan);
	}

	fftw_free(fft_in);
	fftw_free(fft_out);
	delete[] audio_data;

	int bin_size = 100;
	auto b1 = averageBins(result[0], result[2], bin_size);
	auto b2 = averageBins(result[1], result[3], bin_size);

	return { b1.first, b2.first, b1.second, b2.second };

//	return result;
}