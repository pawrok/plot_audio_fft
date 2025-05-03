#pragma once
#include "Plot2D.hpp"
#include <QMainWindow>
#include <memory>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>

class QVTKOpenGLNativeWidget;
class QLabel;
class QPushButton;
class QSlider;
class QDoubleSpinBox;
class QProgressBar;

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;

private slots:
	void loadAudioFile();
	void segmentStartChanged(double);
	void segmentLengthChanged(double);
	void sliderMoved(int);
	void computeAndRenderFFT();

private:
	// --- helpers -----------------------------------------------------------
	void createCentralVTKWidget();
	void createControlDock();
	void wireConnections();
	void enableControls(bool enabled);

	// --- UI members --------------------------------------------------------
	QVTKOpenGLNativeWidget* m_vtkWidget{nullptr};
	vtkNew<vtkGenericOpenGLRenderWindow> m_renderWindow;
	std::unique_ptr<LinePlot> m_plot;
	// vtkNew<vtkRenderer> m_renderer;

	// controls
	QPushButton*     m_btnLoad{nullptr};
	QDoubleSpinBox*  m_spinStart{nullptr};
	QDoubleSpinBox*  m_spinLen{nullptr};
	QSlider*         m_slider{nullptr};
	QLabel*          m_lblFile{nullptr};
	QProgressBar*    m_progress{nullptr};

	// --- state -------------------------------------------------------------
	QString          m_currentFile;
};
