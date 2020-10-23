#ifndef GAMEMAP_H
#define GAMEMAP_H

#include <QWidget>
#include <QSizeGrip>
#include <QColor>
#include <QVector>

#include "player.h"

namespace Ui {
class GameMap;
}

class GameMap : public QWidget
{
	Q_OBJECT

public:
	explicit GameMap(QWidget *parent = nullptr);
	~GameMap();

	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

signals:
	void gridSizeChanged(int aSize);
	void gridHOffsetChanged(int aOffset);
	void gridVOffsetChanged(int aOffset);

public slots:
	void changeGridSize(int size);
	void changeGridHOffset(int offset);
	void changeGridVOffset(int offset);

	void changeGridColor(QColor color);
	void addPlayer(Player* apPlayer);
	void removePlayer(Player* apPlayer);
	int gridSize();
	QVector<Player*> getPlayers();

protected:
	bool eventFilter(QObject* obj, QEvent* event) override;

private:
	Ui::GameMap *ui;
	QSizeGrip* mpSizeGrip = nullptr;
	QColor mGridColor = Qt::white;
	QVector<Player*> mPlayers;
	int gridStep = 20;
	int gridHOffset = 0;
	int gridVOffset = 0;

	Player* mpCurrentPlayer = nullptr;
	QPoint mEventPos;

private slots:
	void ShowContextMenu(const QPoint& pos);
};

#endif // GAMEMAP_H
