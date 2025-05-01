#pragma once

#include <QMainWindow>
#include <memory>
#include <vtkGenericOpenGLRenderWindow.h>

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
	void computeAndRenderFFT();          // hook your fftw3 + VTK pipeline here

private:
	// --- helpers -----------------------------------------------------------
	void createCentralVTKWidget();
	void createControlDock();
	void wireConnections();
	void enableControls(bool enabled);

	// --- UI members --------------------------------------------------------
	QVTKOpenGLNativeWidget* m_vtkWidget{nullptr};
	vtkNew<vtkGenericOpenGLRenderWindow> m_renderWindow;

	// controls
	QPushButton*     m_btnLoad{nullptr};
	QDoubleSpinBox*  m_spinStart{nullptr};
	QDoubleSpinBox*  m_spinLen{nullptr};
	QSlider*         m_slider{nullptr};
	QLabel*          m_lblFile{nullptr};
	QProgressBar*    m_progress{nullptr};

	// --- state -------------------------------------------------------------
	QString          m_currentFile;
	std::vector<float> m_currentSamples; // 32-bit float mono buffer (for example)
};
