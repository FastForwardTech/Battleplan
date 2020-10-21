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

	void drawPlayerCard(QPainter* aPainter, int x, int y);

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

signals:
	void playerUpdated();
	void requestDelete(Player* player);

protected:
	void paintEvent(QPaintEvent *event) override;

	bool event(QEvent *e) override;
private:
	QColor mColor = Qt::black;
	QString mName = "Player Name";
	int mMaxHitpoints = 10;
	int mCurrentHitpoints = 8;
	QVector<QString> mConditions = { "Charmed", "Invisible" };

	QRegion clippingRegion;
	void mouseDoubleClick(QMouseEvent *);

private slots:
	void ShowContextMenu(const QPoint& pos);
};

#endif // PLAYER_H
