#pragma once
#include <QWidget>

class RangeSlider : public QWidget
{
	Q_OBJECT
public:
	explicit RangeSlider(QWidget* parent = nullptr);
	void setRange(int min, int max);
	void setValues(int lower, int upper);
	int lowerValue() const;
	int upperValue() const;

	signals:
		void valuesChanged(int lower, int upper);

protected:
	void paintEvent(QPaintEvent*) override;
	void mousePressEvent(QMouseEvent*) override;
	void mouseMoveEvent(QMouseEvent*) override;
	void mouseReleaseEvent(QMouseEvent*) override;

private:
	int m_min{0}, m_max{1000};
	int m_lower{0}, m_upper{1000};
	bool m_dragLower{false}, m_dragUpper{false};
};
