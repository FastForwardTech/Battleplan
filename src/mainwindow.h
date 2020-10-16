#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolBar>
#include <QSlider>
#include <QAction>
#include <QErrorMessage>

#include "gamemap.h"
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
	void on_actionNew_Map_triggered();
	void openColorDialog();

	void on_actionAdd_Player_triggered();

	void on_actionConnect_triggered();

	void initializeServerState();

	void updateStateFromServer(State::GameState aNewState);

	void sendPlayerUpdate();

private:
	Ui::MainWindow *ui;

	GameMap* mpGameMap = nullptr;

	QToolBar* mpToolbar;

	QSlider* mpSlider;

	QSlider* mpGridHOffset;

	QSlider* mpGridVOffset;

	QAction* mpChangeGridColor;

	void connectMapSignals();

	BattleClient* mpBattleClient;

	QErrorMessage noMapOnConnectError;
};
#endif // MAINWINDOW_H
