#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QLabel>
#include <QColorDialog>

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
		mpGameMap = new GameMap(ui->centralwidget);

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

void MainWindow::on_actionAdd_Player_triggered()
{
	NewPlayerDialog dialog(this);
	connect(&dialog, SIGNAL(addPlayer(Player*)), mpGameMap, SLOT(addPlayer(Player*)));
	dialog.exec();
}
