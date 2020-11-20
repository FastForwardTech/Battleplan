#include "serverconnectdialog.h"
#include "ui_serverconnectdialog.h"

ServerConnectDialog::ServerConnectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ServerConnectDialog)
{
    ui->setupUi(this);
}

ServerConnectDialog::~ServerConnectDialog()
{
    delete ui;
}

void ServerConnectDialog::on_buttonBox_accepted()
{
	emit connectToServer(ui->roomCode->text());
}
