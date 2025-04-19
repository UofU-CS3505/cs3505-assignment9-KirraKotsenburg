#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <qmessagebox.h>

#include <QLabel>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Create a QStackedWidget: index 0 = Main Menu, index 1 = Tutorial, index 2 = Game
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
    startButton->setFixedSize(150, 40);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);

    menuLayout->addStretch();
    menuLayout->addWidget(titleLabel);
    menuLayout->addWidget(infoLabel);
    menuLayout->addWidget(startButton, 0, Qt::AlignCenter);
    menuLayout->addStretch();


    menuWidget->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
    );

    // ---------- Tutorial Page UI Setup ----------
    QWidget *tutorialWidget = new QWidget(this);
    QVBoxLayout *tutorialLayout = new QVBoxLayout(tutorialWidget);

    // Title
    QLabel *tutorialTitle = new QLabel("TUTORIAL", tutorialWidget);
    tutorialTitle->setAlignment(Qt::AlignCenter);
    tutorialTitle->setFont(titleFont);

    // Purpose section
    QLabel *purposeLabel = new QLabel("Purpose of the Game:", tutorialWidget);
    purposeLabel->setFont(QFont(purposeLabel->font().family(), 14, QFont::Bold));

    QLabel *purposeText = new QLabel("The purpose of this game is to educate about Utah's poisonous plants. \n"
                                     "Through gameplay, you will learn to identify various herbs and poisonous \n"
                                     "plants native to Utah. This knowledge could be valuable in real-life situations.",
                                     tutorialWidget);
    purposeText->setWordWrap(true);
    purposeText->setAlignment(Qt::AlignLeft);

    // How to play section
    QLabel *howToPlayLabel = new QLabel("How To Play:", tutorialWidget);
    howToPlayLabel->setFont(QFont(howToPlayLabel->font().family(), 14, QFont::Bold));

    QLabel *howToPlayText = new QLabel("Use the LEFT and RIGHT arrow keys to move your vehicle. \n"
                                       "When you encounter a plant, you must decide whether to collect it or avoid it. \n"
                                       "Collect beneficial herbs but avoid poisonous plants! \n"
                                       "Get to the house your grandmother is waiting for while collecting plants!",
                                       tutorialWidget);
    howToPlayText->setWordWrap(true);
    howToPlayText->setAlignment(Qt::AlignLeft);

    // Goal section
    QLabel *goalLabel = new QLabel("Goal of the Game:", tutorialWidget);
    goalLabel->setFont(QFont(goalLabel->font().family(), 14, QFont::Bold));

    QLabel *goalText = new QLabel("Put it later~", tutorialWidget);
    goalText->setWordWrap(true);
    goalText->setAlignment(Qt::AlignLeft);

    // Warning Section
    QLabel *warningLabel = new QLabel("WARNING", tutorialWidget);
    goalLabel->setFont(QFont(goalLabel->font().family(), 14, QFont::Bold));

    QLabel *warningText = new QLabel("Don't eat those plants outside unless you're a professional forager.");
    warningText->setWordWrap(true);
    warningText->setAlignment(Qt::AlignLeft);

    // OK button
    QPushButton *proceedButton = new QPushButton("OK", tutorialWidget);
    proceedButton->setFixedSize(150, 40);
    connect(proceedButton, &QPushButton::clicked, this, &MainWindow::tutorialPage);

    // Add widgets to layout with proper spacing
    tutorialLayout->addStretch();
    tutorialLayout->addWidget(tutorialTitle);
    tutorialLayout->addSpacing(20);
    tutorialLayout->addWidget(purposeLabel);
    tutorialLayout->addWidget(purposeText);
    tutorialLayout->addSpacing(15);
    tutorialLayout->addWidget(howToPlayLabel);
    tutorialLayout->addWidget(howToPlayText);
    tutorialLayout->addSpacing(15);
    tutorialLayout->addWidget(goalLabel);
    tutorialLayout->addWidget(goalText);
    tutorialLayout->addSpacing(15);
    tutorialLayout->addWidget(warningLabel);
    tutorialLayout->addWidget(warningText);
    tutorialLayout->addSpacing(20);
    tutorialLayout->addWidget(proceedButton, 0, Qt::AlignCenter);
    tutorialLayout->addStretch();


    tutorialWidget->setStyleSheet(

        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
    );

    // ---------- Game Screen UI Setup ----------
    gameWidget = new WorldRenderer(this);
    connect(gameWidget->gameManager(), &GameManager::stateChanged,
            this, &MainWindow::handleGameStateChange);

    // Add both screens to the stacked widget
    m_stackWidget->addWidget(menuWidget);   // index 0: Main=Menu
    m_stackWidget->addWidget(tutorialWidget);   // index 1: tutorial
    m_stackWidget->addWidget(gameWidget);   // index 1: gamePlay

    // Set window size and title
    resize(1200, 600);
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

