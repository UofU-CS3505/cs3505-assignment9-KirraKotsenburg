#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include "gamemanager.h"
#include "worldrenderer.h"
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// MainWindow handles the UI for both the main menu and the game screen
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // Constructor
    MainWindow(QWidget *parent = nullptr);

    // Destructor
    ~MainWindow();

    void handleGameStateChange(GameState newState);

private slots:
    // Slot to start the game when the "START" button is clicked
    void startGame();
    void tutorialPage();
    void showHelpDialog();

private:
    Ui::MainWindow *ui;

    // Widget stack for switching between menu and game views
    QStackedWidget *m_stackWidget;

    WorldRenderer *gameWidget;

    void showGameOverPopup();

    void showGameClearPopup();
    void updateTutorialForLevel(int level);
    QPushButton *m_helpButton;

    void createHelpButton();
    void updateHelpButtonVisibility(GameState state);

    void updateButtonPosition();
};

#endif // MAINWINDOW_H
