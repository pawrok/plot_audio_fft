#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>
#include <QFileDialog>

#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <cmath>
#include <omp.h>

#include "LinePlot.hpp"
#include "FFT.hpp"


int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    // Main window.
    QMainWindow* mainWindow = new QMainWindow();
    mainWindow->resize(1200, 900);

    // Control area.
    QDockWidget controlDock;
    mainWindow->addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    QLabel controlDockTitle("Control Dock");
    controlDockTitle.setMargin(20);
    controlDock.setTitleBarWidget(&controlDockTitle);

    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    controlDock.setWidget(&layoutContainer);

    auto btn = new QPushButton("Open file");
    dockLayout->addWidget(btn);

	auto test = qApp;
	if (Q_UNLIKELY(!qApp))
		std::cout << "halo halo!\n";
	else
		std::cout << "halo OK\n";

	vtkNew<vtkGenericOpenGLRenderWindow> window;

	// Render area.
    QVTKOpenGLNativeWidget vtkRenderWidget(window.GetPointer(), &layoutContainer); // = new QVTKOpenGLNativeWidget(&mainWindow);
    mainWindow->setCentralWidget(&vtkRenderWidget);

	auto plot = new LinePlot(window.Get());
	// Some defaults.
	plot->setAxesNames("x1", "x2", "ch1", "ch2");
	plot->setColumns();
	plot->setTitles("Frequency (Hz)", "Magnitude");

	QObject::connect(btn, &QPushButton::clicked, [&](){
		QString filePath = QFileDialog::getOpenFileName(mainWindow, "Open File", "", "All Files (*)");
		if (!filePath.isEmpty()) {
			QApplication::setOverrideCursor(Qt::WaitCursor);
			auto r = FFT::getBins(filePath.toStdString());
			plot->setSamples(r);
			plot->setColumns();
			QApplication::restoreOverrideCursor();
		}
	});

	fftw_init_threads();
	fftw_plan_with_nthreads(omp_get_max_threads() / 2);

	mainWindow->show();

    return app.exec();
}
