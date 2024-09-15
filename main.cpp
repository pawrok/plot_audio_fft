#include <QVTKOpenGLNativeWidget.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkDoubleArray.h>
#include <vtkGenericOpenGLRenderWindow.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>

#include <QApplication>
#include <QDockWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QPushButton>

#include <cmath>
#include <random>
#include <QFileDialog>

#include "LinePlot.h"
#include "FFT.h"


void openFileExplorer(QMainWindow* mw)
{
}

int main(int argc, char* argv[])
{
    QSurfaceFormat::setDefaultFormat(QVTKOpenGLNativeWidget::defaultFormat());

    QApplication app(argc, argv);

    // Main window.
    QMainWindow mainWindow;
    mainWindow.resize(1200, 900);

    // Control area.
    QDockWidget controlDock;
    mainWindow.addDockWidget(Qt::LeftDockWidgetArea, &controlDock);

    QLabel controlDockTitle("Control Dock");
    controlDockTitle.setMargin(20);
    controlDock.setTitleBarWidget(&controlDockTitle);

    QPointer<QVBoxLayout> dockLayout = new QVBoxLayout();
    QWidget layoutContainer;
    layoutContainer.setLayout(dockLayout);
    controlDock.setWidget(&layoutContainer);

    auto btn = new QPushButton("Open file");
    dockLayout->addWidget(btn);

    // Render area.
    QPointer<QVTKOpenGLNativeWidget> vtkRenderWidget = new QVTKOpenGLNativeWidget();
    mainWindow.setCentralWidget(vtkRenderWidget);

    // VTK part.
    vtkNew<vtkGenericOpenGLRenderWindow> window;
    vtkRenderWidget->setRenderWindow(window.Get());

	auto plot = new LinePlot(window.Get());
	plot->setColumns("x1", "x2", "ch1", "ch2");

	auto r = FFT::getBins("pilgrim.wav");
	plot->setSamples(r);

	QObject::connect(btn, &QPushButton::clicked, [&](){
		QString filePath = QFileDialog::getOpenFileName(&mainWindow, "Open File", "", "All Files (*)");
		if (!filePath.isEmpty()) {
			QApplication::setOverrideCursor(Qt::WaitCursor);
			auto r = FFT::getBins(filePath.toStdString());
			plot->setSamples(r);
			QApplication::restoreOverrideCursor();
		}
	});

    mainWindow.show();

    return QApplication::exec();
}
