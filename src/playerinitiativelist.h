#ifndef PLAYERINITIATIVELIST_H
#define PLAYERINITIATIVELIST_H

#include <QWidget>

#include "player.h"

class PlayerInitiativeList : public QWidget
{
	Q_OBJECT

public:
	explicit PlayerInitiativeList(QWidget *parent = nullptr);
	~PlayerInitiativeList();

	void addPlayer(Player* pPlayer);

public slots:
	void playersChanged(QVector<Player*> players);

protected:
	void paintEvent(QPaintEvent* event) override;

private:
//	Ui::PlayerInitiativeList *ui;

	QList<QPair<QString, int>> mInternalPlayerInitiativeList;

	void sort();
};

#endif // PLAYERINITIATIVELIST_H
