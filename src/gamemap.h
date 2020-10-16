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
	int gridSize();
	QVector<Player*> getPlayers();

protected:
	bool event(QEvent *e) override;
	void mouseMove(QMouseEvent *event);
	void mouseDoubleClick(QMouseEvent *event);
	void mousePress(QMouseEvent *event);
	void mouseRelease(QMouseEvent *event);

private:
	Ui::GameMap *ui;
	QSizeGrip* mpSizeGrip = nullptr;
	QColor mGridColor = Qt::white;
	QVector<Player*> mPlayers;
	int gridStep = 80;
	int gridHOffset = 0;
	int gridVOffset = 0;

	Player* mpCurrentPlayer = nullptr;
	int mCurrentMouseX;
	int mCurrentMouseY;

	bool dragOccuring = false;
	Player* dragging = nullptr;

	void drawPlayers();
	void drawPlayerCard(Player* player, int x, int y);
	QRect getPlayerRect(Player* player);
};

#endif // GAMEMAP_H
