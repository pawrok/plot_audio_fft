#pragma once

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkTable.h>
#include <vtkChartXY.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include "FFT.hpp"

class LinePlot {
public:
	explicit LinePlot(vtkGenericOpenGLRenderWindow* window);
	void setColumns();
	void setSamples(const ResultFFT& samples);
	void setAxesNames(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2);
	void setTitles(std::string_view t1, std::string_view t2);

private:
	vtkNew<vtkTable> m_table;
	vtkNew<vtkFloatArray> m_array_x1;
	vtkNew<vtkFloatArray> m_array_x2;
	vtkNew<vtkFloatArray> m_array_y1;
	vtkNew<vtkFloatArray> m_array_y2;
	vtkNew<vtkChartXY> m_chart;
	vtkNew<vtkContextView> m_view;
};
