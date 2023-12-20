#ifndef CLIENTCRAWLER_H
#define CLIENTCRAWLER_H

#include <QWidget>
#include <QTcpSocket>
#include <QPoint>
#include <QGridLayout>
#include <QDebug>
#include <QBuffer>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QGraphicsRectItem>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientCrawler; }
QT_END_NAMESPACE

class ClientCrawler : public QWidget
{
    Q_OBJECT

public:
    ClientCrawler(QWidget *parent = nullptr);
    ~ClientCrawler();
    void EnvoyerCommande(QChar commande);
    void keyPressEvent(QKeyEvent *event);
private slots:
    void onQTcpSocketReadyRead();
    void onQTcpSocketConnected();
    void onQTcpSocketDisconnect();
    void onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError);
    void on_pushButtonConnexion_clicked();

private:
    Ui::ClientCrawler *ui;
    QGraphicsRectItem *joueur;
    QTcpSocket *socketClient;
    QGraphicsScene scene;

};
#endif // CLIENTCRAWLER_H
