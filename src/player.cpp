#include "player.h"

#include <QHoverEvent>
#include <QStyleOption>
#include <QMenu>

#include "playereditdialog.h"

Player::Player(QWidget *parent) : QWidget(parent)
{
	this->setAttribute(Qt::WA_Hover, true);
	this->setMouseTracking(true);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	this->setFocusPolicy(Qt::ClickFocus);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
}

bool Player::event(QEvent* e)
{
	switch(e->type())
	{
		case QEvent::MouseButtonDblClick:
			mouseDoubleClick(static_cast<QMouseEvent*>(e));
			return true;
		break;
		default:
			break;
	}
	return QWidget::event(e);
}

QRegion Player::getClippingRegion() const
{
	return clippingRegion;
}

void Player::setClippingRegion(const QRegion &value)
{
	clippingRegion = value;
}

void Player::manualRepaint()
{
	QPaintEvent event(this->rect());
	this->paintEvent(&event);
}

void Player::mouseDoubleClick(QMouseEvent*)
{
	PlayerEditDialog dialog(this);
	dialog.setPlayer(this);
	dialog.exec();
}

QPoint Player::getGridPos() const
{
	return mGridPos;
}

void Player::setGridPos(const int aX, const int aY)
{
	mGridPos.setX(aX);
	mGridPos.setY(aY);
	emit playerUpdated();
}

void Player::ShowContextMenu(const QPoint &pos)
{
	QPoint globalPos = this->mapToGlobal(pos);

	QMenu myMenu;
	QAction deleteAction("Delete This Player");
	myMenu.addAction(&deleteAction);

	QAction* selectedItem = myMenu.exec(globalPos);
	if (selectedItem == &deleteAction)
	{
		emit requestDelete(this);
	}
	else
	{
		// nothing was chosen
	}
}

QRect Player::drawPlayerCard(QPainter* aPainter, int x, int y)
{
	// set up clipping
	QRect cardSurface = QRect(x + 10, y - 10, 100, 150);
	QRect cardSurfaceLocal = QRect(mapFromParent(cardSurface.topLeft()), mapFromParent(cardSurface.bottomRight()));
	QRegion r1 = this->rect();
	QRegion r2 = cardSurfaceLocal;
	QRegion r3 = r1.subtracted(r2);
	clippingRegion = r3;

	// start drawing the card
	aPainter->fillRect(cardSurface, Qt::black);
    aPainter->setPen(Qt::yellow);
	int yOffset = 5;
	int yInc = 15; // space between lines of text
    aPainter->drawText(x + 15, y + yOffset, getName());
    aPainter->setPen(Qt::white);
	y += yInc;
    aPainter->drawText(x + 15, y + yOffset, QString("HP: %1/%2").arg(getCurrentHitpoints()).arg(getMaxHitpoints()));
	y += yInc;
    aPainter->drawLine(x + 20, y + yOffset, x + 100, y + yOffset);
	y += yInc;
    aPainter->drawText(x + 15, y + yOffset, "Conditions:");
    aPainter->setPen(Qt::green);
	for (QString condition : getConditions())
	{
		y += yInc;
        aPainter->drawText(x + 20, y + yOffset, condition);
	}

	return cardSurface;
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

void Player::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.begin(this);

	painter.setClipRegion(clippingRegion);

	painter.setPen(mColor);
	QBrush brush(mColor, Qt::Dense5Pattern);
	painter.fillRect(this->rect(), brush);

	clippingRegion = QRegion(this->rect());

	painter.end();
}

void Player::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Right)
	{
		setGridPos(mGridPos.x() + 1, mGridPos.y());
	}
	else if (event->key() == Qt::Key_Left)
	{
		setGridPos(mGridPos.x() - 1, mGridPos.y());
	}
	else if (event->key() == Qt::Key_Up)
	{
		setGridPos(mGridPos.x(), mGridPos.y() - 1);
	}
	else if (event->key() == Qt::Key_Down)
	{
		setGridPos(mGridPos.x(), mGridPos.y() + 1);
	}
	update();
	this->parentWidget()->update();
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

QColor Player::color() const
{
	return mColor;
}

void Player::setColor(const QColor &color)
{
	mColor = color;
	emit playerUpdated();
}
