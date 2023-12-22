#ifndef FENETREJEU_H
#define FENETREJEU_H

#include "inventaire.h"
#include "qgraphicsview.h"
#include <QWidget>
#include <QList>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QBuffer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include "QGraphicsViewPerso.h"
#include <QMediaPlayer>
#include <QAudioOutput>

#define NBRESSOURCES 9

QT_BEGIN_NAMESPACE
namespace Ui { class FenetreJeu; }
QT_END_NAMESPACE

class FenetreJeu : public QWidget
{
    Q_OBJECT

public:
    FenetreJeu(QWidget *parent = nullptr);
    ~FenetreJeu();

private slots:

    void on_pushButtonNouvelleZone_clicked();
    void onQTcpSocket_connected();
    void onQTcpSocket_disconnected();
    void onQTcpSocket_readyRead();
    void onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError);
    void EnvoyerCommande(QChar commande);
    void on_pushButtonConnexionServeur_clicked();
    void keyPressEvent(QKeyEvent *event);

    void obtenirCoordonneesCurseur();
    void onQMediaPlayer_playbackStateChanged(QMediaPlayer::PlaybackState newState);
    void onQGraphicsViewPerso_positionSouris(QPoint pos);

private:
    Ui::FenetreJeu *ui;
    int zoneCommune;
    QGraphicsScene maScene;
    QGraphicsViewPerso *maVue;
    QGraphicsPixmapItem *joueur;
    QGraphicsView zone;
    QTcpSocket *socketFarmLand;
    Inventaire coffre;
    const QString ressources[NBRESSOURCES]={"ble,pierre,bois,fer,carotte,bouleau,patate,diamant,sapin"};
    QString message;
    QString nomFichier;
    QMediaPlayer player;
    QAudioOutput sortieAudio;

    void actualiserRessources();
    void joueurZone1();

    bool zone2;
    bool zone3;

    QGraphicsPolygonItem *zone2Item;
    QGraphicsPolygonItem *zone3Item;

};
#endif // FENETREJEU_H
