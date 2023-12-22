#include "serveurcrawler.h"
#include "ui_serveurcrawler.h"

ServeurCrawler::ServeurCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ServeurCrawler)
{
    ui->setupUi(this);
    socketEcouteServeur = new QTcpServer(this);
    connect(socketEcouteServeur, &QTcpServer::newConnection, this, &ServeurCrawler::onQTcpServerNewConnection);
    scene.setSceneRect(0,0,800,462);

    vue = new QGraphicsView(this);

    vue->setBackgroundBrush(QBrush(QPixmap(":/img/map.png")));
    vue->setFixedSize(800,462);
    vue->setGeometry(10,20,800,462);
    vue->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vue->fitInView(scene.sceneRect());
    vue->setScene(&scene);
    vue->show();
    boule = new QGraphicsBouleHorizontalItem(0,0,21,21);
    scene.addItem(boule);
    boule->setPos(300,100);


    scene.addItem(recPerso);

    recPerso = new QGraphicsPixmapItem(QPixmap(":/img/candy.png"));
    recPerso->setPos(0,0);
    recPerso->setScale(0.25);
    connect(&timer,&QTimer::timeout,&scene,&QGraphicsScene::advance);
    connect(boule,&QGraphicsBouleHorizontalItem::positionBoule,this,&ServeurCrawler::envoyerPositionBouleAuClients);

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
//boule->setPos(300,100);
    timer.start(5);
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
                pos.setY((pos.y()+41));
                break;
            case 'U':
                pos.setY(abs(pos.y()-41));
                break;
            case 'R':
                pos.setX((pos.x()+41));
                break;
            case 'L':
                pos.setX(abs(pos.x()-41));
                break;
            }

            // autre
            // test des limites
            if (pos.x()<11) pos.setX(11);
            if (pos.x()>739) pos.setX(739);
            if (pos.y()<11) pos.setY(11);
            if (pos.y()>391) pos.setY(391);
            //mise a jour position
            listePositions.replace(indexClient,pos);
            // envoyer position au client
            EnvoyerDonnees(socketVersClient,boule->pos().toPoint(),'J');
        }
    }
}

void ServeurCrawler::onQTcpSocketDisconnect() // quand un client ce deconnecte
{
    QTcpSocket *client=qobject_cast<QTcpSocket*>(sender());
    int indexClient = listeSocketClient.indexOf(client);
    listeSocketClient.removeAt(indexClient);
    listePositions.removeAt(indexClient);
    //message de deconnection d'un client
    qDebug() << "Client deconnecté";
}

void ServeurCrawler::onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // le paramètre n'est pas utilisé dans cette méthode
    qDebug()<< socketEcouteServeur->errorString(); // message erreur de socket coter serveur
}

void ServeurCrawler::on_pushButtonLancer_clicked() // quand le bouton lancer et activer
{
    if(socketEcouteServeur->listen(QHostAddress::Any,ui->spinBoxPort->value())){
        QList<QHostAddress> listeAdresse = QNetworkInterface::allAddresses();
        QList<QHostAddress>::iterator it;

        for(it = listeAdresse.begin(); it != listeAdresse.end(); it++){
            if(it->toIPv4Address()){
                // message derreur si ladresse est mauvaise
                qDebug()<<"Adresse serveur : " + it->toString();
            }
        }

        //active/desactive le boutouon lancer + spinboxport
        ui->pushButtonLancer->setEnabled(false);
        ui->spinBoxPort->setEnabled(false);
    }else{
        //message erreur ecoute
        qDebug() <<socketEcouteServeur->errorString();
    }
}

void ServeurCrawler::EnvoyerDonnees(QTcpSocket *client, QPoint pt, QChar msg)
{
    //envoie des donnee au client

    quint16 taille=0;
    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);
    int indexClient=listeSocketClient.indexOf(client);
    QPoint positionJoueur=listePositions.at(indexClient);
    out<<taille<<msg<<positionJoueur<<pt;
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    tampon.seek(0);
    out<<taille;
    client->write(tampon.buffer());
}


QPoint ServeurCrawler::DonnerPositionUnique()
{
    //Donner envoyai la position d'un joueur unique
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

void ServeurCrawler::onQMediaPlayer_playbackStateChanged(QMediaPlayer::PlaybackState newState)
{
    QString etat;

    switch (newState)
    {
    case QMediaPlayer::StoppedState :
        etat="Stopped";
        break;
    case QMediaPlayer::PlayingState :
        etat="Playing";
        break;
    case QMediaPlayer::PausedState :
        etat="Paused";
        break;
    }
    qDebug()<<etat;
}

void ServeurCrawler::envoyerPositionBouleAuClients(QPointF posBoule)
{
    qDebug()<<"poition boule : "<<posBoule;
  /*  foreach(QTcpSocket *client, listeSocketClient)
    {
        EnvoyerDonnees(client,posBoule.toPoint(),'B');

    }
*/

}


