#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <qmessagebox.h>

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
    // Prevent multiple dialogs
    if (findChild<QDialog*>()) return;

    // Create a custom dialog
    QDialog *gameOverDialog = new QDialog(this);
    gameOverDialog->setWindowTitle("Game Over");
    gameOverDialog->setFixedSize(400, 300);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(gameOverDialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel("Game Over", gameOverDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    // Score display
    QLabel *scoreLabel = new QLabel(QString("Final Score: %1").arg(gameWidget->gameManager()->score()), gameOverDialog);
    scoreLabel->setAlignment(Qt::AlignCenter);
    QFont scoreFont = scoreLabel->font();
    scoreFont.setPointSize(18);
    scoreLabel->setFont(scoreFont);

    // Message
    QLabel *messageLabel = new QLabel("Your grandma didn't make it!\nBetter luck next time!", gameOverDialog);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    // Return button
    QPushButton *returnButton = new QPushButton("Return to Main Menu", gameOverDialog);
    returnButton->setFixedSize(200, 40);
    connect(returnButton, &QPushButton::clicked, gameOverDialog, &QDialog::accept);

    // Add widgets to layout
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(scoreLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();
    layout->addWidget(returnButton, 0, Qt::AlignCenter);

    // Style the dialog
    gameOverDialog->setStyleSheet(
        "QDialog { background-color: #2c3e50; }"
        "QLabel { color: #ecf0f1; }"
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        );

    // Show dialog modally
    //gameWidget->pauseGame();  // Make sure to implement pauseGame() in WorldRenderer
    gameOverDialog->exec();
    //gameWidget->resumeGame(); // Implement resumeGame()

    // Cleanup and return to menu
    m_stackWidget->setCurrentIndex(0);
    gameOverDialog->deleteLater();
}
