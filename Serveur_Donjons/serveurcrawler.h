#ifndef SERVEURCRAWLER_H
#define SERVEURCRAWLER_H

#include <QWidget>
#include <QTcpServer>
#include <QAbstractSocket>
#include <QTcpSocket>
#include <QNetworkInterface>
#include <QToolButton>
#include <QRandomGenerator>
#include <QDateTime>
#include <QBuffer>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui { class ServeurCrawler; }
QT_END_NAMESPACE

class ServeurCrawler : public QWidget
{
    Q_OBJECT

public:
    ServeurCrawler(QWidget *parent = nullptr);
    ~ServeurCrawler();
    QPoint DonnerPositionUnique();

private slots:
    void onQTcpServerNewConnection();
    void onQTcpSocketReadyRead();
    void onQTcpSocketDisconnect();
    void onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError);
    void EnvoyerDonnees(QTcpSocket* client, QPoint pt, QString msg);


    void on_pushButtonLancer_clicked();

private:
    Ui::ServeurCrawler *ui;
    QTcpServer *socketEcouteServeur;
    QList<QTcpSocket *> listeSocketClient;
    QList<QPoint> listePositions;
};
#endif // SERVEURCRAWLER_H
