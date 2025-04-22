/**
 * @file mainwindow.cpp
 * @brief Implementation of the MainWindow class
 *
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg, Jay Lee
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "worldrenderer.h"
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <qmessagebox.h>
#include <QScrollArea>
#include <QLabel>
#include <QGroupBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {

    ui->setupUi(this);

    // Create a QStackedWidget: index 0 = Main Menu, index 1 = Tutorial, index 2 = Game
    m_stackWidget = new QStackedWidget(this);
    setCentralWidget(m_stackWidget);

    // ---------- Main Menu UI Setup ----------
    QWidget *menuWidget = new QWidget(this);
    QVBoxLayout *menuLayout = new QVBoxLayout(menuWidget);

    // Create title label with large font
    QLabel *titleLabel = new QLabel("Save Sick Grandma", menuWidget);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleLabel->setFont(titleFont);

    // Create info label with game instructions
    QLabel *infoLabel = new QLabel("You're collecting herbs to help your sick grandmother.\n"
                                   "But beware—some plants are poisonous!\n\n"
                                   "Use arrow keys to drive the tractor.", menuWidget);
    infoLabel->setAlignment(Qt::AlignCenter);

    // Create start button
    QPushButton *startButton = new QPushButton("START", menuWidget);
    startButton->setFixedSize(150, 40);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startGame);

    // Arrange widgets in the layout
    menuLayout->addStretch();
    menuLayout->addWidget(titleLabel);
    menuLayout->addWidget(infoLabel);
    menuLayout->addWidget(startButton, 0, Qt::AlignCenter);
    menuLayout->addStretch();

    // Style the menu widget
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
    goalText->setObjectName("goalText");  // Set object name for later access
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

    // Style the tutorial widget
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

    // Add all screens to the stacked widget
    m_stackWidget->addWidget(menuWidget);     // index 0: Main Menu
    m_stackWidget->addWidget(tutorialWidget); // index 1: Tutorial
    m_stackWidget->addWidget(gameWidget);     // index 2: Game Play

    // Set window size and title
    resize(1200, 600);
    setWindowTitle("Save Sick Grandma");

    // Initialize help button
    createHelpButton();
    updateHelpButtonVisibility(MainMenu);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startGame()
{
    // Reset game state
    gameWidget->resetGame();

    // Switch to tutorial screen
    m_stackWidget->setCurrentIndex(1);
    updateHelpButtonVisibility(Tutorial);
    updateTutorialForLevel(gameWidget->gameManager()->currentLevel());
}

void MainWindow::tutorialPage() {
    // Start game after tutorial
    gameWidget->resetGame();
    m_stackWidget->setCurrentIndex(2);
}

void MainWindow::handleGameStateChange(GameState newState)
{
    static bool showingPopup = false;
    updateHelpButtonVisibility(newState);

    switch(newState) {
    case GameOver:
        // Prevent multiple popups from appearing simultaneously
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
        // Resume gameplay for level states
        gameWidget->resumeGame();
        break;
    case MainMenu:
        // Return to main menu
        gameWidget->pauseGame();
        m_stackWidget->setCurrentIndex(0);
        break;
    case GameClear:
        // Show level completion popup
        if (!showingPopup) {
            showingPopup = true;
            gameWidget->pauseGame();
            showGameClearPopup();
            showingPopup = false;
        }
        break;
    case Tutorial:
        // No action needed for tutorial state
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
    gameOverDialog->exec();

    // Ensure game remains paused
    gameWidget->pauseGame();

    // Return to main menu
    m_stackWidget->setCurrentIndex(0);
    gameOverDialog->deleteLater();
}

void MainWindow::showGameClearPopup() {
    // Prevent multiple dialogs
    if (findChild<QDialog*>()) return;
    gameWidget->pauseGame();

    int currentLevel = gameWidget->gameManager()->currentLevel();
    bool isMaxLevel = (currentLevel == 3);

    // Create level clear dialog
    QDialog *gameClearDialog = new QDialog(this);
    gameClearDialog->setWindowTitle("Level " + QString::number(currentLevel) + " Clear");
    gameClearDialog->setMinimumSize(800, 600);

    // Create layouts
    QVBoxLayout *mainLayout = new QVBoxLayout(gameClearDialog);
    QScrollArea *scrollArea = new QScrollArea(gameClearDialog);
    scrollArea->setWidgetResizable(true);
    QWidget *scrollContent = new QWidget();
    QVBoxLayout *scrollLayout = new QVBoxLayout(scrollContent);

    // Header
    QLabel *titleLabel = new QLabel("Level " + QString::number(currentLevel) + " Complete!", scrollContent);
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    QLabel *scoreLabel = new QLabel(QString("Final Score: %1").arg(gameWidget->gameManager()->score()), scrollContent);
    scoreLabel->setAlignment(Qt::AlignCenter);
    scoreLabel->setFont(QFont(scoreLabel->font().family(), 16));

    scrollLayout->addWidget(titleLabel);
    scrollLayout->addWidget(scoreLabel);
    scrollLayout->addSpacing(20);

    // Plants header
    QLabel *plantsHeader = new QLabel("Medicinal Plants Collected:", scrollContent);
    plantsHeader->setFont(QFont(plantsHeader->font().family(), 16, QFont::Bold));
    scrollLayout->addWidget(plantsHeader);
    scrollLayout->addSpacing(10);

    // Define plant data structure
    struct PlantData {
        QString name;
        QString description;
        QString imagePath;
    };

    // Define all plants with their information
    QMap<QString, PlantData> allPlants = {
        {"Golden Currant", {
                               "Golden Currant",
                               "- Affected parts: Berries and leaves.\n"
                               "- Effects: Non-toxic in moderate amounts.\n"
                               "- Use: Rich in antioxidants and vitamins; berries eaten fresh or dried.\n"
                               " Leaves brewed as tea to reduce inflammation and support immunity.",
                               ":/safe/Plants/Safe_Plants/golden_currant.jpg"
                           }},
        {"Mormon Tea", {
                           "Mormon Tea",
                           "- Affected parts: Stems.\n"
                           "- Effects: Mild stimulant; can raise heart rate if consumed in large quantities.\n"
                           "- Use: Used to stop bleeding, reduce fever, and treat digestive issues.\n"
                           " Was historically brewed as tea.",
                           ":/safe/Plants/Safe_Plants/mormon_tea.jpg"
                       }},
        {"Creosote Bush", {
                              "Creosote Bush",
                              "- Affected parts: Leaves and stems.\n"
                              "- Effects: May cause liver or kidney irritation in large amounts.\n"
                              "- Use: Traditionally used to boost the immune system and reduce symptoms of infections.\n"
                              " Has antimicrobial properties and was used by Indigenous peoples for colds and wounds.",
                              ":/safe/Plants/Safe_Plants/creosote_bush.jpg"
                          }},
        {"Osha", {
                     "Osha",
                     "- Affected parts: Root.\n"
                     "- Effects: Generally safe in small doses.\n"
                     "- Use: Used for respiratory issues, sleep aid, and inflammation.\n"
                     " Often chewed or brewed into tea for sore throats and colds.\n"
                     " Caution: Resembles Water Hemlock.",
                     ":/safe/Plants/Safe_Plants/osha.jpg"
                 }},
        {"Prairie Flax", {
                             "Prairie Flax",
                             "- Affected parts: Seeds and leaves.\n"
                             "- Effects: Non-toxic; excessive seed intake may cause digestive discomfort.\n"
                             "- Use: Seeds soothe digestion and freshen breath; leaves contain menthol to relieve nasal congestion.\n"
                             " Sometimes used for mild respiratory relief.\n"
                             " Caution: Resembles Lupine/Blue Bonnet.",
                             ":/safe/Plants/Safe_Plants/prairie_flax.jpg"
                         }},
        {"Prickly Pear Cactus", {
                                    "Prickly Pear Cactus",
                                    "- Affected parts: Pads and fruit.\n"
                                    "- Effects: Safe when de-spined and properly prepared.\n"
                                    "- Use: Eaten for fiber and antioxidants; used to regulate blood sugar and hydration.\n"
                                    " Pads and fruit are both edible and nutritionally beneficial.",
                                    ":/safe/Plants/Safe_Plants/prickly_pear_cactus.jpg"
                                }}
    };

    // Get plants for current level
    QStringList levelPlantNames;
    switch(currentLevel) {
    case 1: levelPlantNames = {"Golden Currant", "Mormon Tea", "Creosote Bush"}; break;
    case 2: levelPlantNames = {"Golden Currant", "Mormon Tea", "Creosote Bush", "Osha", "Prairie Flax"}; break;
    case 3: levelPlantNames = {"Golden Currant", "Mormon Tea", "Creosote Bush", "Osha", "Prairie Flax", "Prickly Pear Cactus"}; break;
    }

    // Display plant information for each plant in this level
    for (const QString &plantName : levelPlantNames) {
        if (!allPlants.contains(plantName)) continue;

        const PlantData &plant = allPlants[plantName];
        QGroupBox *plantGroup = new QGroupBox(plant.name, scrollContent);
        QHBoxLayout *plantLayout = new QHBoxLayout(plantGroup);

        // Plant image
        QLabel *imageLabel = new QLabel();
        QPixmap pixmap(plant.imagePath);
        if (!pixmap.isNull()) {
            imageLabel->setPixmap(pixmap.scaledToWidth(150, Qt::SmoothTransformation));
        }

        // Plant description
        QLabel *descLabel = new QLabel(plant.description);
        descLabel->setWordWrap(true);
        descLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);

        plantLayout->addWidget(imageLabel);
        plantLayout->addWidget(descLabel, 1);
        scrollLayout->addWidget(plantGroup);
    }

    scrollLayout->addStretch();
    scrollContent->setLayout(scrollLayout);
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *nextButton = new QPushButton(isMaxLevel ? "Game Complete!" : "Next Level");
    QPushButton *returnButton = new QPushButton("Return to Main Menu");

    nextButton->setFixedSize(150, 40);
    returnButton->setFixedSize(150, 40);

    // Disable "Next Level" button on final level
    if (isMaxLevel) nextButton->setEnabled(false);

    buttonLayout->addStretch();
    buttonLayout->addWidget(nextButton);
    buttonLayout->addWidget(returnButton);
    buttonLayout->addStretch();
    mainLayout->addLayout(buttonLayout);

    // Style the dialog
    gameClearDialog->setStyleSheet(
        "QDialog { background-color: #2c3e50; }"
        "QLabel { color: #ecf0f1; }"
        "QGroupBox { border: 1px solid #3498db; border-radius: 5px; margin-top: 10px; }"
        "QGroupBox::title { color: #ecf0f1; subcontrol-origin: margin; left: 10px; }"
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 16px;"
        "}"
        "QPushButton:hover { background-color: #2980b9; }"
        );

    // Connect button signals
    connect(returnButton, &QPushButton::clicked, gameClearDialog, &QDialog::reject);
    connect(nextButton, &QPushButton::clicked, gameClearDialog, &QDialog::accept);

    // Show dialog and handle result
    int result = gameClearDialog->exec();

    if (result == QDialog::Rejected) {
        // Return to main menu
        gameWidget->pauseGame();
        gameWidget->resetGame();
        gameWidget->gameManager()->startSpecificLevel(currentLevel);
        m_stackWidget->setCurrentIndex(0);
    }
    else if (result == QDialog::Accepted && !isMaxLevel) {
        // Proceed to next level
        gameWidget->gameManager()->nextLevel();
        updateTutorialForLevel(gameWidget->gameManager()->currentLevel());
        m_stackWidget->setCurrentIndex(1);
    }

    gameClearDialog->deleteLater();
}

void MainWindow::updateTutorialForLevel(int level) {
    // Find the goal text label in the tutorial widget
    QWidget* tutorialWidget = m_stackWidget->widget(1);
    QLabel* goalLabel = tutorialWidget->findChild<QLabel*>("goalText");

    if (goalLabel) {
        // Update goal text based on current level
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

void MainWindow::createHelpButton() {
    // Create help button with question mark
    m_helpButton = new QPushButton("?", this);
    m_helpButton->setFixedSize(40, 40);

    // Style the help button
    m_helpButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 20px;"
        "   font-size: 18px;"
        "   font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2980b9;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #1d6fa5;"
        "}"
        );

    // Position button and hide initially
    updateButtonPosition();
    m_helpButton->hide();

    // Connect button to help dialog
    connect(m_helpButton, &QPushButton::clicked, this, &MainWindow::showHelpDialog);
}

void MainWindow::showHelpDialog() {
    // Create the help dialog
    QDialog *helpDialog = new QDialog(this);
    helpDialog->setWindowTitle("Game Help");
    helpDialog->setWindowModality(Qt::WindowModal);
    helpDialog->setFixedSize(500, 400);

    // Create layout
    QVBoxLayout *layout = new QVBoxLayout(helpDialog);
    layout->setContentsMargins(20, 20, 20, 20);

    // Get help text based on current level
    int currentLevel = gameWidget->gameManager()->currentLevel();
    QString helpText;

    switch (currentLevel) {
    case 1:
        helpText = "Level 1: Collect these healing herbs to help your grandmother:\n"
                   "- Golden Currant: A bush with bright yellow flowers\n"
                   "- Mormon Tea: A green plant with jointed stems\n"
                   "- Creosote Bush: Has small yellow flowers and a strong smell\n\n"
                   "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.";
        break;
    case 2:
        helpText = "Level 2: Collect these healing herbs to help your grandmother:\n"
                   "- Golden Currant: A bush with bright yellow flowers\n"
                   "- Mormon Tea: A green plant with jointed stems\n"
                   "- Creosote Bush: Has small yellow flowers and a strong smell\n"
                   "- Osha: Has white flower clusters and fernlike leaves\n"
                   "- Prairie Flax: Has blue-purple flowers\n\n"
                   "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.";
        break;
    case 3:
        helpText = "Level 3 (Master Challenge): Collect ALL these healing herbs:\n"
                   "- Golden Currant: A bush with bright yellow flowers\n"
                   "- Mormon Tea: A green plant with jointed stems\n"
                   "- Creosote Bush: Has small yellow flowers and a strong smell\n"
                   "- Osha: Has white flower clusters and fernlike leaves\n"
                   "- Prairie Flax: Has blue-purple flowers\n"
                   "- Prickly Pear Cactus: Has flat, paddle-shaped segments\n"
                   "- Sagebrush: Silver-gray shrub with a strong fragrance\n\n"
                   "Avoid poisonous plants! Collecting 3 poisonous plants will end the game.";
        break;
    default:
        helpText = "Collect herbs to help your grandmother and reach her house safely.";
        break;
    }

    // Create text label
    QLabel *helpLabel = new QLabel(helpText, helpDialog);
    helpLabel->setWordWrap(true);
    helpLabel->setAlignment(Qt::AlignLeft);

    // Create close button
    QPushButton *closeButton = new QPushButton("Close", helpDialog);
    closeButton->setFixedSize(100, 30);
    connect(closeButton, &QPushButton::clicked, helpDialog, &QDialog::accept);

    // Add widgets to layout
    layout->addWidget(helpLabel);
    layout->addWidget(closeButton, 0, Qt::AlignCenter);

    // Style the dialog
    helpDialog->setStyleSheet(
        "QDialog {"
        "   background-color: #2c3e50;"
        "}"
        "QLabel {"
        "   color: white;"
        "   font-size: 14px;"
        "}"
        "QPushButton {"
        "   background-color: #3498db;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   font-size: 14px;"
        "   padding: 5px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #2980b9;"
        "}"
        );

    // Pause game while dialog is open
    gameWidget->pauseGame();
    helpDialog->exec();
    gameWidget->resumeGame();

    // Clean up
    helpDialog->deleteLater();
}

void MainWindow::updateHelpButtonVisibility(GameState state) {
    // Only show help button during gameplay levels
    bool shouldShow = (state == Level1 || state == Level2 || state == Level3);
    m_helpButton->setVisible(shouldShow);

    if (shouldShow) {
        m_helpButton->raise();  // Ensure button is on top
        updateButtonPosition(); // Position correctly
    }
}

void MainWindow::updateButtonPosition() {
    if (m_helpButton) {
        // Center the button horizontally at the top of the window
        int xPos = (width() - m_helpButton->width()) / 2;
        m_helpButton->move(xPos, 20);
    }
}
