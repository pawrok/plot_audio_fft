#include <fftw3.h>
#include <QVTKOpenGLNativeWidget.h>
#include <QApplication>
#include <QStyleFactory>
#include <omp.h>

#include "MainWindow.hpp"

int main(int argc, char* argv[])
{
    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads() / 2);

    // GUI
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());
    QApplication app(argc, argv);
    QApplication::setStyle(QStyleFactory::create("Fusion"));
    MainWindow mw;
    mw.resize(1200, 800);
    mw.show();

    return QApplication::exec();
}
