/**
 * @file main.cpp
 * @brief Main launching class for the "Save Sick Grandma" educational game
 *
 * @author Team AJKJ
 *
 * This program launches an educational game that teaches players about Utah's native plants,
 * focusing on distinguishing between medicinal herbs and dangerous/poisonous plants.
 * Players drive a vehicle to collect healing herbs for their sick grandmother while
 * avoiding poisonous plants across three progressive levels.
 */

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
