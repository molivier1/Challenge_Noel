#ifndef JOUEUR_H
#define JOUEUR_H

#include <QTcpSocket>
#include <QPoint>

#include "inventaire.h"

class Joueur
{
public:
    Joueur();

    QTcpSocket *getSockClient() const;
    void setSockClient(QTcpSocket *newSockClient);

    int getEpee() const;
    void setEpee(int newEpee);

    int getPioche() const;
    void setPioche(int newPioche);

    int getHache() const;
    void setHache(int newHache);

    int getHoue() const;
    void setHoue(int newHoue);

    QPoint getPos() const;
    void setPos(QPoint newPos);

    QString getUsername() const;
    void setUsername(QString newUsername);

    Inventaire getInventaire() const;
    void setInventaire(Inventaire newInventaire);

private:
    QTcpSocket *sockClient;

    int epee;
    int pioche;
    int hache;
    int houe;

    QPoint pos;

    QString username;

    Inventaire inventaire;
};

#endif // JOUEUR_H
