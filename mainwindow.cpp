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
    goalText->setObjectName("goalText");  // Add this line to set the object name
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
    gameWidget->resetGame();

    // Now switch to tutorial
    m_stackWidget->setCurrentIndex(1);
    updateTutorialForLevel(gameWidget->gameManager()->currentLevel());

    int currentLevel = gameWidget->gameManager()->currentLevel();
    qDebug() << "Current level: " << currentLevel; // Should now be 1

}

void MainWindow::tutorialPage(){
    gameWidget->resetGame();
    m_stackWidget->setCurrentIndex(2);
    int currentLevel = gameWidget->gameManager()->currentLevel();
    qDebug() << "Current level: " << currentLevel; // Should now be 1
}

void MainWindow::handleGameStateChange(GameState newState)
{
    static bool showingPopup = false;
    switch(newState) {
    case GameOver:
        if (!showingPopup) {
            showingPopup = true;
            gameWidget->pauseGame();
            showGameOverPopup();
            showingPopup = false;
        }
        break;
    case Level1:
    case Level2:
    case Level3:
        gameWidget->resumeGame();
        break;
    case MainMenu:
        gameWidget->pauseGame();
        m_stackWidget->setCurrentIndex(0);
        break;
    case GameClear:
        if(!showingPopup){
            showingPopup = true;
            gameWidget->pauseGame();
            showGameClearPopup();
            showingPopup = false;
        }
        break;
    case Tutorial:
        // No change needed here
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

    // Get current level
    int currentLevel = gameWidget->gameManager()->currentLevel();
    qDebug() << "Clear Popup Current level: " << currentLevel; // Should now be 1

    bool isMaxLevel = (currentLevel == 3);

    // Create dialog
    QDialog *gameClearDialog = new QDialog(this);
    gameClearDialog->setWindowTitle("Level " + QString::number(currentLevel) + " Clear");
    gameClearDialog->setFixedSize(500, 300);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(gameClearDialog);
    layout->setContentsMargins(20, 20, 20, 20);
    layout->setSpacing(15);

    // Title
    QLabel *titleLabel = new QLabel("Level " + QString::number(currentLevel) + " Clear", gameClearDialog);
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

    // Next Level Button - only show if not at max level
    QPushButton *nextButton = new QPushButton(isMaxLevel ? "Game Complete!" : "Next Level", gameClearDialog);
    nextButton->setFixedSize(200, 40);

    if (isMaxLevel) {
        nextButton->setEnabled(false);  // Disable if at max level
    }

    // Return Button -> triggers reject()
    connect(returnButton, &QPushButton::clicked, gameClearDialog, &QDialog::reject);

    // Next Button -> triggers accept()
    connect(nextButton, &QPushButton::clicked, gameClearDialog, &QDialog::accept);

    // Add widgets to layout
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addSpacing(20);
    layout->addWidget(scoreLabel);
    layout->addWidget(messageLabel);
    layout->addStretch();

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(nextButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(returnButton);
    layout->addLayout(buttonLayout);

    // Style the dialog (unchanged)
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

    // Show the Dialog result
    int result = gameClearDialog->exec();

    if (result == QDialog::Rejected) {
        // Return to menu - game will restart at the same level
        gameWidget->pauseGame(); // Make sure game is paused

        // Reset the physics world AND the game manager
        gameWidget->resetGame();

        // Make sure the gameManager is in a clean MainMenu state, NOT GameClear
        gameWidget->gameManager()->startSpecificLevel(currentLevel);

        // Now switch to the main menu
        m_stackWidget->setCurrentIndex(0);
    }
    else if (result == QDialog::Accepted && !isMaxLevel) {
        // Advance to next level and show tutorial with updated goals
        gameWidget->gameManager()->nextLevel();
        updateTutorialForLevel(gameWidget->gameManager()->currentLevel());
        m_stackWidget->setCurrentIndex(1);  // Navigate to tutorial page
    }


    gameClearDialog->deleteLater();
}

// New method to update tutorial goals for each level
void MainWindow::updateTutorialForLevel(int level) {
    // Find the goal text label in the tutorial widget
    QWidget* tutorialWidget = m_stackWidget->widget(1);
    QLabel* goalLabel = tutorialWidget->findChild<QLabel*>("goalText");

    if (goalLabel) {
        switch (level) {
        case 1:
            goalLabel->setText("Level 1: Collect these healing herbs to help your grandmother:\n"
                               "- Golden Currant: A bush with bright yellow flowers\n"
                               "- Mormon Tea: A green plant with jointed stems\n"
                               "- Creosote Bush: Has small yellow flowers and a strong smell\n\n"
                               "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.");
            break;
        case 2:
            goalLabel->setText("Level 2: Collect these healing herbs to help your grandmother:\n"
                               "- Golden Currant: A bush with bright yellow flowers\n"
                               "- Mormon Tea: A green plant with jointed stems\n"
                               "- Creosote Bush: Has small yellow flowers and a strong smell\n"
                               "- Osha: Has white flower clusters and fernlike leaves\n"
                               "- Prairie Flax: Has blue-purple flowers\n\n"
                               "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.");
            break;
        case 3:
            goalLabel->setText("Level 3 (Master Challenge): Collect ALL these healing herbs:\n"
                               "- Golden Currant: A bush with bright yellow flowers\n"
                               "- Mormon Tea: A green plant with jointed stems\n"
                               "- Creosote Bush: Has small yellow flowers and a strong smell\n"
                               "- Osha: Has white flower clusters and fernlike leaves\n"
                               "- Prairie Flax: Has blue-purple flowers\n"
                               "- Prickly Pear Cactus: Has flat, paddle-shaped segments\n"
                               "- Sagebrush: Silver-gray shrub with a strong fragrance\n\n"
                               "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.");
            break;
        default:
            goalLabel->setText("Collect herbs to help your grandmother and reach her house safely.");
            break;
        }
    }
}
