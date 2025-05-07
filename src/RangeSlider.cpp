#include "RangeSlider.hpp"
#include <QPainter>
#include <QMouseEvent>

static constexpr int kMinHeight     = 36;
static constexpr int kTrackHeight   = 6;
static constexpr int kHandleRadius  = 10;

RangeSlider::RangeSlider(QWidget* parent) : QWidget(parent)
{
    setMinimumHeight(kMinHeight);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    setCursor(Qt::PointingHandCursor);
}

void RangeSlider::setRange(int min, int max)
{
    m_min = min;
    m_max = max;
    update();
}

void RangeSlider::setValues(int lower, int upper)
{
    m_lower = qBound(m_min, lower, m_max);
    m_upper = qBound(m_min, upper, m_max);
    if (m_lower > m_upper) qSwap(m_lower, m_upper);
    update();
}

int RangeSlider::lowerValue() const { return m_lower; }
int RangeSlider::upperValue() const { return m_upper; }

void RangeSlider::paintEvent(QPaintEvent*)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const int w   = width();
    const int h   = height();
    const int ty  = h / 2 - kTrackHeight / 2;
    const double span = (m_max - m_min) ? (m_max - m_min) : 1.0;

    const int lx = (double)(m_lower - m_min) / span * w;
    const int ux = (double)(m_upper - m_min) / span * w;

    QColor bgTrack(100, 100, 100);
    QColor selTrack(200, 200, 200);
    QColor handleFill(Qt::white);
    QColor handleBorder(80, 80, 80);

    p.setPen(Qt::NoPen);
    p.setBrush(bgTrack);
    p.drawRoundedRect(QRectF(0, ty, w, kTrackHeight), 3, 3);

    p.setBrush(selTrack);
    p.drawRoundedRect(QRectF(lx, ty, ux - lx, kTrackHeight), 3, 3);

    p.setBrush(handleFill);
    p.setPen(handleBorder);
    p.drawEllipse(QPointF(lx, h / 2), kHandleRadius, kHandleRadius);
    p.drawEllipse(QPointF(ux, h / 2), kHandleRadius, kHandleRadius);
}

void RangeSlider::mousePressEvent(QMouseEvent* e)
{
    const int w = width();
    const double span = (m_max - m_min) ? (m_max - m_min) : 1.0;
    const int lx = (double)(m_lower - m_min) / span * w;
    const int ux = (double)(m_upper - m_min) / span * w;

    m_dragLower = qAbs(e->pos().x() - lx) < qAbs(e->pos().x() - ux);
    m_dragUpper = !m_dragLower;
    mouseMoveEvent(e);
}

void RangeSlider::mouseMoveEvent(QMouseEvent* e)
{
    if (!m_dragLower && !m_dragUpper) return;
    int pos = qBound(0, e->pos().x(), width());
    int val = (double)pos / width() * (m_max - m_min) + m_min;
    if (m_dragLower) m_lower = qBound(m_min, val, m_upper);
    else              m_upper = qBound(m_lower, val, m_max);
    update();
    emit valuesChanged(m_lower, m_upper);
}

void RangeSlider::mouseReleaseEvent(QMouseEvent*) { m_dragLower = m_dragUpper = false; }
