#pragma once
#include "Plot2D.hpp"
#include "RangeSlider.hpp"
#include <QMainWindow>
#include <memory>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QStackedLayout>

#include "AudioFile.hpp"

class QVTKOpenGLNativeWidget;
class QLabel;
class QPushButton;
class QSlider;
class QDoubleSpinBox;
class QProgressBar;

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow(QWidget *parent = nullptr);
	~MainWindow() override = default;

private slots:
	void loadAudioFile();
	void computeAndRenderFFT();

private:
	void createCentralVTKWidget();
	void createControlDock();
	void wireConnections();
	void enableControls(bool enabled);

	// Plot
	QVTKOpenGLNativeWidget* m_vtkWidget{nullptr};
	vtkNew<vtkGenericOpenGLRenderWindow> m_renderWindow;
	std::unique_ptr<LinePlot> m_plot;

	// UI
	QPushButton*     m_btnLoad{nullptr};
	QDoubleSpinBox*  m_spinStart{nullptr};
	QDoubleSpinBox*  m_spinLen{nullptr};
	QLabel*          m_lblFile{nullptr};
	QProgressBar*    m_progress{nullptr};
	QLabel*          m_lblDuration {};
	QStackedLayout*  m_stackLay {};

	// other
	double           m_totalSec      {0.0};

	std::unique_ptr<AudioFile> m_audioFile;
};
