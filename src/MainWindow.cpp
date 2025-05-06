#include "MainWindow.hpp"
#include "FFT.hpp"

#include <qcoreapplication.h>
#include <QVTKOpenGLNativeWidget.h>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>
#include <sndfile.h>

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
    m_vtkWidget = new QVTKOpenGLNativeWidget();
    setCentralWidget(m_vtkWidget);
    m_vtkWidget->setRenderWindow(m_renderWindow.Get());
    m_plot = std::make_unique<LinePlot>(m_renderWindow.Get());
    m_plot->setAxesNames("x1", "x2", "ch1", "ch2");
    m_plot->setColumns();
    m_plot->setTitles("Frequency (Hz)", "Magnitude");
}

void MainWindow::createControlDock()
{
    auto* dock = new QDockWidget(this);
    dock->setAllowedAreas(Qt::TopDockWidgetArea);
    dock->setFeatures(QDockWidget::NoDockWidgetFeatures);
    dock->setTitleBarWidget(new QWidget);

    m_btnLoad = new QPushButton(tr("Load Audio…"));
    m_progress = new QProgressBar;
    m_progress->setRange(0, 0);

    m_stackLay = new QStackedLayout;
    m_stackLay->addWidget(m_btnLoad);    // index 0
    m_stackLay->addWidget(m_progress);   // index 1

    auto* loader = new QWidget;
    loader->setLayout(m_stackLay);

    m_lblFile     = new QLabel(tr("<i>No file loaded</i>"));
    m_lblFile->setWordWrap(true);
    m_lblDuration = new QLabel(tr("Total: 0.000 s"));
    m_range       = new RangeSlider;

    auto* row1 = new QHBoxLayout;
    row1->addWidget(loader);
    row1->addWidget(m_lblFile, 1);
    row1->addWidget(m_lblDuration);

    auto* lay = new QVBoxLayout;
    lay->addLayout(row1);
    lay->addWidget(m_range);

    auto* w = new QWidget;
    w->setLayout(lay);
    dock->setWidget(w);
    addDockWidget(Qt::TopDockWidgetArea, dock);
    dock->setFixedHeight(dock->sizeHint().height());

    enableControls(false);
}

void MainWindow::wireConnections()
{
    connect(m_btnLoad, &QPushButton::clicked, this, &MainWindow::loadAudioFile);
    connect(m_range,   &RangeSlider::valuesChanged, this, &MainWindow::rangeChanged);
}

void MainWindow::enableControls(bool enabled)
{
    m_range->setEnabled(enabled);
}

void MainWindow::loadAudioFile()
{
    const QString fn = QFileDialog::getOpenFileName(this, tr("Open audio"), {}, tr("Audio files (*.wav *.flac *.ogg *.mp3)"));
    if (fn.isEmpty()) return;

    m_stackLay->setCurrentIndex(1);
    qApp->processEvents();

    m_audioFile.reset(new AudioFile(fn.toStdString()));

    SF_INFO info {};
    if (SNDFILE* f = sf_open(fn.toStdString().c_str(), SFM_READ, &info)) {
        m_totalSec = static_cast<double>(info.frames) / info.samplerate;
        sf_close(f);
    } else m_totalSec = 0.0;

    m_lblDuration->setText(
        tr("Total: %1 s").arg(QString::number(m_totalSec, 'f', 3)));
    m_lblFile->setText(tr("File loaded: %1").arg(fn));

    computeAndRenderFFT();

    m_lblDuration->setText(tr("Track duration: %1 s").arg(QString::number(m_totalSec, 'f', 3)));
    m_lblFile->setText(tr("File loaded: %1").arg(fn));

    m_stackLay->setCurrentIndex(0);
}

void MainWindow::computeAndRenderFFT()
{
    const auto result = FFT::getBins(m_audioFile.get());
    if (!result[0].empty() && !result[2].empty()) {
        m_plot->setSamples(result);
        m_renderWindow->Render();
        enableControls(true);
    }
}

void MainWindow::rangeChanged(int lo, int hi)
{
    if (m_totalSec <= 0.0) return;
    m_segStartSec = (lo / 1000.0) * m_totalSec;
    m_segLenSec   = ((hi - lo) / 1000.0) * m_totalSec;
    // computeAndRenderFFT();
}