//
// Created by dreph on 10/03/2024.
//

#include "LinePlot.h"

#include <vtkPlot.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkAxis.h>

LinePlot::LinePlot(vtkGenericOpenGLRenderWindow *window)
{
	m_table->AddColumn(m_array_x1);
	m_table->AddColumn(m_array_x2);
	m_table->AddColumn(m_array_y1);
	m_table->AddColumn(m_array_y2);

	// Arrays need to have always some name
	m_array_x1->SetName("x1");
	m_array_x2->SetName("x2");
	m_array_y1->SetName("y1");
	m_array_y2->SetName("y2");


	// Set up the view
	m_view->GetRenderWindow()->SetWindowName("LinePlot");
	m_view->GetRenderer()->SetBackground(50/255.0,50/255.0,50/255.0);

    // Add multiple line plots, setting the colors etc.
	m_view->GetScene()->AddItem(m_chart);

	// Add view to VTK window, which is an interface to Qt.
	m_view->SetRenderWindow(window);

	// Set the x-axis to logarithmic scale
	m_chart->GetAxis(vtkAxis::BOTTOM)->SetLogScale(true);

	// For dotted line, the line type can be from 2 to 5 for different dash/dot
	// patterns (see enum in vtkPen containing DASH_LINE, value 2):
	// #ifndef WIN32
	//   line->GetPen()->SetLineType(vtkPen::DASH_LINE);
	// #endif
	// (ifdef-ed out on Windows because DASH_LINE does not work on Windows
	//  machines with built-in Intel HD graphics card...)

	// view->GetRenderWindow()->SetMultiSamples(0);
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
		m_table->SetValue(i, 2, samples[2][i]);
	}
	for (int i = 0; i < samples[0].size(); ++i) {
		m_table->SetValue(i, 1, samples[1][i]);
		m_table->SetValue(i, 3, samples[3][i]);
	}
//	m_chart->Update();
	m_view->GetRenderWindow()->Render();
}


void LinePlot::setColumns(std::string_view x1, std::string_view x2, std::string_view y1, std::string_view y2)
{
	m_array_x1->SetName(x1.data());
	m_array_x2->SetName(x2.data());
	m_array_y1->SetName(y1.data());
	m_array_y2->SetName(y2.data());

	vtkPlot* line = m_chart->AddPlot(vtkChart::LINE);
	line->SetInputData(m_table, 0, 2);
	line->SetColor(0, 255, 0, 255);
	line->SetWidth(1.0);
	line = m_chart->AddPlot(vtkChart::LINE);
	line->SetInputData(m_table, 1, 3);
	line->SetColor(255, 0, 0, 255);
	line->SetWidth(2.0);
	m_chart->GetAxis(0)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetNumberOfTicks(5);
	m_chart->GetAxis(1)->SetUnscaledMinimumLimit(20);
	m_chart->GetAxis(1)->SetUnscaledMaximumLimit(20000);
//	m_chart->GetAxis(vtkAxis::BOTTOM)->SetRange(2000, 20000);

	m_chart->GetAxis(1)->SetTitle("Frequency (Hz)");
	m_chart->GetAxis(0)->SetTitle("Magnitude");
}
