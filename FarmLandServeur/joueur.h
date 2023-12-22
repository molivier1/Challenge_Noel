#ifndef JOUEUR_H
#define JOUEUR_H

#include <QTcpSocket>
#include <QPoint>

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

    int epee;
    int pioche;
    int hache;
    int houe;

private:
    QTcpSocket *sockClient;


    QPoint pos;

    QString username;
};

#endif // JOUEUR_H
