#include "MainWindow.hpp"
#include "FFT.hpp"
#include "settings.hpp"

#include <QApplication>
#include <QtConcurrent>
#include <qcoreapplication.h>
#include <QVTKOpenGLNativeWidget.h>
#include <QDockWidget>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFileDialog>
#include <QProgressBar>
#include <sndfile.h>

GlobalSettings SETTINGS;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    createCentralVTKWidget();
    createControlDock();
    wireConnections();
    resize(1200, 800);

    // for development:
    loadAudioFile(R"(D:\Nextcloud\Music\69_Desire.mp3)");
}

void MainWindow::createCentralVTKWidget()
{
    m_vtkWidget = new QVTKOpenGLNativeWidget();
    setCentralWidget(m_vtkWidget);
    m_vtkWidget->setRenderWindow(m_renderWindow.Get());
    m_plot = std::make_unique<FFTLinePlot>(m_renderWindow.Get());
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

    auto* row1 = new QHBoxLayout;
    row1->addWidget(loader);
    row1->addWidget(m_lblFile, 1);
    row1->addWidget(m_lblDuration);

    auto* lay = new QVBoxLayout;
    lay->addLayout(row1);

    auto* w = new QWidget;
    w->setLayout(lay);
    dock->setWidget(w);
    addDockWidget(Qt::TopDockWidgetArea, dock);
    dock->setFixedHeight(dock->sizeHint().height());

    enableControls(false);
}

void MainWindow::wireConnections()
{
    connect(m_btnLoad, &QPushButton::clicked, this, &MainWindow::userOpenAudioFile);
}

void MainWindow::enableControls(bool enabled)
{
    m_btnLoad->setEnabled(enabled);
}

void MainWindow::loadAudioFile(const QString &fn)
{
    m_audioFilePath = fn;
    QApplication::setOverrideCursor(Qt::BusyCursor);

    QFuture<void> future = QtConcurrent::run([this, fn]() {
        m_audioFile.reset(new AudioFile(fn.toStdString()));
        computeAndRenderFFT();
    });
}

void MainWindow::updateAudioLabels()
{
    SF_INFO info {};
    if (SNDFILE* f = sf_open(m_audioFilePath.toStdString().c_str(), SFM_READ, &info)) {
        m_totalSec = static_cast<double>(info.frames) / info.samplerate;
        sf_close(f);
    } else
        m_totalSec = 0.0;
    m_lblDuration->setText(tr("File duration: %1 s").arg(QString::number(m_totalSec, 'f', 0)));
    auto audio_name = QFileInfo(m_audioFilePath).fileName();
    m_lblFile->setText(tr("File loaded: %1").arg(audio_name));
}

void MainWindow::userOpenAudioFile()
{
    const QString fn = QFileDialog::getOpenFileName(this, tr("Open audio"), {}, tr("Audio files (*.wav *.flac *.ogg *.mp3)"));
    if (fn.isEmpty()) {
        return;
    }

    enableControls(false);
    m_stackLay->setCurrentIndex(1);
    m_lblFile->setText("Loading Audio File...");
    m_plot->clearPlot();
    qApp->processEvents();

    loadAudioFile(fn);
}

void MainWindow::computeAndRenderFFT()
{
    QMetaObject::invokeMethod(this, [this]() {
        m_lblFile->setText("Computing FFT...");
        qApp->processEvents();
    }, Qt::QueuedConnection);

    QFuture<void> future = QtConcurrent::run([this]() {
        // Calculate FFT
        const auto result = FFT::getBins(m_audioFile.get());
        // Update UI
        QMetaObject::invokeMethod(this, [this, result]() {
            m_plot->setSamples(result);
            m_renderWindow->Render();

            enableControls(true);
            m_stackLay->setCurrentIndex(0);
            updateAudioLabels();
            QApplication::restoreOverrideCursor();
        }, Qt::QueuedConnection);
    });
}
