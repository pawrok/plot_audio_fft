#include "MainWindow.hpp"

#include <QVTKOpenGLNativeWidget.h>
#include <QDockWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QSlider>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFileDialog>
#include <QProgressBar>

#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkRenderer.h>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    createCentralVTKWidget();
    createControlDock();
    wireConnections();
    resize(1200, 800);
}

void MainWindow::createCentralVTKWidget()
{
    m_vtkWidget = new QVTKOpenGLNativeWidget(this);
    setCentralWidget(m_vtkWidget);
    m_vtkWidget->setRenderWindow(m_renderWindow.Get());
}

void MainWindow::createControlDock()
{
    auto* dock   = new QDockWidget(tr("Controls"), this);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);

    // --- load file --------------------------------------------------------
    m_btnLoad    = new QPushButton(tr("Load Audio…"));
    m_lblFile    = new QLabel(tr("<i>No file loaded</i>"));
    m_lblFile->setWordWrap(true);

    // --- start & length ----------------------------------------------------
    m_spinStart  = new QDoubleSpinBox;
    m_spinStart->setPrefix("Start: ");
    m_spinStart->setSuffix(" s");
    m_spinStart->setRange(0.0, 1e6);
    m_spinStart->setDecimals(3);

    m_spinLen    = new QDoubleSpinBox;
    m_spinLen->setPrefix("Length: ");
    m_spinLen->setSuffix(" s");
    m_spinLen->setRange(0.01, 3600.0);       // min 10 ms, max 1 h
    m_spinLen->setDecimals(3);
    m_spinLen->setValue(20.0);

    // --- slider (global position) -----------------------------------------
    m_slider     = new QSlider(Qt::Horizontal);
    m_slider->setEnabled(false);

    // --- progress bar ------------------------------------------------------
    m_progress   = new QProgressBar;
    m_progress->setRange(0, 0);  // busy state by default
    m_progress->setVisible(false);

    // --- layout ------------------------------------------------------------
    auto* lay    = new QVBoxLayout;
    lay->addWidget(m_btnLoad);
    lay->addWidget(m_lblFile);
    lay->addSpacing(8);
    lay->addWidget(m_spinStart);
    lay->addWidget(m_spinLen);
    lay->addWidget(m_slider);
    lay->addWidget(m_progress);
    lay->addStretch(1);

    auto* w = new QWidget;
    w->setLayout(lay);
    dock->setWidget(w);
    addDockWidget(Qt::LeftDockWidgetArea, dock);

    enableControls(false);
}

void MainWindow::wireConnections()
{
    connect(m_btnLoad,  &QPushButton::clicked,          this, &MainWindow::loadAudioFile);
    connect(m_spinStart,&QDoubleSpinBox::valueChanged,  this, &MainWindow::segmentStartChanged);
    connect(m_spinLen,  &QDoubleSpinBox::valueChanged,  this, &MainWindow::segmentLengthChanged);
    connect(m_slider,   &QSlider::sliderMoved,          this, &MainWindow::sliderMoved);
}

void MainWindow::enableControls(bool enabled)
{
    m_spinStart->setEnabled(enabled);
    m_spinLen->setEnabled(enabled);
    m_slider->setEnabled(enabled);
}

void MainWindow::loadAudioFile()
{
    const QString fn = QFileDialog::getOpenFileName(this, tr("Open audio"), {}, tr("Audio files (*.wav *.flac *.ogg *.mp3)"));
    if (fn.isEmpty())
        return;

    // show busy
    m_progress->setVisible(true);
    m_lblFile->setText(tr("Loading …"));

    // async load with libsndfile (or your own wrapper)
    // QtConcurrent::run([this, fn]{
    //     // ---- your code: open with sndfile, decode to float mono -----------------
    //     // fill m_currentSamples, discover durationSec
    //     double durationSec = 0.0; // placeholder
    //     // -------------------------------------------------------------------------
    //
    //     // after done, switch back to the UI thread
    //     QMetaObject::invokeMethod(this, [this, fn, durationSec]() {
    //         m_currentFile = fn;
    //         m_lblFile->setText(fn);
    //         m_slider->setMaximum(static_cast<int>(durationSec * 1000)); // slider in milliseconds
    //         enableControls(true);
    //         m_progress->setVisible(false);
    //         computeAndRenderFFT();
    //     });
    // });
}

void MainWindow::segmentStartChanged(double)          { computeAndRenderFFT(); }
void MainWindow::segmentLengthChanged(double)         { computeAndRenderFFT(); }

void MainWindow::sliderMoved(int posMs)
{
    m_spinStart->blockSignals(true);
    m_spinStart->setValue(posMs / 1000.0);
    m_spinStart->blockSignals(false);
    computeAndRenderFFT();
}

void MainWindow::computeAndRenderFFT()
{
    if (m_currentSamples.empty()) return;

    const double startSec = m_spinStart->value();
    const double lenSec   = m_spinLen->value();

    // -------- (a) extract fragment -----------------------------------------
    // figure out sample range, copy to tmp buffer
    // -------- (b) run FFT using fftw3 --------------------------------------
    // -------- (c) pipe magnitude array into VTK ----------------------------
    // (You’ll probably build a vtkImageData or vtkPolyData and
    //  attach it to an existing vtkActor in m_vtkWidget’s renderer.)
    // Finally call:
    //   m_vtkWidget->renderWindow()->Render();
}
