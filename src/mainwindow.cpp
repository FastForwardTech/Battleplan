#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QColorDialog>
#include <QBuffer>
#include <QGridLayout>

#include "serverconnectdialog.h"
#include "newplayerdialog.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	QGridLayout* layout = new QGridLayout();
	ui->centralwidget->setLayout(layout);

	mpBattleClient = new BattleClient();

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

	mpChangeGridColor = new QPushButton("Change Grid Color");

	mpInitiativeList = new PlayerInitiativeList(ui->centralwidget);

	mpGameMap = new GameMap();
	mpGameMap->setAutoFillBackground(false);
	mpGameMap->hide();

	layout->setColumnStretch(0, 1);
	layout->setRowStretch(7, 1);
	layout->addWidget(mpGameMap, 0, 0, 8, 1);
	layout->addWidget(new QLabel("Grid Size"), 0, 1);
	layout->addWidget(mpGridSpinBox, 1, 1);
	layout->addWidget(new QLabel("Grid Horizontal Offset"), 2, 1);
	layout->addWidget(mpGridHOffset, 3, 1);
	layout->addWidget(new QLabel("Grid Vertical Offset"), 4, 1);
	layout->addWidget(mpGridVOffset, 5, 1);
	layout->addWidget(mpChangeGridColor, 6, 1);
	layout->addWidget(mpInitiativeList, 7, 1);

	connectMapSignals();

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

	initializeState(true);
}

MainWindow::~MainWindow()
{
	delete mpInitiativeList;
	delete mpChangeGridColor;
	delete mpGridVOffset;
	delete mpGridHOffset;
	delete mpGridSpinBox;
	delete mpBattleClient;
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
		fileNames = dialog.selectedFiles();
		QImage selectedImage(fileNames[0]);
		setMapImage(selectedImage);

		QByteArray ba;
		QDataStream stream(&ba, QIODevice::WriteOnly);
		stream << selectedImage;
		mpBattleClient->initializeMap(ba);
	}
}

void MainWindow::setMapImage(QImage img)
{
	mpGameMap->setBackgroundImage(img);
	mpGameMap->show();
	mpGameMap->update();
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
	connect(mpChangeGridColor, SIGNAL(pressed()), this, SLOT(openColorDialog()));

	connect(mpGameMap, SIGNAL(gridSizeChanged(qreal)), mpBattleClient, SLOT(updateGridStep(qreal)));
	connect(mpGameMap, SIGNAL(gridHOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetX(int)));
	connect(mpGameMap, SIGNAL(gridVOffsetChanged(int)), mpBattleClient, SLOT(updateGridOffsetY(int)));
	connect(mpGameMap, SIGNAL(playersChanged(QVector<Player*>)), mpBattleClient, SLOT(updatePlayers(QVector<Player*>)));
	connect(mpGameMap, SIGNAL(playersChanged(QVector<Player*>)), mpInitiativeList, SLOT(playersChanged(QVector<Player*>)));
	connect(mpGameMap, SIGNAL(markerAdded(PositionMarker*)), mpBattleClient, SLOT(addMarker(PositionMarker*)));
	connect(mpBattleClient, SIGNAL(stateUpdateFromServer(State::GameState)), this, SLOT(updateStateFromServer(State::GameState)));
	connect(mpBattleClient, SIGNAL(mapUpdateFromServer(QImage)), this, SLOT(setMapImage(QImage)));
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
		connect(&dialog, SIGNAL(connectToServer(QString)), this, SLOT(connectToServer(QString)));
        dialog.exec();
	}
	else
	{
		noMapOnConnectError.showMessage("No game map was detected");
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
		player.initiative = gamePlayer->getInitiative();
		for (int j = 0; j < gamePlayer->getConditions().size(); j++)
		{
			auto condition = gamePlayer->getConditions().at(j);
			player.conditions.append(condition);
		}
		state.players.append(player);
	}

	State::Marker marker;
	marker.x = 0;
	marker.y = 0;
	marker.valid = false;
	state.marker = marker;

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
		player->setInitiative(newPlayer.initiative);
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
			player->setInitiative(newPlayer.initiative);
			player->blockSignals(false);
			mpGameMap->blockSignals(true);
			mpGameMap->addPlayer(player);
			mpGameMap->blockSignals(false);
		}
	}

	mpInitiativeList->playersChanged(mpGameMap->getPlayers());

	// for position marker, we just add it when received. it should be automatically cleared from state when it expires
	if (aNewState.marker.valid == true)
	{
		mpGameMap->blockSignals(true);
		PositionMarker* marker = new PositionMarker();
		marker->setGridPos(aNewState.marker.x, aNewState.marker.y);
		mpGameMap->addMarker(marker);
		mpGameMap->blockSignals(false);
	}

	repaint();
}

void MainWindow::sendPlayerUpdate()
{
	mpBattleClient->updatePlayers(mpGameMap->getPlayers());
}

void MainWindow::on_actionStart_Game_triggered()
{
	initializeState();
	mpBattleClient->sendMapData();
}

void MainWindow::connectToServer(QString code)
{
	mpBattleClient->connectToServer(code);
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
