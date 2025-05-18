#pragma once
#include <string_view>
#include <sndfile.h>
#include <vector>
#include <span>

// Loads audio file using libsndfile
class AudioFile {
public:
	explicit AudioFile(std::string_view path);

	// Access
	std::span<float> channel(size_t idx);
	std::span<const float> channel(size_t idx) const;

	// Meta-data
	sf_count_t frames() const noexcept { return m_info.frames; }
	int channels() const noexcept { return m_info.channels; }
	int rate() const noexcept { return m_info.samplerate; }

private:
	SF_INFO m_info{};
    std::vector<std::vector<float>> m_channels;
};
