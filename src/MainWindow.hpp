#pragma once
#include "FFTLinePlot.hpp"
#include "AudioFile.hpp"

#include <QMainWindow>
#include <memory>
#include <vtkGenericOpenGLRenderWindow.h>
#include <QStackedLayout>

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
	void userOpenAudioFile();
	void computeAndRenderFFT();

private:
	void createCentralVTKWidget();
	void createControlDock();
	void wireConnections();
	void enableControls(bool enabled);
	void loadAudioFile(const QString& fn);
	void updateAudioLabels();

	// Plot
	QVTKOpenGLNativeWidget* m_vtkWidget{nullptr};
	vtkNew<vtkGenericOpenGLRenderWindow> m_renderWindow;
	std::unique_ptr<FFTLinePlot> m_plot;

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
	QString m_audioFilePath;
};
