//
// Created by dreph on 10/03/2024.
//

#ifndef CLION_TEST_LINEPLOT_H
#define CLION_TEST_LINEPLOT_H

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkTable.h>
#include <vtkChartXY.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include "FFT.h"

class LinePlot {
public:
	explicit LinePlot(vtkGenericOpenGLRenderWindow* window);

	void setColumns(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2);
	void setSamples(const ResultFFT& samples);

private:
	// Create a table with some points in it.
	vtkNew<vtkTable> m_table;
	vtkNew<vtkFloatArray> m_array_x1;
	vtkNew<vtkFloatArray> m_array_x2;
	vtkNew<vtkFloatArray> m_array_y1;
	vtkNew<vtkFloatArray> m_array_y2;
	vtkNew<vtkChartXY> m_chart;
	vtkNew<vtkContextView> m_view;
};


#endif //CLION_TEST_LINEPLOT_H
