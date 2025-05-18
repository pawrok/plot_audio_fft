#include <print>
#include <vtkAxis.h>
#include "FFTLinePlot.hpp"

#include <vtkRenderer.h>
#include <vtkPlot.h>
#include <vtkTextProperty.h>

FFTLinePlot::FFTLinePlot(vtkGenericOpenGLRenderWindow* window)
{
	m_table->AddColumn(m_array_x1);
	m_table->AddColumn(m_array_x2);
	m_table->AddColumn(m_array_y1);
	m_table->AddColumn(m_array_y2);

	// Arrays need to have always some name_
	m_array_x1->SetName("x1");
	m_array_x2->SetName("x2");
	m_array_y1->SetName("y1");
	m_array_y2->SetName("y2");

	// Add this plot to the window
	m_view->SetRenderWindow(window);
	m_view->GetScene()->AddItem(m_chart);
	m_view->GetRenderWindow()->SetWindowName("LinePlot");

	// Set the x-axis to a logarithmic scale
	m_chart->GetAxis(vtkAxis::BOTTOM)->SetLogScale(true);
	m_view->GetRenderer()->SetBackground(42 / 255.0, 42 / 255.0, 42 / 255.0);
}

void FFTLinePlot::setSamples(const std::shared_ptr<FFTResult>& samples)
{
	m_table->RemoveAllRows();

	// Fill in the table with some example values.
	m_table->SetNumberOfRows(samples->size);

	for (int i = 0; i < samples->size; ++i) {
		m_table->SetValue(i, 0, samples->frequency[0][i]);
		m_table->SetValue(i, 1, samples->frequency[1][i]);
		m_table->SetValue(i, 2, samples->magnitude[0][i]);
		m_table->SetValue(i, 3, samples->magnitude[1][i]);
	}

	m_chart->ClearPlots();
	m_chart->AddPlot(vtkChart::LINE);
	m_chart->AddPlot(vtkChart::LINE);
	auto plt = m_chart->GetPlot(0);
	if (plt) {
		plt->SetInputData(m_table, 0, 2);
		plt->SetColor(0, 255, 0, 255);
		plt->SetWidth(1.0);
	}
	plt = m_chart->GetPlot(1);
	if (plt) {
		plt->SetInputData(m_table, 1, 3);
		plt->SetColor(255, 0, 0, 255);
		plt->SetWidth(2.0);
	}

	m_chart->GetAxis(0)->GetPen()->SetColor(255, 255, 255, 255);
	m_chart->GetAxis(1)->GetPen()->SetColor(255, 255, 255, 255);

	auto p = m_chart->GetAxis(0)->GetLabelProperties();
	p->SetColor(1, 1, 1);
	p = m_chart->GetAxis(1)->GetLabelProperties();
	p->SetColor(1, 1, 1);

	p = m_chart->GetAxis(1)->GetTitleProperties();
	p->SetColor(1, 1, 1);
	p = m_chart->GetAxis(0)->GetTitleProperties();
	p->SetColor(1, 1, 1);

	setColumns();
	m_view->GetRenderWindow()->Render();
}

// todo ?
void FFTLinePlot::setColumns()
{
	m_chart->GetAxis(0)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetUnscaledMinimumLimit(20);
	m_chart->GetAxis(1)->SetUnscaledMaximumLimit(20000);
}

void FFTLinePlot::setAxesNames(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2)
{
	m_array_x1->SetName(x1.data());
	m_array_x2->SetName(x2.data());
	m_array_y1->SetName(y1.data());
	m_array_y2->SetName(y2.data());
}

void FFTLinePlot::setTitles(std::string t1, std::string t2)
{
	m_chart->GetAxis(1)->SetTitle(t1);
	m_chart->GetAxis(0)->SetTitle(t2);
}

void FFTLinePlot::resetCamera()
{
	m_chart->RecalculateBounds();
	std::print("reset used\n");
}

void FFTLinePlot::clearPlot()
{
	m_table->RemoveAllRows();
	m_chart->ClearPlots();
}
