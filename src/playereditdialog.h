#ifndef PLAYEREDITDIALOG_H
#define PLAYEREDITDIALOG_H

#include <QDialog>
#include <QColor>

#include "player.h"

namespace Ui {
class PlayerEditDialog;
}

class PlayerEditDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PlayerEditDialog(QWidget *parent = nullptr);
	~PlayerEditDialog();

	void setPlayer(Player* apPlayer);

private slots:
	void on_buttonBox_accepted();

	void on_buttonBox_rejected();

	void on_addCondition_button_clicked();

	void on_removeCondition_button_clicked();

	void on_pushButton_Color_clicked();

private:
	Ui::PlayerEditDialog *ui;
	Player* mpPlayer = nullptr;
	QColor mColor = Qt::black;
};

#endif // PLAYEREDITDIALOG_H
