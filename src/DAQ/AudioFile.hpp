#pragma once
#include <memory>
#include <string_view>
#include <sndfile.h>

// Loads audio file using libsndfile
class AudioFile {
public:
	explicit AudioFile(std::string_view file_name)
		: file_name_(file_name) {
		load(file_name_);
	}
	~AudioFile() {
		if (file_)
			sf_close(file_);
	}

	void load(std::string_view file_name);
	unsigned getFrameCount() const { return info_.frames; }
	unsigned getChannelCount() const { return info_.channels; }
	unsigned getSampleRate() const { return info_.samplerate; }
	double* getChannelData(unsigned ch);

private:
	std::unique_ptr<double[]> samples_;
	std::string file_name_;
	sf_private_tag* file_ = nullptr;
	SF_INFO info_;
};
