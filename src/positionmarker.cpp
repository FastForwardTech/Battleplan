#include "positionmarker.h"

#include <QPropertyAnimation>

PositionMarker::PositionMarker(QWidget *parent) : Player(parent)
{
	this->setContextMenuPolicy(Qt::NoContextMenu);
	animation = new QPropertyAnimation(this, "parcelDivision");
	animation->setDuration(1600);
	animation->setStartValue(5);
	animation->setKeyValueAt(0.5, 2.3);
	animation->setEndValue(5);
	animation->setLoopCount(-1);
	animation->start();
}

PositionMarker::~PositionMarker()
{
	delete animation;
}

void PositionMarker::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.begin(this);

	int size = this->width();
	int srsize = size / parcelDivision;
	QColor color = QColor(255, 140, 0);
	painter.fillRect(0, 0, srsize, srsize, color);
	painter.fillRect(0, size - srsize, srsize, srsize, color);
	painter.fillRect(size - srsize, 0, srsize, srsize, color);
	painter.fillRect(size - srsize, size - srsize, srsize, srsize, color);

	painter.end();
}

double PositionMarker::getParcelDivision() const
{
	return parcelDivision;
}

void PositionMarker::setParcelDivision(double value)
{
	parcelDivision = value;
	repaint();
}
