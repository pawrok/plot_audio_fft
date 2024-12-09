#include <stdexcept>
#include "AudioFile.hpp"

void AudioFile::load(std::string_view file_name) {
	if (file_) {
		sf_close(file_);
		file_ = nullptr;
	}

	file_ = sf_open(file_name.data(), SFM_READ, &info_);
	if (!file_) {
		std::string err = sf_strerror(file_);
		throw std::runtime_error("Error opening audio file." + err);
	}

	// todo: catch allocation failure? files can be gigabyte big!
	auto audio_data = new double[info_.channels * info_.frames];

	auto frames_read = sf_readf_double(file_, audio_data, info_.frames);
	if (frames_read != info_.frames)
		throw std::runtime_error("Error reading audio data.");

	samples_.reset(audio_data);
}

// Channels counted from 0
double *AudioFile::getChannelData(unsigned int ch) {
	if (ch >= info_.channels)
		throw std::runtime_error("Wrong channel requested.");

	return samples_.get() + info_.frames * ch;
}
