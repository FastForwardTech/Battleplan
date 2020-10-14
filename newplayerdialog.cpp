#include "newplayerdialog.h"
#include "ui_newplayerdialog.h"

#include <QColorDialog>

NewPlayerDialog::NewPlayerDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::NewPlayerDialog)
{
	ui->setupUi(this);
	mpNewPlayer = new Player();
}

NewPlayerDialog::~NewPlayerDialog()
{
	delete ui;
}

void NewPlayerDialog::on_buttonBox_accepted()
{
	mpNewPlayer->setName(ui->playerName->text());
	mpNewPlayer->setXPos(ui->xPos->value());
	mpNewPlayer->setYPos(ui->yPos->value());
	mpNewPlayer->setColor(mColor);
	mpNewPlayer->setCurrentHitpoints(ui->currentHitpoints->value());
	mpNewPlayer->setMaxHitpoints(ui->maxHitpoints->value());

	emit addPlayer(mpNewPlayer);
}

void NewPlayerDialog::on_buttonBox_rejected()
{
	delete mpNewPlayer;
}

void NewPlayerDialog::on_pushButton_clicked()
{
	mColor = QColorDialog::getColor(Qt::black, this, "Player Color");
}
