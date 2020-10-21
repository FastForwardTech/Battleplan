#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QLabel>
#include <QColorDialog>

#include "serverconnectdialog.h"
#include "newplayerdialog.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	mpToolbar = new QToolBar("Tools");
	this->addToolBar(Qt::RightToolBarArea, mpToolbar);
	mpSlider = new QSlider();
	mpSlider->setOrientation(Qt::Horizontal);
	mpSlider->setValue(80);
	mpSlider->setMinimum(10);
	mpSlider->setMaximum(100);

	mpGridHOffset = new QSlider();
	mpGridHOffset->setOrientation(Qt::Horizontal);
	mpGridHOffset->setValue(0);
	mpGridHOffset->setMinimum(0);
	mpGridHOffset->setMaximum(100);

	mpGridVOffset = new QSlider();
	mpGridVOffset->setOrientation(Qt::Horizontal);
	mpGridVOffset->setValue(0);
	mpGridVOffset->setMinimum(0);
	mpGridVOffset->setMaximum(100);

	mpChangeGridColor = new QAction("Change Grid Color");

	mpToolbar->addWidget(new QLabel("Grid Size"));
	mpToolbar->addWidget(mpSlider);
	mpToolbar->addWidget(new QLabel("Grid Horizontal Offset"));
	mpToolbar->addWidget(mpGridHOffset);
	mpToolbar->addWidget(new QLabel("Grid Vertical Offset"));
	mpToolbar->addWidget(mpGridVOffset);
	mpToolbar->addSeparator();
	mpToolbar->addAction(mpChangeGridColor);
	mpToolbar->setStyleSheet("background-color: white;");
}

MainWindow::~MainWindow()
{
	delete mpToolbar;
	delete mpGameMap;
	delete ui;
}

void MainWindow::on_actionNew_Map_triggered()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.png *.jpg)"));
	QStringList fileNames;
	if (dialog.exec())
	{
		if (mpGameMap == nullptr)
		{
			mpGameMap = new GameMap(ui->centralwidget);
		}

			fileNames = dialog.selectedFiles();
			QString stylesheet = QString("GameMap { background-image:url(\"%1\"); background-repeat: no-repeat; }").arg(fileNames[0]);
			mpGameMap->setStyleSheet(stylesheet);
			mpGameMap->show();
			connectMapSignals();
	}
}

void MainWindow::openColorDialog()
{
	mpGameMap->changeGridColor(QColorDialog::getColor(Qt::white, nullptr, "Grid Color"));
}

void MainWindow::connectMapSignals()
{
	connect(mpSlider, SIGNAL(valueChanged(int)), mpGameMap, SLOT(changeGridSize(int)));
	connect(mpGridHOffset, SIGNAL(valueChanged(int)), mpGameMap, SLOT(changeGridHOffset(int)));
	connect(mpGridVOffset, SIGNAL(valueChanged(int)), mpGameMap, SLOT(changeGridVOffset(int)));
    connect(mpChangeGridColor, SIGNAL(triggered()), this, SLOT(openColorDialog()));
}

void MainWindow::connectStateSignals()
{
    // connect the signals once we're sure the ws connection is stable
    connect(mpGameMap, SIGNAL(gridSizeChanged(int)), mpBattleClient, SLOT(updateGridStep(int)));
    connect(mpGameMap, SIGNAL(gridHOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetX(int)));
    connect(mpGameMap, SIGNAL(gridVOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetY(int)));
    connect(mpBattleClient, SIGNAL(stateUpdateFromServer(State::GameState)), this, SLOT(updateStateFromServer(State::GameState)));
    for(Player* player: mpGameMap->getPlayers())
    {
        connect(player, SIGNAL(playerUpdated()), this, SLOT(sendPlayerUpdate()));
    }
}

void MainWindow::on_actionAdd_Player_triggered()
{
	NewPlayerDialog dialog(this);
	connect(&dialog, SIGNAL(addPlayer(Player*)), mpGameMap, SLOT(addPlayer(Player*)));
	dialog.exec();
}

void MainWindow::on_actionConnect_triggered()
{
	if (mpGameMap != nullptr)
	{
        ServerConnectDialog dialog;
        connect(&dialog, SIGNAL(connectToServer(QString, int)), this, SLOT(connectToServer(QString, int)));
        dialog.exec();
	}
	else
	{
		noMapOnConnectError.showMessage("You must load a map before connecting to a server");
	}
}

void MainWindow::initializeServerState()
{
	State::GameState state;
	state.gridOffsetX = mpGridHOffset->value();
	state.gridOffsetY = mpGridVOffset->value();
	state.gridStep = mpGameMap->gridSize();
	state.numPlayers = mpGameMap->getPlayers().size();
	for(int i = 0; i < mpGameMap->getPlayers().size(); i++)
	{
		auto gamePlayer = mpGameMap->getPlayers().at(i);
		State::Player player;
		player.name = gamePlayer->getName();
		player.x = gamePlayer->x();
		player.y = gamePlayer->y();
		player.red = gamePlayer->color().red();
		player.blue = gamePlayer->color().blue();
		player.green = gamePlayer->color().green();
		player.currHp = gamePlayer->getCurrentHitpoints();
		player.maxHp = gamePlayer->getMaxHitpoints();
		for (int j = 0; j < gamePlayer->getConditions().size(); j++)
		{
			auto condition = gamePlayer->getConditions().at(j);
			player.conditions.append(condition);
		}
		state.players.append(player);
	}
	mpBattleClient->initializeState(state);
}

void MainWindow::updateStateFromServer(State::GameState aNewState)
{
	mpSlider->blockSignals(true);
	mpSlider->setValue(aNewState.gridStep);
	mpSlider->blockSignals(false);
	mpGameMap->blockSignals(true);
	mpGameMap->changeGridSize(aNewState.gridStep);
	mpGameMap->changeGridHOffset(aNewState.gridOffsetX);
	mpGameMap->changeGridVOffset(aNewState.gridOffsetY);
	mpGameMap->blockSignals(false);

	for (int i = 0; i < aNewState.players.size(); i++)
	{
		auto player = mpGameMap->getPlayers().at(i);
		auto newPlayer = aNewState.players.at(i);
		player->blockSignals(true);
		player->setName(newPlayer.name);
		player->move(newPlayer.x, newPlayer.y);
//		player->setColor()
		// TODO: figure out how to set color
		// Also TODO: figure out how to handle adding/deleting players from network perspective
		// make sure signals in initializeServerState() are hooked up to new players when created
		player->setMaxHitpoints(newPlayer.maxHp);
		player->setCurrentHitpoints(newPlayer.currHp);
		player->setConditions(newPlayer.conditions);
		player->blockSignals(false);
	}

	repaint();
}

void MainWindow::sendPlayerUpdate()
{
	mpBattleClient->updatePlayers(mpGameMap->getPlayers());
}

void MainWindow::on_actionStart_Game_triggered()
{
    initializeServerState();
}

void MainWindow::connectToServer(QString address, int port)
{
    QString connectStr = QString("ws://%1:%2").arg(address).arg(port);
    mpBattleClient = new BattleClient(QUrl(connectStr));
    connect(mpBattleClient, SIGNAL(connected()), this, SLOT(connectStateSignals()));
}
