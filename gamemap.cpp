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

	   drawPlayers();
	   if (mpCurrentPlayer != nullptr)
	   {
		   drawPlayerCard(mpCurrentPlayer, mCurrentMouseX, mCurrentMouseY);
	   }
		QWidget::paintEvent(event);
	   painter.end();
}

void GameMap::addPlayer(Player *apPlayer)
{
	mPlayers.append(apPlayer);
}

void GameMap::changeGridSize(int size)
{
	gridStep = size;
	repaint();
}

void GameMap::changeGridVOffset(int offset)
{
	gridVOffset = offset;
	repaint();
}

void GameMap::changeGridColor(QColor color)
{
	mGridColor = color;
	repaint();
}

void GameMap::drawPlayers()
{
	QPainter painter(this);
	painter.begin(this);
	for(Player* player: mPlayers)
	{
		painter.setPen(player->color());
		QBrush brush(player->color(), Qt::Dense5Pattern);
		painter.fillRect(getPlayerRect(player), brush);
	}
	painter.end();
}

void GameMap::drawPlayerCard(Player* player, int x, int y)
{
	QPainter painter(this);
	painter.begin(this);
	painter.fillRect(x + 10, y - 10, 100, 150, Qt::black);
	painter.setPen(Qt::yellow);
	int yOffset = 5;
	int yInc = 15; // space between lines of text
	painter.drawText(x + 15, y + yOffset, player->getName());
	painter.setPen(Qt::white);
	y += yInc;
	painter.drawText(x + 15, y + yOffset, QString("HP: %1/%2").arg(player->getCurrentHitpoints()).arg(player->getMaxHitpoints()));
	y += yInc;
	painter.drawLine(x + 20, y + yOffset, x + 100, y + yOffset);
	y += yInc;
	painter.drawText(x + 15, y + yOffset, "Conditions:");
	painter.setPen(Qt::green);
	for(QString condition: player->getConditions())
	{
		y += yInc;
		painter.drawText(x + 20, y + yOffset, condition);
	}
	painter.end();
}

QRect GameMap::getPlayerRect(Player* player)
{
	return QRect((player->getXPos() * gridStep) + gridHOffset, (player->getYPos() * gridStep) + gridVOffset, gridStep, gridStep);
}

void GameMap::changeGridHOffset(int offset)
{
	gridHOffset = offset;
	repaint();
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
