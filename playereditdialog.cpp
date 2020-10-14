#include "playereditdialog.h"
#include "ui_playereditdialog.h"

PlayerEditDialog::PlayerEditDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PlayerEditDialog)
{
	ui->setupUi(this);
}

PlayerEditDialog::~PlayerEditDialog()
{
	delete ui;
}

void PlayerEditDialog::setPlayer(Player *apPlayer)
{
	mpPlayer = apPlayer;
	ui->name->setText(apPlayer->getName());
	ui->currentHitpoints->setValue(apPlayer->getCurrentHitpoints());
	ui->maxHitpoints->setValue(apPlayer->getMaxHitpoints());
	ui->conditionsList->addItems(apPlayer->getConditions().toList());
}

void PlayerEditDialog::on_buttonBox_accepted()
{
	if (mpPlayer != nullptr)
	{
		mpPlayer->setName(ui->name->text());
		mpPlayer->setCurrentHitpoints(ui->currentHitpoints->value());
		mpPlayer->setMaxHitpoints(ui->maxHitpoints->value());
		QVector<QString> conditions;
		for(int i = 0; i < ui->conditionsList->count(); i++)
		{
			QListWidgetItem* item = ui->conditionsList->item(i);
			conditions.append(item->text());
		}
		mpPlayer->setConditions(conditions);
	}
}

void PlayerEditDialog::on_buttonBox_rejected()
{
	return;
}
