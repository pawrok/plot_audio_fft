#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>

#include <omp.h>
#include <windows.h>

#include "LinePlot.hpp"
#include "FFT.hpp"

vtkNew<vtkContextView> view;

std::string getWindowsSoundFilePath() 
{
    char filePath[MAX_PATH] = { 0 };

    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(ofn));

    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr; // No specific owner window
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrFilter = "Sound Files\0*.WAV;*.MP3;*.FLAC;*.AAC;*.OGG;*.M4A\0All Files\0*.*\0";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn))
        return std::string(filePath);
    
    return std::string();
}

void onKeyPress(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    std::string key = static_cast<vtkRenderWindowInteractor*>(caller)->GetKeySym();
    if (key == "o") {
        std::string filePath = getWindowsSoundFilePath();
        if (filePath.empty())
            return;

        auto result = FFT::getBins(filePath);
        auto plot = static_cast<LinePlot*>(clientData);
        plot->setSamples(result);
        plot->setColumns();
        view->GetRenderWindow()->Render();
    }
}

int main(int argc, char* argv[])
{
    SetProcessDPIAware();

    fftw_init_threads();
    fftw_plan_with_nthreads(omp_get_max_threads() / 2);

    view->GetRenderWindow()->SetWindowName("LinePlot");
    view->GetRenderer()->SetBackground(50 / 255.0, 50 / 255.0, 50 / 255.0);

    vtkNew<vtkRenderWindowInteractor> renderWindowInteractor;
    renderWindowInteractor->SetRenderWindow(view->GetRenderWindow());

    // Set up a simple plot
    auto plot = new LinePlot(view.Get());
    plot->setAxesNames("x1", "x2", "ch1", "ch2");
    plot->setColumns();
    plot->setTitles("Frequency (Hz)", "Magnitude");

    // Add interaction for opening files
    vtkNew<vtkCallbackCommand> keyPressCallback;
    keyPressCallback->SetCallback(onKeyPress);
    keyPressCallback->SetClientData(plot);
    renderWindowInteractor->AddObserver(vtkCommand::KeyPressEvent, keyPressCallback);

    // Start rendering
    view->GetRenderWindow()->Render();
    view->GetInteractor()->Initialize();
    view->GetInteractor()->Start();

    return 0;
}
