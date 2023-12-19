#include "fenetrejeu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FenetreJeu w;
    w.show();
    return a.exec();
}
