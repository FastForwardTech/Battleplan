#include "gamemap.h"
#include "ui_gamemap.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QSizeGrip>
#include <QFrame>

#include "playereditdialog.h"

GameMap::GameMap(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::GameMap)
{
	ui->setupUi(this);
	setWindowFlags(Qt::SubWindow);
	ui->layout->setContentsMargins(QMargins());
	ui->layout->setSpacing(0);
	//Add size grip (to resize) to layout
	mpSizeGrip = new QSizeGrip(this);
	mpSizeGrip->setStyleSheet("QSizeGrip { background: url(:/resize.jpg); }");
	ui->layout->addWidget(mpSizeGrip, 0, 0, 1, 1, Qt::AlignBottom | Qt::AlignRight);
	Player* player = new Player();
	player->setXPos(2);
	addPlayer(player);
	this->setAttribute(Qt::WA_Hover, true);
	this->setAttribute(Qt::WA_MouseTracking);
	this->setAttribute(Qt::WA_MouseNoMask);
}

GameMap::~GameMap()
{
	delete mpSizeGrip;
	delete ui;
}

void GameMap::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
		 opt.init(this);
		 QPainter painter(this);
		 style()->drawPrimitive(QStyle::PE_Widget, &opt, &painter, this);

	   painter.begin(this);

		 painter.setPen(mGridColor);
		 const QRectF rect = event->rect();
		 qreal start = rect.top() + gridVOffset;
		 for (qreal y = start - gridStep; y < rect.bottom(); )
		 {
			y += gridStep;
			painter.drawLine(rect.left(), y, rect.right(), y);
		 }
		 // now draw vertical grid
		 start = rect.left() + gridHOffset;
		 for (qreal x = start - gridStep; x < rect.right(); )
		 {
			x += gridStep;
			painter.drawLine(x, rect.top(), x, rect.bottom());
		 }

       drawPlayers(&painter);
	   if (mpCurrentPlayer != nullptr)
	   {
		   mpCurrentPlayer->drawPlayerCard(&painter, mCurrentMouseX, mCurrentMouseY);
	   }
		QWidget::paintEvent(event);
	   painter.end();
}

void GameMap::addPlayer(Player *apPlayer)
{
	mPlayers.append(apPlayer);
}

int GameMap::gridSize()
{
	return gridStep;
}

QVector<Player *> GameMap::getPlayers()
{
	return mPlayers;
}

void GameMap::changeGridSize(int size)
{
	gridStep = size;
	emit gridSizeChanged(gridStep);
	repaint();
}

void GameMap::changeGridVOffset(int offset)
{
	gridVOffset = offset;
	emit gridVOffsetChanged(gridVOffset);
	repaint();
}

void GameMap::changeGridHOffset(int offset)
{
	gridHOffset = offset;
	emit gridHOffsetChanged(gridHOffset);
	repaint();
}

void GameMap::changeGridColor(QColor color)
{
	mGridColor = color;
	repaint();
}

void GameMap::drawPlayers(QPainter* aPainter)
{
	for(Player* player: mPlayers)
	{
        aPainter->setPen(player->color());
		QBrush brush(player->color(), Qt::Dense5Pattern);
        aPainter->fillRect(getPlayerRect(player), brush);
	}
}

QRect GameMap::getPlayerRect(Player* player)
{
	return QRect((player->getXPos() * gridStep) + gridHOffset, (player->getYPos() * gridStep) + gridVOffset, gridStep, gridStep);
}

bool GameMap::event(QEvent * e)
{
	switch(e->type())
	{
	case QEvent::MouseMove:
		mouseMove(static_cast<QMouseEvent*>(e));
		return true;
		break;
	case QEvent::MouseButtonPress:
		mousePress(static_cast<QMouseEvent*>(e));
		return true;
		break;
	case QEvent::MouseButtonRelease:
		mouseRelease(static_cast<QMouseEvent*>(e));
		return true;
		break;
	case QEvent::MouseButtonDblClick:
		mouseDoubleClick(static_cast<QMouseEvent*>(e));
		return true;
		break;
	default:
		break;
	}
	return QWidget::event(e);
}

void GameMap::mouseMove(QMouseEvent *event)
{
	int xPos = event->x();
	int yPos = event->y();

	mpCurrentPlayer = nullptr;

	for (Player* player: mPlayers)
	{
		if (getPlayerRect(player).contains(xPos, yPos))
		{
			mpCurrentPlayer = player;
			mCurrentMouseX = xPos;
			mCurrentMouseY = yPos;
			repaint();
		}
	}

	if (dragOccuring == true && mpCurrentPlayer == nullptr && dragging != nullptr)
	{
		QRect playerRect = getPlayerRect(dragging);
		int width = playerRect.width();
		int height = playerRect.height();
		if (xPos > playerRect.x() + width)
		{
			dragging->setXPos(dragging->getXPos()+1);
		}
		else if (xPos < playerRect.x())
		{
			dragging->setXPos(dragging->getXPos()-1);
		}
		else if (yPos > playerRect.y() + height)
		{
			dragging->setYPos(dragging->getYPos()+1);
		}
		else if (yPos < playerRect.y())
		{
			dragging->setYPos(dragging->getYPos()-1);
		}

		dragging = nullptr;
		dragOccuring = false;
	}
}

void GameMap::mouseDoubleClick(QMouseEvent*)
{
	if (mpCurrentPlayer != nullptr)
	{
		PlayerEditDialog dialog(this);
		dialog.setPlayer(mpCurrentPlayer);
		dialog.exec();
	}
}

void GameMap::mousePress(QMouseEvent*)
{
	if (mpCurrentPlayer != nullptr)
	{
		dragOccuring = true;
		dragging = mpCurrentPlayer;
	}
}

void GameMap::mouseRelease(QMouseEvent*)
{
	dragOccuring = false;
}
