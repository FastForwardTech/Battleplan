#include "player.h"

#include <QHoverEvent>

Player::Player(QWidget *parent) : QWidget(parent)
{
	this->setAttribute(Qt::WA_Hover, true);
}

bool Player::event(QEvent * e)
{
	switch(e->type())
	{
	case QEvent::HoverEnter:
		hoverEnter(static_cast<QHoverEvent*>(e));
		return true;
		break;
	case QEvent::HoverLeave:
		hoverLeave(static_cast<QHoverEvent*>(e));
		return true;
		break;
	default:
		break;
	}
	return QWidget::event(e);
}

void Player::hoverEnter(QHoverEvent*)
{
	printf("Hover enter\n");
	fflush(stdout);
}

void Player::hoverLeave(QHoverEvent*)
{
	printf("Hover leave\n");
	fflush(stdout);
}

QVector<QString> Player::getConditions() const
{
	return mConditions;
}

void Player::setConditions(const QVector<QString> &conditions)
{
	mConditions = conditions;
}

int Player::getCurrentHitpoints() const
{
	return mCurrentHitpoints;
}

void Player::setCurrentHitpoints(int currentHitpoints)
{
	mCurrentHitpoints = currentHitpoints;
}

int Player::getMaxHitpoints() const
{
	return mMaxHitpoints;
}

void Player::setMaxHitpoints(int maxHitpoints)
{
	mMaxHitpoints = maxHitpoints;
}

QString Player::getName() const
{
	return mName;
}

void Player::setName(const QString &name)
{
	mName = name;
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
}

int Player::getXPos() const
{
	return xPos;
}

void Player::setXPos(int value)
{
	xPos = value;
}

int Player::getYPos() const
{
	return yPos;
}

void Player::setYPos(int value)
{
	yPos = value;
}
