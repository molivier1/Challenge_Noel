#ifndef SERVEURCRAWLER_H
#define SERVEURCRAWLER_H

#include <QWidget>
#include <QTcpServer>
#include <QTcpSocket>
#include <QGridLayout>
#include <QHostInfo>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QToolButton>
#include <QRandomGenerator>
#include <QDateTime>
#include <QThread>
#include <QTimer>
#include <QBuffer>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPixmap>
#define TAILLE 20

QT_BEGIN_NAMESPACE
namespace Ui { class ServeurCrawler; }
QT_END_NAMESPACE

class ServeurCrawler : public QWidget
{
    Q_OBJECT

public:
    explicit ServeurCrawler(QWidget *parent = nullptr);
    ~ServeurCrawler();

private slots:
    void onQTcpServer_newConnection();
    void onQTcpSocket_readyRead();
    void onQTcpSocket_disconnected();

    void on_pushButtonLancer_clicked();

private:
    void   EnvoyerDonnees(QTcpSocket *client,QPoint pt, QString msg);
    void   afficherGrille();
    void   viderGrille();
    QPoint donnerPositionUnique();
    double calculerDistance(QPoint pos);
    void   attendre(int millisecondes);

    Ui::ServeurCrawler *ui;
    QTcpServer *socketEcoute;
    QList <QTcpSocket *>listeSocketsClient;
    QList <QPoint>listePositions;
    QPoint tresor;
    QGraphicsView *vue;
    QGraphicsScene *scene;
    QGridLayout *grille;
    QMediaPlayer *player;
    QAudioOutput *audiOutput;
};
#endif // SERVEURCRAWLER_H
