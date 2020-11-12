#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
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

	mpBattleClient = new BattleClient();

	mpToolbar = new QToolBar("Tools");
	this->addToolBar(Qt::RightToolBarArea, mpToolbar);

	mpGridSpinBox = new QDoubleSpinBox();
	mpGridSpinBox->setValue(20);
	mpGridSpinBox->setMinimum(15);
	mpGridSpinBox->setMaximum(30);

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
	mpToolbar->addWidget(mpGridSpinBox);
	mpToolbar->addWidget(new QLabel("Grid Horizontal Offset"));
	mpToolbar->addWidget(mpGridHOffset);
	mpToolbar->addWidget(new QLabel("Grid Vertical Offset"));
	mpToolbar->addWidget(mpGridVOffset);
	mpToolbar->addSeparator();
	mpToolbar->addAction(mpChangeGridColor);
	mpToolbar->setStyleSheet("background-color: white;");

	// set up the connection status indicator
	mpConnectionStatus.resize(15, 15);
	mpConnectionStatus.setMask(QRegion(QRect(0, 0, 15, 15), QRegion::Ellipse));
	QPalette palette = mpConnectionStatus.palette();
	palette.setColor(mpConnectionStatus.backgroundRole(), Qt::red);
	mpConnectionStatus.setAutoFillBackground(true);
	mpConnectionStatus.setPalette(palette);
	QPalette statusPalette = this->statusBar()->palette();
	statusPalette.setColor(this->statusBar()->backgroundRole(), Qt::white);
	this->statusBar()->setAutoFillBackground(true);
	this->statusBar()->setPalette(statusPalette);
	this->statusBar()->addWidget(&mpConnectionStatus);
	this->statusBar()->addPermanentWidget(new QLabel("Connection Status"));
	this->statusBar()->addPermanentWidget(&mpConnectionStatus, 1);

	connect(mpBattleClient, SIGNAL(closed()), this, SLOT(onServerDisconnect()));
	connect(mpBattleClient, SIGNAL(connected()), this, SLOT(onServerConnect()));
}

MainWindow::~MainWindow()
{
	delete mpChangeGridColor;
	delete mpGridVOffset;
	delete mpGridHOffset;
	delete mpGridSpinBox;
	delete mpBattleClient;
	delete mpToolbar;
	delete mpGameMap;
	delete ui;
}

void MainWindow::on_actionNew_Map_triggered()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Images (*.png *.jpg)"));
	dialog.setWindowTitle("Choose map file");
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
			initializeState();
	}
}

void MainWindow::openColorDialog()
{
	mpGameMap->changeGridColor(QColorDialog::getColor(Qt::white, nullptr, "Grid Color"));
}

void MainWindow::connectMapSignals()
{
	connect(mpGridSpinBox, SIGNAL(valueChanged(double)), mpGameMap, SLOT(changeGridSize(qreal)));
	connect(mpGridHOffset, SIGNAL(valueChanged(int)), mpGameMap, SLOT(changeGridHOffset(int)));
	connect(mpGridVOffset, SIGNAL(valueChanged(int)), mpGameMap, SLOT(changeGridVOffset(int)));
    connect(mpChangeGridColor, SIGNAL(triggered()), this, SLOT(openColorDialog()));

	connect(mpGameMap, SIGNAL(gridSizeChanged(qreal)), mpBattleClient, SLOT(updateGridStep(qreal)));
	connect(mpGameMap, SIGNAL(gridHOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetX(int)));
	connect(mpGameMap, SIGNAL(gridVOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetY(int)));
	connect(mpGameMap, SIGNAL(playersChanged(QVector<Player*>)), mpBattleClient, SLOT(updatePlayers(QVector<Player*>)));
	connect(mpBattleClient, SIGNAL(stateUpdateFromServer(State::GameState)), this, SLOT(updateStateFromServer(State::GameState)));
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

void MainWindow::initializeState(bool local)
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
		player.x = gamePlayer->getGridPos().x();
		player.y = gamePlayer->getGridPos().y();
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
	if (local == true)
	{
		mpBattleClient->updateLocalState(state);
	}
	else
	{
		mpBattleClient->initializeState(state);
	}
}

