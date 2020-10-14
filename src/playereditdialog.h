#ifndef PLAYEREDITDIALOG_H
#define PLAYEREDITDIALOG_H

#include <QDialog>

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

private:
	Ui::PlayerEditDialog *ui;
	Player* mpPlayer = nullptr;
};

#endif // PLAYEREDITDIALOG_H