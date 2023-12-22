#include "joueur.h"

Joueur::Joueur()
{

}

QTcpSocket *Joueur::getSockClient() const
{
    return sockClient;
}

void Joueur::setSockClient(QTcpSocket *newSockClient)
{
    sockClient = newSockClient;
}

int Joueur::getEpee() const
{
    return epee;
}

void Joueur::setEpee(int newEpee)
{
    epee = newEpee;
}

int Joueur::getPioche() const
{
    return pioche;
}

void Joueur::setPioche(int newPioche)
{
    pioche = newPioche;
}

int Joueur::getHache() const
{
    return hache;
}

void Joueur::setHache(int newHache)
{
    hache = newHache;
}

int Joueur::getHoue() const
{
    return houe;
}

void Joueur::setHoue(int newHoue)
{
    houe = newHoue;
}

QPoint Joueur::getPos() const
{
    return pos;
}

void Joueur::setPos(QPoint newPos)
{
    pos = newPos;
}

QString Joueur::getUsername() const
{
    return username;
}

void Joueur::setUsername(QString newUsername)
{
    username = newUsername;
}
