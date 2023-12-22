#ifndef CLIENTCRAWLER_H
#define CLIENTCRAWLER_H

#include "qgraphicsboulehorizontalitem.h"
#include <QWidget>
#include <QTcpSocket>
#include <QPoint>
#include <QGridLayout>
#include <QDebug>
#include <QBuffer>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsRectItem>
#include <QGraphicsView>
#include <QTimer>

#define TAILLE 1

QT_BEGIN_NAMESPACE
namespace Ui { class ClientCrawler; }
QT_END_NAMESPACE

class ClientCrawler : public QWidget
{
    Q_OBJECT

public:
    ClientCrawler(QWidget *parent = nullptr);
    ~ClientCrawler();
private slots:
    void onQTcpSocketReadyRead();
    void onQTcpSocketConnected();
    void onQTcpSocketDisconnect();
    void onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError);
    void on_pushButtonConnexion_clicked();
    void EnvoyerCommande(QChar commande);
    void keyPressEvent(QKeyEvent *event);
    void ViderGrille();
private:
    Ui::ClientCrawler *ui;
    QGraphicsRectItem *joueur;
    QTcpSocket *socketClient;
    QGraphicsScene *scene;
    QGridLayout *grille;
    QPoint position;
    QGraphicsView *vue;
    QTimer timer;
    QGraphicsBouleHorizontalItem *boule;
    QGraphicsBouleHorizontalItem *boule2;
    QGraphicsPixmapItem *recPerso;



};
#endif // CLIENTCRAWLER_H
