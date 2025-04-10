#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create the WorldRenderer and set it as the central widget
    WorldRenderer* renderer = new WorldRenderer(this);
    setCentralWidget(renderer);

    // Set window size and title
    resize(800, 600);
    setWindowTitle("Vehicle Simulation");
}

MainWindow::~MainWindow()
{
    delete ui;
}
