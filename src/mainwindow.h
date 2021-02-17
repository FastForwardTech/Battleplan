#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QErrorMessage>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>

#include "gamemap.h"
#include "playerinitiativelist.h"
#include "network/battleclient.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void setMapImage(QImage img);
	void on_actionNew_Map_triggered();
	void openColorDialog();

	void on_actionAdd_Player_triggered();

	void on_actionConnect_triggered();

	void initializeState(bool local = false);

	void updateStateFromServer(State::GameState aNewState);

	void sendPlayerUpdate();

	void connectToServer(QString code);

	void on_actionSave_triggered();

	void on_actionLoad_triggered();

	void onServerConnect();

	void onServerDisconnect();

private:
	Ui::MainWindow *ui;

	GameMap* mpGameMap = nullptr;

	QDoubleSpinBox* mpGridSpinBox;

	QSlider* mpGridHOffset;

	QSlider* mpGridVOffset;

	QPushButton* mpChangeGridColor;

	void connectMapSignals();

	BattleClient* mpBattleClient;

	QErrorMessage noMapOnConnectError;

	QLabel mpConnectionStatus;

	PlayerInitiativeList* mpInitiativeList;
};
#endif // MAINWINDOW_H
