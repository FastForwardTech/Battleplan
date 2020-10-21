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
	ui->activeConditionsList->addItems(apPlayer->getConditions().toList());
	ui->posX->setValue(apPlayer->x());
	ui->posY->setValue(apPlayer->y());
	for(QString text: apPlayer->getConditions())
	{
		auto matches = ui->inactiveConditionsList->findItems(text, Qt::MatchFixedString);
		if (matches.size() > 0)
		{
			delete matches.at(0);
		}
	}
}

void PlayerEditDialog::on_buttonBox_accepted()
{
	if (mpPlayer != nullptr)
	{
		mpPlayer->setName(ui->name->text());
		mpPlayer->setCurrentHitpoints(ui->currentHitpoints->value());
		mpPlayer->setMaxHitpoints(ui->maxHitpoints->value());
		QVector<QString> conditions;
		for(int i = 0; i < ui->activeConditionsList->count(); i++)
		{
			QListWidgetItem* item = ui->activeConditionsList->item(i);
			conditions.append(item->text());
		}
		mpPlayer->move(ui->posX->value(), ui->posY->value());
		mpPlayer->setConditions(conditions);
	}
}

void PlayerEditDialog::on_buttonBox_rejected()
{
	return;
}

void PlayerEditDialog::on_addCondition_button_clicked()
{
	ui->activeConditionsList->addItem(ui->inactiveConditionsList->takeItem(ui->inactiveConditionsList->currentRow()));
	ui->activeConditionsList->sortItems();
}

void PlayerEditDialog::on_removeCondition_button_clicked()
{
	ui->inactiveConditionsList->addItem(ui->activeConditionsList->takeItem(ui->activeConditionsList->currentRow()));
	ui->inactiveConditionsList->sortItems();
}
