#include "CustomChartXY.hpp"
#include "vtkContext2D.h"
#include "vtkBrush.h"
#include <vtkTransform2D.h>
#include <vtkContextTransform.h>

#include <vtkPlot.h>

bool CustomChartXY::MouseButtonPressEvent(const vtkContextMouseEvent& mouse)
{
    if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON) {
        MouseBox.Set(mouse.GetPos().GetX(), mouse.GetPos().GetY(), 0, 0);
        draw_rect = true;
        Scene->SetDirty(true);
        return true;
    }
    if (mouse.GetButton() == vtkContextMouseEvent::RIGHT_BUTTON) {
        RecalculateBounds();
        Scene->SetDirty(true);
        return true;
    }
    return Superclass::MouseButtonPressEvent(mouse);
}

bool CustomChartXY::MouseMoveEvent(const vtkContextMouseEvent& mouse)
{
    if (draw_rect) {
        MouseBox.SetWidth(mouse.GetPos().GetX() - MouseBox.GetX());
        MouseBox.SetHeight(mouse.GetPos().GetY() - MouseBox.GetY());
        Scene->SetDirty(true);
        return true;
    }
    return Superclass::MouseMoveEvent(mouse);
}

bool CustomChartXY::MouseButtonReleaseEvent(const vtkContextMouseEvent& mouse)
{
    if (mouse.GetButton() == vtkContextMouseEvent::LEFT_BUTTON) {
        draw_rect = false;
        ZoomToRect(MouseBox);
        return true;
    }
    return Superclass::MouseButtonReleaseEvent(mouse);
}

bool CustomChartXY::Paint(vtkContext2D* painter)
{
    const bool ok = Superclass::Paint(painter);
    if (ok && draw_rect)
    {
        painter->GetBrush()->SetColor(255, 255, 255, 0);
        painter->GetPen()->SetColor(255, 255, 255, 255);
        painter->GetPen()->SetWidth(2.0);
        painter->GetPen()->SetLineType(vtkPen::SOLID_LINE);
        painter->DrawRect(MouseBox.GetX(), MouseBox.GetY(), MouseBox.GetWidth(), MouseBox.GetHeight());
    }

    return ok;
}

void CustomChartXY::ZoomToRect(const vtkRectf& rect)
{
    // ignore a click-only or line-only “rectangle”
    if (std::abs(rect.GetWidth()) < 1e-3f || std::abs(rect.GetHeight()) < 1e-3f)
        return;

    double inPts[4] = {
        rect.GetX(),
        rect.GetY(),
        rect.GetX() + rect.GetWidth(),
        rect.GetY() + rect.GetHeight()
    };
    double outPts[4];

    vtkTransform2D* t = GetTransforms().at(0)->GetTransform();
    t->InverseTransformPoints(inPts, outPts, 2);

    double x0 = std::min(outPts[0], outPts[2]);
    double x1 = std::max(outPts[0], outPts[2]);
    double y0 = std::min(outPts[1], outPts[3]);
    double y1 = std::max(outPts[1], outPts[3]);

    auto* xAxis = GetAxis(vtkAxis::BOTTOM);
    auto* yAxis = GetAxis(vtkAxis::LEFT);
    xAxis->SetRange(x0, x1);
    yAxis->SetRange(y0, y1);

    // Modified();
}