#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkInteractorStyleRubberBand2D.h>
#include <vtkGenericOpenGLRenderWindow.h>

#include <QVTKOpenGLNativeWidget.h>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include <QSlider>
#include <QPointer>
#include <QApplication>
#include <QDockWidget>
#include <QLabel>

#include <omp.h>
#include <windows.h>
#include <codecvt>

#include "LinePlot.hpp"
#include "FFT.hpp"


std::string getWindowsSoundFilePath() 
{
    wchar_t filePath[MAX_PATH] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr; // No specific owner window
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = L"Sound Files\0*.WAV;*.MP3;*.FLAC;*.AAC;*.OGG;*.M4A\0All Files\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(filePath);
    }
    
    return std::string();
}

void loadNewFile(LinePlot* plot)
{
    std::string filePath = getWindowsSoundFilePath();
    if (filePath.empty())
        return;

    auto result = FFT::getBins(filePath);
    plot->setSamples(result);
    plot->setColumns();
}

void onKeyPress(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    auto plot = static_cast<LinePlot*>(clientData);
    std::string key = static_cast<vtkRenderWindowInteractor*>(caller)->GetKeySym();
    if (key == "o") 
        loadNewFile(plot);
}

int main(int argc, char* argv[])
{
    SetProcessDPIAware(); // doesn't work?
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads() / 2);

    // Qt

    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication app(argc, argv);
    QMainWindow mw;
    mw.resize(1200, 800);

    QDockWidget controlDock;
    mw.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    QLabel controlDockTitle("Control Dock");
    controlDockTitle.setMargin(20);
    controlDock.setTitleBarWidget(&controlDockTitle); 
    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    controlDock.setWidget(&layoutContainer);

    QPushButton randomizeButton;
    randomizeButton.setText("Randomize");
    dockLayout->addWidget(&randomizeButton);

    // VTK

    QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
    mw.setCentralWidget(vtkRenderWidget);

    vtkNew<vtkGenericOpenGLRenderWindow> window;
    vtkRenderWidget->setRenderWindow(window.Get()); 

    auto plot = new LinePlot(window.Get());
    plot->setAxesNames("x1", "x2", "ch1", "ch2");
    plot->setColumns();
    plot->setTitles("Frequency (Hz)", "Magnitude");

    mw.show();
    loadNewFile(plot);

    return app.exec();
}
