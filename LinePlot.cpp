#include "LinePlot.hpp"

#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkAxis.h>

LinePlot::LinePlot(vtkContextView *view)
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

    // Add multiple line plots, setting the colors etc.
	view->GetScene()->AddItem(m_chart);

	// Set the x-axis to logarithmic scale
	m_chart->GetAxis(vtkAxis::BOTTOM)->SetLogScale(true);
}

void LinePlot::setSamples(const ResultFFT& samples) 
{
	if (samples[0].size() != samples[2].size() || samples[1].size() != samples[3].size())
		return;

	m_table->RemoveAllRows();

	// Fill in the table with some example values.
	auto num_points = std::max(samples[0].size(), samples[1].size());
	m_table->SetNumberOfRows(num_points);

	for (int i = 0; i < samples[0].size(); ++i) {
		m_table->SetValue(i, 0, samples[0][i]);
		m_table->SetValue(i, 1, samples[1][i]);
		m_table->SetValue(i, 2, samples[2][i]);
		m_table->SetValue(i, 3, samples[3][i]);
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
}

// todo ?
void LinePlot::setColumns() 
{
	m_chart->GetAxis(0)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetUnscaledMinimumLimit(20);
	m_chart->GetAxis(1)->SetUnscaledMaximumLimit(20000);
}

void LinePlot::setAxesNames(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2) 
{
	m_array_x1->SetName(x1.data());
	m_array_x2->SetName(x2.data());
	m_array_y1->SetName(y1.data());
	m_array_y2->SetName(y2.data());
}

void LinePlot::setTitles(std::string t1, std::string t2) 
{
	m_chart->GetAxis(1)->SetTitle(t1);
	m_chart->GetAxis(0)->SetTitle(t2);
}
