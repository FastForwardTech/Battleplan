#ifndef POSITIONMARKER_H
#define POSITIONMARKER_H

#include <QPropertyAnimation>
#include "player.h"

class PositionMarker : public Player
{
	Q_OBJECT
	Q_PROPERTY(double parcelDivision READ getParcelDivision WRITE setParcelDivision)
public:
	explicit PositionMarker(QWidget *parent = nullptr);
	~PositionMarker();

	double getParcelDivision() const;
	void setParcelDivision(double value);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
	double parcelDivision = 3;

	QPropertyAnimation* animation;
};

#endif // POSITIONMARKER_H
