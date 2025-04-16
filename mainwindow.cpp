#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

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
    gameWidget = new WorldRenderer(this);
    connect(gameWidget->gameManager(), &GameManager::stateChanged,
            this, &MainWindow::handleGameStateChange);


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
    gameWidget->resetGame();
    m_stackWidget->setCurrentIndex(1);
}

void MainWindow::handleGameStateChange(GameState newState)
{
    static bool showingPopup = false;
    switch(newState) {
    case GameOver:
        if (!showingPopup) {
            showingPopup = true;
            showGameOverPopup();
            showingPopup = false;
        }
        break;
    case Playing:
        // Handle game start
        break;
    case MainMenu:
        m_stackWidget->setCurrentIndex(0);
        break;
    default:
        break;
    }
}

void MainWindow::showGameOverPopup()
{

    QMessageBox msgBox;
    msgBox.setWindowTitle("Game Over");
    msgBox.setText(QString("Your grandma didn't make it!\nFinal Score: %1")
                       .arg(gameWidget->gameManager()->score()));

    QPushButton* menuButton = msgBox.addButton("Return to Main Menu", QMessageBox::AcceptRole);

    msgBox.exec();

    // Since there's only one button, we don't need to check which was clicked
    m_stackWidget->setCurrentIndex(0); // Return to main menu
}





