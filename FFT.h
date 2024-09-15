//
// Created by dreph on 17/03/2024.
//

#ifndef CLION_TEST_FFT_H
#define CLION_TEST_FFT_H

#include <fftw3.h>
#include <sndfile.h>

#include <string_view>
#include <vector>
#include <array>

// x1[], x2[] y1[], y2[]
typedef std::array<std::vector<double>, 4> ResultFFT;

class FFT
{
public:
	static ResultFFT getBins(std::string_view file_name);

private:
};


#endif //CLION_TEST_FFT_H
