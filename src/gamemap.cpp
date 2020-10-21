#include "gamemap.h"
#include "ui_gamemap.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QSizeGrip>
#include <QFrame>
#include <QMenu>

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
	player->move(2, 1);
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
	// draw horizontal grid lines
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

	QWidget::paintEvent(event);

	// do this after we draw children, so it stays on top
	if (mpCurrentPlayer != nullptr)
	{
		mpCurrentPlayer->drawPlayerCard(&painter, mEventPos.x(), mEventPos.y());
	}

	painter.end();
}

void GameMap::addPlayer(Player *apPlayer)
{
	mPlayers.append(apPlayer);
	apPlayer->setParent(this);
	apPlayer->move(apPlayer->x() * gridStep, apPlayer->y() * gridStep);
	apPlayer->resize(gridStep, gridStep);
	apPlayer->installEventFilter(this);
	apPlayer->show();
	connect(apPlayer, SIGNAL(requestDelete(Player*)), this, SLOT(removePlayer(Player*)));
}

void GameMap::removePlayer(Player* apPlayer)
{
	mPlayers.removeAll(apPlayer);
	layout()->removeWidget(apPlayer);
	if (mpCurrentPlayer == apPlayer)
	{
		mpCurrentPlayer = nullptr;
	}
	delete apPlayer;
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
	update();
}

void GameMap::changeGridVOffset(int offset)
{
	gridVOffset = offset;
	emit gridVOffsetChanged(gridVOffset);
	update();
}

void GameMap::changeGridHOffset(int offset)
{
	gridHOffset = offset;
	emit gridHOffsetChanged(gridHOffset);
	update();
}

void GameMap::changeGridColor(QColor color)
{
	mGridColor = color;
	update();
}

bool GameMap::eventFilter(QObject *obj, QEvent *event)
{
	if (event->type() == QEvent::MouseMove)
	{
		Player* player = static_cast<Player*>(obj);
		mpCurrentPlayer = player;
		QMouseEvent* e = static_cast<QMouseEvent*>(event);
		mEventPos = player->mapToParent(e->pos());
		update();
		return true;
	}
	else
	{
		return QObject::eventFilter(obj, event);
	}
}
