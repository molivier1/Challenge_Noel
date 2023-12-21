#include "serveurcrawler.h"
#include "ui_serveurcrawler.h"

ServeurCrawler::ServeurCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServeurCrawler)
{
    ui->setupUi(this);
    socketEcouteServeur = new QTcpServer(this);

}

ServeurCrawler::~ServeurCrawler()
{
    delete ui;
}

void ServeurCrawler::onQTcpServerNewConnection()
{

    QTcpSocket *client=socketEcouteServeur->nextPendingConnection();

    // connection signal/slot pour la socket
    connect(client,&QTcpSocket::disconnected,this,&ServeurCrawler::onQTcpSocketDisconnect);
    connect(client,&QTcpSocket::readyRead,this,&ServeurCrawler::onQTcpSocketReadyRead);
    connect(client,&QTcpSocket::errorOccurred,this,&ServeurCrawler::onQTcpSocketErrorOccured);
    // création et ajout du client dans la liste des clients
    listeSocketClient.append(client);
    QHostAddress addresseClient = client->peerAddress();
    qDebug() << "Client : " + addresseClient.toString();
    QPoint pos=DonnerPositionUnique();
    listePositions.append(pos);
}

void ServeurCrawler::onQTcpSocketReadyRead()
{
    QChar   commande;
    quint16 taille=0;
    QTcpSocket *socketVersClient=qobject_cast<QTcpSocket*>(sender());
    int indexClient = listeSocketClient.indexOf(socketVersClient);
    QPoint pos = listePositions.at(indexClient);

    if (socketVersClient->bytesAvailable() >= (qint64)sizeof(taille))
    {
        QDataStream in(socketVersClient);
        in >> taille;

        if (socketVersClient->bytesAvailable() >= (qint64)taille)
        {
            // decomposition de la trame
            in >> commande;
            switch (commande.toLatin1()) {
            case 'D':
                pos.setY((pos.y()+11));
                break;
            case 'U':
                pos.setY(abs(pos.y()-11));
                break;
            case 'R':
                pos.setX((pos.x()+11));
                break;
            case 'L':
                pos.setX(abs(pos.x()-11));
                break;
            }
            // collision?
            if (listePositions.contains(pos))
            {
                int indexAutre = listePositions.indexOf(pos);
                listePositions.replace(indexClient,DonnerPositionUnique());
                listePositions.replace(indexAutre,DonnerPositionUnique());
                // envoyer position aux clients
                EnvoyerDonnees(socketVersClient,listePositions.at(indexClient),"collision");
                EnvoyerDonnees(listeSocketClient.at(indexAutre),listePositions.at(indexAutre),"collision");
            }
            else
            {
                   // autre

                    listePositions.replace(indexClient,pos);
                    // envoyer position au client
                    EnvoyerDonnees(socketVersClient,pos,"vide");

            }
        }
    }
}

void ServeurCrawler::onQTcpSocketDisconnect()
{
    QTcpSocket *client=qobject_cast<QTcpSocket*>(sender());
    int indexClient = listeSocketClient.indexOf(client);
    listeSocketClient.removeAt(indexClient);
    listePositions.removeAt(indexClient);
    qDebug() << "Client deconnecté";
}

void ServeurCrawler::onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // le paramètre n'est pas utilisé dans cette méthode
    qDebug()<< socketEcouteServeur->errorString();
}

void ServeurCrawler::on_pushButtonLancer_clicked()
{
    if(socketEcouteServeur->listen(QHostAddress::Any,ui->spinBoxPort->value())){
        QList<QHostAddress> listeAdresse = QNetworkInterface::allAddresses();
        QList<QHostAddress>::iterator it;
        for(it = listeAdresse.begin(); it != listeAdresse.end(); it++){
            if(it->toIPv4Address()){
                qDebug()<<"Adresse serveur : " + it->toString();
            }
        }
        connect(socketEcouteServeur, &QTcpServer::newConnection, this, &ServeurCrawler::onQTcpServerNewConnection);
        ui->pushButtonLancer->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
    }else{
        qDebug() <<socketEcouteServeur->errorString();
    }
}

void ServeurCrawler::EnvoyerDonnees(QTcpSocket *client, QPoint pt, QString msg)
{
    quint16 taille=0;
    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);
    out<<taille<<pt<<msg;
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    tampon.seek(0);
    out<<taille;
    client->write(tampon.buffer());
}

QPoint ServeurCrawler::DonnerPositionUnique()
{
    QRandomGenerator gen;
    QPoint pt;
    gen.seed(QDateTime::currentMSecsSinceEpoch());
    int ligne;
    int colonne;
    do
    {
        ligne = gen.bounded(TAILLE);
        QThread::usleep(20000);	// attendre 20ms
        colonne = gen.bounded(TAILLE);
        QThread::usleep(20000);	// attendre 20ms
        pt = QPoint(colonne,ligne);
    }while (listePositions.contains(pt));
    return pt;
}

