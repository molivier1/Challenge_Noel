#ifndef FENETREJEU_H
#define FENETREJEU_H

#include "qgraphicsview.h"
#include <QWidget>
#include <QList>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QBuffer>
#include <QKeyEvent>
#include <QGraphicsScene>
#include "QGraphicsViewPerso.h"

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

private:
    Ui::FenetreJeu *ui;
    int zoneCommune;
    QGraphicsScene maScene;
    QGraphicsViewPerso *maVue;
    QGraphicsRectItem *rectangle;
    QGraphicsView zone;
    QTcpSocket *socketFarmLand;
    const QString ressources[NBRESSOURCES]={"ble,pierre,bois,fer,carotte,bouleau,patate,diamant,sapin"};
private slots:
    void onQGraphicsViewPerso_positionSouris(QPoint pos);
};
#endif // FENETREJEU_H
