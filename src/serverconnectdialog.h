#ifndef SERVERCONNECTDIALOG_H
#define SERVERCONNECTDIALOG_H

#include <QDialog>

namespace Ui {
class ServerConnectDialog;
}

class ServerConnectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ServerConnectDialog(QWidget *parent = nullptr);
    ~ServerConnectDialog();

signals:
    void connectToServer(QString address, int  port);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::ServerConnectDialog *ui;
};

#endif // SERVERCONNECTDIALOG_H
