#include "player.h"

#include <QHoverEvent>

Player::Player(QWidget *parent) : QWidget(parent)
{
	this->setAttribute(Qt::WA_Hover, true);
}

void Player::drawPlayerCard(QPainter* aPainter, int x, int y)
{
	aPainter.fillRect(x + 10, y - 10, 100, 150, Qt::black);
	painter.setPen(Qt::yellow);
	int yOffset = 5;
	int yInc = 15; // space between lines of text
	painter.drawText(x + 15, y + yOffset, getName());
	painter.setPen(Qt::white);
	y += yInc;
	painter.drawText(x + 15, y + yOffset, QString("HP: %1/%2").arg(getCurrentHitpoints()).arg(getMaxHitpoints()));
	y += yInc;
	painter.drawLine(x + 20, y + yOffset, x + 100, y + yOffset);
	y += yInc;
	painter.drawText(x + 15, y + yOffset, "Conditions:");
	painter.setPen(Qt::green);
	for (QString condition : getConditions())
	{
		y += yInc;
		painter.drawText(x + 20, y + yOffset, condition);
	}
}

QVector<QString> Player::getConditions() const
{
	return mConditions;
}

void Player::setConditions(const QVector<QString> &conditions)
{
	mConditions = conditions;
	emit playerUpdated();
}

int Player::getCurrentHitpoints() const
{
	return mCurrentHitpoints;
}

void Player::setCurrentHitpoints(int currentHitpoints)
{
	mCurrentHitpoints = currentHitpoints;
	emit playerUpdated();
}

int Player::getMaxHitpoints() const
{
	return mMaxHitpoints;
}

void Player::setMaxHitpoints(int maxHitpoints)
{
	mMaxHitpoints = maxHitpoints;
	emit playerUpdated();
}

QString Player::getName() const
{
	return mName;
}

void Player::setName(const QString &name)
{
	mName = name;
	emit playerUpdated();
}

int Player::size() const
{
	return mSize;
}

void Player::setSize(int size)
{
	mSize = size;
}

QColor Player::color() const
{
	return mColor;
}

void Player::setColor(const QColor &color)
{
	mColor = color;
	emit playerUpdated();
}

int Player::getXPos() const
{
	return xPos;
}

void Player::setXPos(int value)
{
	xPos = value;
	emit playerUpdated();
}

int Player::getYPos() const
{
	return yPos;
}

void Player::setYPos(int value)
{
	yPos = value;
	emit playerUpdated();
}