void MainWindow::tutorialPage(){
    gameWidget->resetGame();
    m_stackWidget->setCurrentIndex(2);
}

void MainWindow::handleGameStateChange(GameState newState)
{
    static bool showingPopup = false;
    switch(newState) {
    case GameOver:
        if (!showingPopup) {
            showingPopup = true;
            gameWidget->pauseGame();  // Pause before showing popup
            showGameOverPopup();
            showingPopup = false;
        }
        break;
    case Playing:
        gameWidget->resumeGame();  // Resume when returning to game
        break;
    case MainMenu:
        gameWidget->pauseGame();  // Pause before switching to menu
        m_stackWidget->setCurrentIndex(0);
        break;
    case GameClear:
        if(!showingPopup){
            showingPopup = true;
            gameWidget->pauseGame();  // Pause before showing popup
            showGameClearPopup();
            showingPopup = false;
        }
        break;
    default:
        break;
    }
}

void MainWindow::showGameOverPopup()
{
    // Prevent multiple dialogs
    if (findChild<QDialog*>()) return;

    gameWidget->pauseGame();
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

    gameWidget->pauseGame();
    // Cleanup and return to menu
    m_stackWidget->setCurrentIndex(0);
    gameOverDialog->deleteLater();
}

void MainWindow::showGameClearPopup(){
    // Prevent multiple dialogs
    if (findChild<QDialog*>()) return;

    gameWidget->pauseGame();
    // Create a custom dialog
    QDialog *gameClearDialog = new QDialog(this);
    gameClearDialog->setWindowTitle("Game Clear");
    gameClearDialog->setFixedSize(500, 300);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(gameClearDialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel("Game Clear", gameClearDialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    // Score display
    QLabel *scoreLabel = new QLabel(QString("Final Score: %1").arg(gameWidget->gameManager()->score()), gameClearDialog);
    scoreLabel->setAlignment(Qt::AlignCenter);
    QFont scoreFont = scoreLabel->font();
    scoreFont.setPointSize(18);
    scoreLabel->setFont(scoreFont);

    // Message
    QLabel *messageLabel = new QLabel("Your grandma makes it!", gameClearDialog);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    // Return button
    QPushButton *returnButton = new QPushButton("Return to Main Menu", gameClearDialog);
    returnButton->setFixedSize(200, 40);

    // Next Level Button
    QPushButton *NextButton = new QPushButton("Next Level", gameClearDialog);
    NextButton->setFixedSize(200, 40);

    // Return Button -> triggers reject()
    connect(returnButton, &QPushButton::clicked, gameClearDialog, &QDialog::reject);

    // Next Button -> triggers accept()
    connect(NextButton, &QPushButton::clicked, gameClearDialog, &QDialog::accept);

    // Add widgets to layout
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(scoreLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(NextButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(returnButton);
    layout->addLayout(buttonLayout);

    // Style the dialog
    gameClearDialog->setStyleSheet(
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

    // show the Dialog result
    int result = gameClearDialog->exec();

    if (result == QDialog::Rejected) {
        m_stackWidget->setCurrentIndex(0);
    }
    else if (result == QDialog::Accepted) {

    }

    gameWidget->pauseGame();
    gameClearDialog->deleteLater();
}
