#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPainter>
#include <QPoint>

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

	QRegion getClippingRegion() const;
	void setClippingRegion(const QRegion &value);

	void manualRepaint();

	QPoint getGridPos() const;
	void setGridPos(const int aX, const int aY);

	void setSelected(bool value);

	int getInitiative() const;
	void setInitiative(int initiative);

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
	int mInitiative = 1;
	bool selected = false;
	bool mBloodied = false;
	QVector<QString> mConditions;

	QRegion clippingRegion;
	void mouseDoubleClick(QMouseEvent *);
	void checkAndUpdateBloodiedState();

	QPoint mGridPos;
	QPixmap mBloodiedIndicator;

private slots:
	void ShowContextMenu(const QPoint& pos);
};

#endif // PLAYER_H
