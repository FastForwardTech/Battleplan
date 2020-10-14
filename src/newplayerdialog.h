#ifndef NEWPLAYERDIALOG_H
#define NEWPLAYERDIALOG_H

#include <QDialog>

#include "player.h"

namespace Ui {
class NewPlayerDialog;
}

class NewPlayerDialog : public QDialog
{
	Q_OBJECT

public:
	explicit NewPlayerDialog(QWidget *parent = nullptr);
	~NewPlayerDialog();

signals:

	void addPlayer(Player* player);

private slots:
	void on_pushButton_clicked();

	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

private:
	Ui::NewPlayerDialog *ui;

	Player* mpNewPlayer;

	QColor mColor = Qt::black;
};

#endif // NEWPLAYERDIALOG_H
