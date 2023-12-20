#include "serveurcrawler.h"
#include "ui_serveurcrawler.h"

ServeurCrawler::ServeurCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServeurCrawler)
{
    ui->setupUi(this);
    socketEcouteServeur = new QTcpServer(this);
    connect(socketEcouteServeur, &QTcpServer::newConnection, this, &ServeurCrawler::onQTcpServerNewConnection);
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
    AfficherGrille();
}

void ServeurCrawler::onQTcpSocketReadyRead()
{
    quint16 taille=0;
    QChar commande;
    QTcpSocket *client = qobject_cast<QTcpSocket*>(sender());
    int indexClient = listeSocketClient.indexOf(client);
//    QPoint posclient = listePositions.at(indexClient);
    // Il y a au moins le champs taille d'arrive
    if (client->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Lecture de la taille de la trame
        QDataStream in(client);
        in >> taille;
        // Le reste de la trame est disponible
        if (client->bytesAvailable() >= (qint64)taille)
        {
            // Lecture de la commande
            in>>commande;
            switch (commande.toLatin1()) {
            case 'U':
                //posclient.setY(posclient.y()-1);
                joueur->moveBy(0,-10);
                break;
            case 'D':
                //posclient.setY(posclient.y()+1);
                joueur->moveBy(0,10);
                break;
            case 'L':
                //posclient.setX(posclient.x()-1);
                joueur->moveBy(-10,0);
                break;
            case 'R':
                //posclient.setX(posclient.x()+1);
                joueur->moveBy(10,0);
                break;
            }
            //collision
//            if(listePositions.contains(joueur)){
//                int indexAutre=listePositions.indexOf(joueur);
//                QPoint posAutre = DonnerPositionUnique();
//                QPoint posClient = DonnerPositionUnique();
//                listePositions.replace(indexAutre,posAutre);
//                listePositions.replace(indexClient,posClient);
//                EnvoyerDonnees(client,posClient,"collision");
//                EnvoyerDonnees(listeSocketClient.at(indexAutre),posAutre,"collision");
//            }
        }
        AfficherGrille();
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

void ServeurCrawler::AfficherGrille()
{

    foreach(QPoint pt,listePositions){
        joueur = new QGraphicsRectItem(0,0,20,20);
        QBrush interieurRec(Qt::red);
        joueur->setBrush(interieurRec);
        joueur->setPos(pt.x(),pt.y());
    }
}
