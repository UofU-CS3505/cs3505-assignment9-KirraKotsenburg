#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a QStackedWidget: index 0 = Main Menu, index 1 = Game
    m_stackWidget = new QStackedWidget(this);
    setCentralWidget(m_stackWidget);

    // ---------- Main Menu UI Setup ----------
    QWidget *menuWidget = new QWidget(this);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);

    QLabel *titleLabel = new QLabel("Save Sick Grandma", menuWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleLabel->setFont(titleFont);

    QLabel *infoLabel = new QLabel("You’re collecting herbs to help your sick grandmother.\n"
                                   "But beware—some plants are poisonous!\n\n"
                                   "Use arrow keys to drive the tractor.", menuWidget);
    infoLabel->setAlignment(Qt::AlignCenter);

    QPushButton *startButton = new QPushButton("START", menuWidget);
    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);

    menuLayout->addStretch();
    menuLayout->addWidget(titleLabel);
    menuLayout->addWidget(infoLabel);
    menuLayout->addWidget(startButton);
    menuLayout->addStretch();

    // ---------- Game Screen UI Setup ----------
    WorldRenderer *gameWidget = new WorldRenderer(this);

    // Add both screens to the stacked widget
    m_stackWidget->addWidget(menuWidget);   // index 0: menu
    m_stackWidget->addWidget(gameWidget);   // index 1: gameplay

    // Set window size and title
    resize(800, 600);
    setWindowTitle("Save Sick Grandma");
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Switch to game screen when "START" is clicked
void MainWindow::startGame()
{
    m_stackWidget->setCurrentIndex(1);
}
