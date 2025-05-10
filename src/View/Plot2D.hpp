#pragma once
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkTable.h>
#include <vtkChartXY.h>
#include <vtkContextView.h>
#include <vtkFloatArray.h>
#include <vtkNew.h>
#include "FFT.hpp"
#include "CustomChartXY.hpp""
#include <vtkContextMouseEvent.h>
#include <vtkSmartPointer.h>

class LinePlot {
public:
	explicit LinePlot(vtkGenericOpenGLRenderWindow* window);
	void setColumns();
	void setSamples(const ResultFFT& samples);
	void setAxesNames(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2);
	void setTitles(std::string t1, std::string t2);
	void resetCamera();

private:
	vtkNew<vtkTable> m_table;
	vtkNew<vtkFloatArray> m_array_x1;
	vtkNew<vtkFloatArray> m_array_x2;
	vtkNew<vtkFloatArray> m_array_y1;
	vtkNew<vtkFloatArray> m_array_y2;
	vtkNew<CustomChartXY> m_chart;
	vtkNew<vtkContextView> m_view;
};
