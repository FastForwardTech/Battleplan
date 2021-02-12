#include "gamemap.h"
#include "ui_gamemap.h"

#include <QPainter>
#include <QPaintEvent>
#include <QStyleOption>
#include <QSizeGrip>
#include <QFrame>
#include <QMenu>

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
	this->setFocus();
	this->setFocusPolicy(Qt::ClickFocus);

	this->setMouseTracking(true);
	this->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));
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
	QWidget::paintEvent(event);

	painter.drawImage(0, 0, backgroundImage);

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

	// align players based on current grid parameters
	for (Player* p: mPlayers)
	{
		p->move((p->getGridPos().x() * gridStep) + gridHOffset, (p->getGridPos().y() * gridStep) + gridVOffset);
		p->resize(gridStep, gridStep);
	}

	// do this after we draw children, so it stays on top
	if (mpPlayerUnderMouse != nullptr)
	{
		// yep, this is ugly. clip all other players by translating the global coordinates of the info
		// card into local coordinates for each widget, then applying the inverse as the clipping region
		QRect card = mpPlayerUnderMouse->drawPlayerCard(&painter, mEventPos.x(), mEventPos.y());
		for(Player* p: mPlayers)
		{
			QRect localCard = QRect(p->mapFromParent(card.topLeft()), p->mapFromParent(card.bottomRight()));

			QRect map = this->rect();
			QRect localMap = QRect(p->mapFromParent(map.topLeft()), p->mapFromParent(map.bottomRight()));

			QRegion localClip = QRegion(QRegion(localMap)).subtracted(QRegion(localCard));
			p->setClippingRegion(localClip);
		}
	}
}

void GameMap::addPlayer(Player *apPlayer)
{
	mPlayers.append(apPlayer);
	apPlayer->setParent(this);
	apPlayer->installEventFilter(this);
	apPlayer->show();
	connect(apPlayer, SIGNAL(requestDelete(Player*)), this, SLOT(removePlayer(Player*)));
	connect(apPlayer, SIGNAL(playerUpdated()), this, SLOT(EmitPlayerUpdate()));
	EmitPlayerUpdate();
}

void GameMap::removePlayer(Player* apPlayer)
{
	mPlayers.removeAll(apPlayer);
	layout()->removeWidget(apPlayer);
	if (mpPlayerUnderMouse == apPlayer)
	{
		mpPlayerUnderMouse = nullptr;
	}
	delete apPlayer;
	EmitPlayerUpdate();
}

int GameMap::gridSize()
{
	return gridStep;
}

QVector<Player *> GameMap::getPlayers()
{
	return mPlayers;
}

void GameMap::changeGridSize(qreal size)
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
		mpPlayerUnderMouse = player;
		QMouseEvent* e = static_cast<QMouseEvent*>(event);
		mEventPos = player->mapToParent(e->pos());
		update();
		return true;
	}
	else if (event->type() == QEvent::Leave)
	{
		mpPlayerUnderMouse = nullptr;
		update();
		return true;
	}
	else
	{
		return QObject::eventFilter(obj, event);
	}
}

void GameMap::mousePressEvent(QMouseEvent *event)
{
	for (Player* player: mPlayerSelection)
	{
		player->setSelected(false);
	}
	mPlayerSelection.clear();
	mRubberBandOrigin = event->pos();
	if (mpSelectionBox == nullptr)
	{
		mpSelectionBox = new QRubberBand(QRubberBand::Rectangle, this);
	}
	mpSelectionBox->setGeometry(QRect(mRubberBandOrigin, QSize()));
	mpSelectionBox->show();
	update();
}

void GameMap::mouseMoveEvent(QMouseEvent *event)
{
	if (mpSelectionBox != nullptr)
	{
		mpSelectionBox->setGeometry(QRect(mRubberBandOrigin, event->pos()).normalized());
	}
}

void GameMap::mouseReleaseEvent(QMouseEvent *)
{
	for (Player* player: mPlayers)
	{
		if (mpSelectionBox->geometry().contains(player->geometry()) || mpSelectionBox->geometry().intersects(player->geometry()))
		{
			mPlayerSelection.append(player);
			player->setSelected(true);
		}
	}
	mpSelectionBox->hide();
	delete mpSelectionBox;
	mpSelectionBox = nullptr;
	update();
}

void GameMap::keyPressEvent(QKeyEvent *event)
{
	for (Player* player: mPlayerSelection)
	{
		QApplication::sendEvent(player, event);
	}
}

void GameMap::setBackgroundImage(const QImage &value)
{
	backgroundImage = value;
}

void GameMap::ShowContextMenu(const QPoint &pos)
{
	QPoint globalPos = this->mapToGlobal(pos);

	QMenu myMenu;
	QAction addAction("Add Player Here");
	myMenu.addAction(&addAction);

	QAction* selectedItem = myMenu.exec(globalPos);
	if (selectedItem == &addAction)
	{
		Player* p = new Player();
		// if the grid offset has shifted, we need to subtract that shift from the add pos
		// to get the actual square
		p->setGridPos((pos.x() - gridHOffset) / gridStep, (pos.y() - gridVOffset) / gridStep);
		addPlayer(p);
	}
	else
	{
		// nothing was chosen
	}
}

void GameMap::EmitPlayerUpdate()
{
	emit playersChanged(mPlayers);
}
