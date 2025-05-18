#pragma once
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkTable.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include "../FFT.hpp"
#include "CustomChartXY.hpp"

class FFTLinePlot {
public:
	explicit FFTLinePlot(vtkGenericOpenGLRenderWindow* window);
	void setColumns();
	void setSamples(const std::shared_ptr<FFTResult>& samples);
	void setAxesNames(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2);
	void setTitles(std::string t1, std::string t2);
	void resetCamera();
	void clearPlot();

private:
	vtkNew<vtkTable> m_table;
	vtkNew<vtkFloatArray> m_array_x1;
	vtkNew<vtkFloatArray> m_array_x2;
	vtkNew<vtkFloatArray> m_array_y1;
	vtkNew<vtkFloatArray> m_array_y2;
	vtkNew<CustomChartXY> m_chart;
	vtkNew<vtkContextView> m_view;
};
