#ifndef SERVEUR_H
#define SERVEUR_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>

#include <QBuffer>
#include <QNetworkInterface>

#include "joueur.h"
#include "inventaire.h"

#include <QGraphicsScene>
#include "qgraphicsviewperso.h"

#include <QVBoxLayout>
#include <QHBoxLayout>

#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class Serveur; }
QT_END_NAMESPACE

class Serveur : public QWidget
{
    Q_OBJECT

public:
    Serveur(QWidget *parent = nullptr);
    ~Serveur();

private slots:
    void onQTcpServer_newConnection();
    void onQTcpSocket_readyRead();
    void onQTcpSocket_disconnected();
    void onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError);

    void on_pushButtonLancerServeur_clicked();
    void onQTimer_timeout();

private:
    Ui::Serveur *ui;

    QList <QTcpSocket *> listeSocketsClients;
    QList <Joueur *> listeJoueurs;

    QTcpServer socketServeur;

    void EnvoyerDonnees(QTcpSocket *client);

    void CreerJoueur(QTcpSocket *client);

    int getIndexClient(QTcpSocket *client);

    void envoyerDonneesAll();

    Inventaire coffreCommun;

    QGraphicsScene maScene;
    QGraphicsViewPerso *maVue;

    bool checkPositions(Joueur *joueur);

    int checkZone(Joueur *joueur);

    bool zone2Verif;
    bool zone3Verif;

    QString messageValide;

    QTimer *timer;
};
#endif // SERVEUR_H