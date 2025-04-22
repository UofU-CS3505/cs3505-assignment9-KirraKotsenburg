/**
 * @file mainwindow.h
 * @brief Defines the MainWindow class that manages the game's user interface
 *
 * @author Jason Chang
 *
 * Checked by Arthur Mo, Kirra Kotsenburg, Jay Lee
 */

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

/**
 * @brief MainWindow handles the UI for both the main menu and the game screen
 */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    /**
     * @brief Constructor for MainWindow
     * @param parent Parent widget
     */
    MainWindow(QWidget *parent = nullptr);

    /**
     * @brief Destructor
     */
    ~MainWindow();

    /**
     * @brief Handles state changes in the game
     * @param newState The new game state
     */
    void handleGameStateChange(GameState newState);

private slots:
    /**
     * @brief Slot to start the game when the "START" button is clicked
     */
    void startGame();

    /**
     * @brief Switches to the tutorial page
     */
    void tutorialPage();

    /**
     * @brief Displays the help dialog
     */
    void showHelpDialog();

private:
    Ui::MainWindow *ui;

    // Widget stack for switching between menu and game views
    QStackedWidget *m_stackWidget;

    WorldRenderer *gameWidget;

    /**
     * @brief Displays the game over popup
     */
    void showGameOverPopup();

    /**
     * @brief Displays the game clear popup
     */
    void showGameClearPopup();

    /**
     * @brief Updates the tutorial text for the given level
     * @param level The level number
     */
    void updateTutorialForLevel(int level);

    QPushButton *m_helpButton;

    /**
     * @brief Creates the help button
     */
    void createHelpButton();

    /**
     * @brief Updates the visibility of the help button based on game state
     * @param state The current game state
     */
    void updateHelpButtonVisibility(GameState state);

    /**
     * @brief Updates the position of the help button
     */
    void updateButtonPosition();
};

#endif // MAINWINDOW_H
