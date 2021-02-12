#include "playerinitiativelist.h"
#include "ui_playerinitiativelist.h"

#include <QWidgetAction>

static bool compareInitiativeMaps(QPair<QString, int> map1, QPair<QString, int> map2)
{
	return map1.second > map2.second;
}

PlayerInitiativeList::PlayerInitiativeList(QWidget *parent) :
	QWidget(parent)
{
	this->resize(100, 20);
	this->setMinimumSize(100, 20);
	this->setVisible(true);
}

PlayerInitiativeList::~PlayerInitiativeList()
{
}

void PlayerInitiativeList::addPlayer(Player *pPlayer)
{
	QPair<QString, int> player(pPlayer->getName(), pPlayer->getInitiative());
	mInternalPlayerInitiativeList.append(player);
	sort();
}

void PlayerInitiativeList::playersChanged(QVector<Player *> players)
{
	mInternalPlayerInitiativeList.clear();
	for(Player* player: players)
	{
		mInternalPlayerInitiativeList.append(QPair<QString, int>(player->getName(), player->getInitiative()));
	}
	sort();
	repaint();
}

void PlayerInitiativeList::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.begin(this);

	int offset = 10;
	painter.setPen(Qt::black);
	painter.drawText(5, offset, "Marching Order");
	painter.drawLine(5, offset + 5, this->width() - 10, offset + 5);
	offset += 20;
	for(QPair<QString, int> player: mInternalPlayerInitiativeList)
	{
		painter.drawText(5, offset, player.first);
		offset += 15;
	}
	painter.drawLine(5, offset - 10, this->width() - 10, offset - 10);

	painter.end();
}

void PlayerInitiativeList::sort()
{
	std::sort(mInternalPlayerInitiativeList.begin(), mInternalPlayerInitiativeList.end(), compareInitiativeMaps);
}



