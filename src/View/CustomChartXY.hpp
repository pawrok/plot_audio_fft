#pragma once
#include <vtkChartXY.h>
#include <vtkContextMouseEvent.h>
#include <vtkContextScene.h>
#include <vtkTransform2D.h>
#include <vtkAxis.h>

class CustomChartXY : public vtkChartXY {
public:
    static CustomChartXY* New() { return new CustomChartXY(); }
    vtkTypeMacro(CustomChartXY, vtkChartXY);

protected:
    // Virtual functions
    bool Paint(vtkContext2D* painter) override;
    bool MouseButtonPressEvent(const vtkContextMouseEvent& mouse) override;
    bool MouseMoveEvent(const vtkContextMouseEvent& mouse) override;
    bool MouseButtonReleaseEvent(const vtkContextMouseEvent& mouse) override;

private:
    void ZoomToRect(const vtkRectf& rect);
    bool draw_rect {false};
};