void MainWindow::updateStateFromServer(State::GameState aNewState)
{
	mpGridSpinBox->blockSignals(true);
	mpGridSpinBox->setValue(aNewState.gridStep);
	mpGridSpinBox->blockSignals(false);
	mpGridHOffset->blockSignals(true);
	mpGridHOffset->setValue(aNewState.gridOffsetX);
	mpGridHOffset->blockSignals(false);
	mpGridVOffset->blockSignals(true);
	mpGridVOffset->setValue(aNewState.gridOffsetY);
	mpGridVOffset->blockSignals(false);
	mpGameMap->blockSignals(true);
	mpGameMap->changeGridSize(aNewState.gridStep);
	mpGameMap->changeGridHOffset(aNewState.gridOffsetX);
	mpGameMap->changeGridVOffset(aNewState.gridOffsetY);
	mpGameMap->blockSignals(false);

	// update the existing players as far as there is a corresponding player in the state update
	for (int i = 0; i < std::min(mpGameMap->getPlayers().size(), aNewState.players.size()); i++)
	{
		Player* player = mpGameMap->getPlayers().at(i);
		State::Player newPlayer = aNewState.players.at(i);

		player->blockSignals(true);
		player->setName(newPlayer.name);
		player->setGridPos(newPlayer.x, newPlayer.y);
		player->setColor(QColor(newPlayer.red, newPlayer.green, newPlayer.blue));
		player->setMaxHitpoints(newPlayer.maxHp);
		player->setCurrentHitpoints(newPlayer.currHp);
		player->setConditions(newPlayer.conditions);
		player->blockSignals(false);
	}

	// if there are more existing players than incoming, remove the excess existing players
	if (mpGameMap->getPlayers().size() > aNewState.players.size())
	{
		for (int j = aNewState.players.size(); j < mpGameMap->getPlayers().size(); j++)
		{
			mpGameMap->blockSignals(true);
			mpGameMap->removePlayer(mpGameMap->getPlayers().at(j));
			mpGameMap->blockSignals(false);
		}
	}

	// if there are more new players than existing, we must allocate them first, then add to existing list
	if (aNewState.players.size() > mpGameMap->getPlayers().size())
	{
		for (int j = mpGameMap->getPlayers().size(); j < aNewState.players.size(); j++)
		{
			Player* player = new Player();
			State::Player newPlayer = aNewState.players.at(j);

			player->blockSignals(true);
			player->setName(newPlayer.name);
			player->setGridPos(newPlayer.x, newPlayer.y);
			player->setColor(QColor(newPlayer.red, newPlayer.green, newPlayer.blue));
			player->setMaxHitpoints(newPlayer.maxHp);
			player->setCurrentHitpoints(newPlayer.currHp);
			player->setConditions(newPlayer.conditions);
			player->blockSignals(false);
			mpGameMap->blockSignals(true);
			mpGameMap->addPlayer(player);
			mpGameMap->blockSignals(false);
		}
	}

	this->initializeState(true);

	repaint();
}

void MainWindow::sendPlayerUpdate()
{
	mpBattleClient->updatePlayers(mpGameMap->getPlayers());
}

void MainWindow::on_actionStart_Game_triggered()
{
	initializeState();
}

void MainWindow::connectToServer(QString address, int port)
{
    QString connectStr = QString("ws://%1:%2").arg(address).arg(port);
	mpBattleClient->connectToServer(QUrl(connectStr));
}

void MainWindow::on_actionSave_triggered()
{
	if (mpGameMap == nullptr)
	{
		noMapOnConnectError.showMessage("There is no game to save!");
	}
	else
	{
		QFileDialog dialog(this);
		dialog.setFileMode(QFileDialog::AnyFile);
		dialog.setNameFilter("*.dat");
		dialog.setWindowTitle("Choose a Save Location");
		QStringList fileNames;
		if (dialog.exec())
		{
			fileNames = dialog.selectedFiles();
			QString fileName = fileNames[0];
			if (!fileName.endsWith(".dat"))
			{
				fileName.append(".dat");
			}

			QFile file(fileName);
			if (!file.open(QIODevice::WriteOnly))
			{
				return;
			}

			QDataStream out(&file);
			out << mpBattleClient->getState().serialize();
		}
	}
}

void MainWindow::on_actionLoad_triggered()
{
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter("*.dat");
	dialog.setWindowTitle("Choose a save file");
	QStringList fileNames;
	if (dialog.exec())
	{
		fileNames = dialog.selectedFiles();

		QFile file(fileNames[0]);
		if (!file.open(QIODevice::ReadOnly))
		{
			return;
		}

		QDataStream in(&file);
		State::GameState state;
		QByteArray data;
		in >> data;
		state = state.deserialize(data);

		if (mpGameMap == nullptr)
		{
			on_actionNew_Map_triggered();
		}
		updateStateFromServer(state);
	}
}

void MainWindow::onServerConnect()
{
	QPalette palette = mpConnectionStatus.palette();
	palette.setColor(mpConnectionStatus.backgroundRole(), Qt::green);
	mpConnectionStatus.setPalette(palette);
	mpConnectionStatus.update();
}

void MainWindow::onServerDisconnect()
{
	QPalette palette = mpConnectionStatus.palette();
	palette.setColor(mpConnectionStatus.backgroundRole(), Qt::red);
	mpConnectionStatus.setPalette(palette);
	mpConnectionStatus.update();
}
