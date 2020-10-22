#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPainter>

class Player : public QWidget
{

	Q_OBJECT
public:
	explicit Player(QWidget *parent = nullptr);

	QRect drawPlayerCard(QPainter* aPainter, int x, int y);

	QColor color() const;
	void setColor(const QColor &color);

	QString getName() const;
	void setName(const QString &name);

	int getMaxHitpoints() const;
	void setMaxHitpoints(int maxHitpoints);

	int getCurrentHitpoints() const;
	void setCurrentHitpoints(int currentHitpoints);

	QVector<QString> getConditions() const;
	void setConditions(const QVector<QString> &conditions);

	int getGridX() const;
	void setGridX(int gridX);

	int getGridY() const;
	void setGridY(int gridY);

	void setGridPos(int aX, int aY);

	QRegion getClippingRegion() const;
	void setClippingRegion(const QRegion &value);

signals:
	void playerUpdated();
	void requestDelete(Player* player);

protected:
	void paintEvent(QPaintEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

	bool event(QEvent *e) override;
private:
	QColor mColor = Qt::black;
	QString mName = "Player Name";
	int mMaxHitpoints = 10;
	int mCurrentHitpoints = 10;
	QVector<QString> mConditions;

	QRegion clippingRegion;
	void mouseDoubleClick(QMouseEvent *);

	int mGridX = 0;
	int mGridY = 0;

private slots:
	void ShowContextMenu(const QPoint& pos);
};

#endif // PLAYER_H
