#include <stdexcept>
#include "AudioFile.hpp"
#include <memory>

using sf_handle = std::unique_ptr<SNDFILE, decltype(&sf_close)>;

AudioFile::AudioFile(std::string_view path) {
	SNDFILE* raw = sf_open(path.data(), SFM_READ, &m_info);
	if (!raw)
		throw std::runtime_error(std::string("AudioFile: cannot open \"") + path.data() + "\": " + sf_strerror(raw));
	const auto file = sf_handle(raw, sf_close);

	std::vector<float> tmp(static_cast<size_t>(m_info.frames) * m_info.channels);
	const sf_count_t nread = sf_readf_float(file.get(), tmp.data(), m_info.frames);
	if (nread != m_info.frames)
		throw std::runtime_error("AudioFile: short read.");

	// allocate per-channel storage
	m_channels.resize(m_info.channels, std::vector<float>(m_info.frames));

	// de-interleave
	for (sf_count_t frame = 0; frame < m_info.frames; ++frame)
		for (int ch = 0; ch < m_info.channels; ++ch)
			m_channels[ch][frame] = tmp[frame * m_info.channels + ch];
}

std::span<float> AudioFile::channel(size_t idx)
{
	if (idx >= m_channels.size())
		throw std::out_of_range("AudioFile::channel(): bad channel index");
	return m_channels[idx];
}

std::span<const float> AudioFile::channel(size_t idx) const
{
	if (idx >= m_channels.size())
		throw std::out_of_range("AudioFile::channel(): bad channel index");
	return m_channels[idx];
}

