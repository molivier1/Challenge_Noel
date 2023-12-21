#include "clientcrawler.h"
#include "ui_clientcrawler.h"
#include <QDebug>


ClientCrawler::ClientCrawler(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ClientCrawler),
    socketDeDialogueAvecServeur(new QTcpSocket(this)),
    grille(new QGridLayout(this))

{
    ui->setupUi(this);
    player = new QMediaPlayer();
    audiOutput = new QAudioOutput();
    player->setAudioOutput(audiOutput);

    scene=new QGraphicsScene();
    vue = new QGraphicsView(this);
     scene->setSceneRect(0,0,1948,1178);
    vue->setBackgroundBrush(QBrush(QPixmap("/home/USERS/ELEVES/SNIR2022/tsoulaimana/Qt/Snir 3 C++/Projet Noel/Projet_Noel2022/serveur/image/map.png")));
    vue->setGeometry(0,0,1200,720);
    vue->fitInView(scene->sceneRect());
    vue->setScene(scene);
    vue->show();
    grille->addWidget(vue);

    // Placement sur la grille des objets présents sur ui
    // les labels
    grille->addWidget(ui->labelAdresseServeur,TAILLE,0,1,5);
    grille->addWidget(ui->labelNumeroPort,TAILLE,6,1,5);;
    // les QLineEdit adresse et port
    grille->addWidget(ui->lineEditAdresseServeur,TAILLE+1,0,1,5);
    grille->addWidget(ui->spinBoxPortServeur,TAILLE+1,6,1,5);
    // les QPushButton connexion et quitter
    grille->addWidget(ui->pushButtonConnexion,TAILLE+2,0,1,5);
    grille->addWidget(ui->pushButtonQuitter,TAILLE+2,6,1,5);
    this->setLayout(grille);
    setFocus();


    // association des evenements liés à la socket avec les slots appelés
    connect(socketDeDialogueAvecServeur,&QTcpSocket::connected,this,&ClientCrawler::onQTcpSocket_connected);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::disconnected,this,&ClientCrawler::onQTcpSocket_disconnected);
    //connect(socketDeDialogueAvecServeur,QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),this,&ClientCrawler::onQTcpSocket_error);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::hostFound,this,&ClientCrawler::onQTcpSocket_hostFound);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::stateChanged,this,&ClientCrawler::onQTcpSocket_stateChanged);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::aboutToClose,this,&ClientCrawler::onQTcpSocket_aboutToClose);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::bytesWritten,this,&ClientCrawler::onQTcpSocket_bytesWritten);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::readChannelFinished,this,&ClientCrawler::onQTcpSocket_readChannelFinished);
    connect(socketDeDialogueAvecServeur,&QTcpSocket::readyRead,this,&ClientCrawler::onQTcpSocket_readyRead);
}

ClientCrawler::~ClientCrawler()
{
    delete ui;
}

void ClientCrawler::on_pushButtonConnexion_clicked()
{
    if (ui->pushButtonConnexion->text()=="Connexion")
    {
        socketDeDialogueAvecServeur->connectToHost(ui->lineEditAdresseServeur->text(),ui->spinBoxPortServeur->value());
    }
    else
    {
        socketDeDialogueAvecServeur->disconnectFromHost();
    }

}


void ClientCrawler::onQTcpSocket_connected()
{
    qDebug()<< this << "connected";
    ui->pushButtonConnexion->setText("Deconnexion");
    ui->spinBoxPortServeur->setEnabled(false);

}

void ClientCrawler::onQTcpSocket_disconnected()
{
    qDebug() << this << "disconnected";
    ui->pushButtonConnexion->setText("Connexion");
    ui->spinBoxPortServeur->setEnabled(true);
}

void ClientCrawler::onQTcpSocket_error(QAbstractSocket::SocketError socketError)
{
    qDebug() << this << socketDeDialogueAvecServeur->errorString();
}

void ClientCrawler::onQTcpSocket_hostFound()
{
    qDebug() << this << "host found";
}

void ClientCrawler::onQTcpSocket_stateChanged(QAbstractSocket::SocketState socketState)
{
    QString etat;
    switch (socketState) {
    case QAbstractSocket::UnconnectedState:
        etat = "The socket is not connected.";

        break;
    case QAbstractSocket::HostLookupState:
        etat = "The socket is performing a host name lookup";
        break;
    case QAbstractSocket::ConnectingState:
        etat = "The socket has started establishing a connection";
        break;
    case QAbstractSocket::ConnectedState:
        etat = "A connection is established";
        break;
    case QAbstractSocket::BoundState:
        etat = "The socket is bound to an address and port";
        break;
    case QAbstractSocket::ListeningState:
        etat = "For internal use only";
        break;
    case QAbstractSocket::ClosingState:
        etat = "The socket is about to close";
        break;

    }
    qDebug() << this << etat;
}

void ClientCrawler::onQTcpSocket_aboutToClose()
{
    qDebug() << this << "about to close";
}

void ClientCrawler::onQTcpSocket_bytesWritten(qint64 bytes)
{
    qDebug() << this << "bytes written : " << bytes;
}

void ClientCrawler::onQTcpSocket_readChannelFinished()
{
    qDebug() << this << "read channel finnished";
}

void ClientCrawler::onQTcpSocket_readyRead()
{
    quint16 taille; // La taille minimale pour prendre en compte les datas
    QString message;
    double distance;

    qDebug() << this << "Ready Read : " << QString::number(socketDeDialogueAvecServeur->bytesAvailable()) << " bytes Availables";
    if (socketDeDialogueAvecServeur->bytesAvailable() >= (qint64)sizeof(taille))
    {
        QDataStream in(socketDeDialogueAvecServeur);
        in >> taille;

        if (socketDeDialogueAvecServeur->bytesAvailable() >= (qint64)taille)
        {
            // décomposition de la trame
            in >> position >> message >> distance;
            qDebug() << this << "position : " << position;
            qDebug() << this << "message : " << message;
            qDebug() << this << "distance : " <<distance;
            ViderGrille();
            if (position != QPoint(-1,-1))
            {
                if (message.contains("collision"))
                {

                    player->setSource(QUrl::fromLocalFile("/home/USERS/ELEVES/SNIR2021/tsoulaimana/Qt/projet noel/prog prof/client/music/OOF.mp3"));
                    audiOutput->setVolume(50);
                    player->play();
                }
                else
                {
                }

                //grille->itemAtPosition(position.y(),position.x())->widget()->setStyleSheet("background-color : black");
                QGraphicsPixmapItem *rectTresor = new QGraphicsPixmapItem(QPixmap("/home/USERS/ELEVES/SNIR2022/tsoulaimana/Qt/Snir 3 C++/Projet Noel/Projet_Noel2022/Perso.png"));
                    rectTresor->setPos(0,0);
                    rectTresor->setScale(0.35);
                    scene->addItem(rectTresor);
            }
            else
            {
                if (message.contains(socketDeDialogueAvecServeur->localAddress().toString()))
                {
                    player->setSource(QUrl::fromLocalFile("/home/USERS/ELEVES/SNIR2021/tsoulaimana/Qt/projet noel/prog prof/client/music/victory.mp3"));
                    audiOutput->setVolume(50);
                    player->play();
                    distance = 0;
                }
                else
                {
                    player->setSource(QUrl::fromLocalFile("/home/USERS/ELEVES/SNIR2021/tsoulaimana/Qt/projet noel/prog prof/client/music/audiomorty.mp3"));
                    audiOutput->setVolume(50);
                    player->play();
                }
            }

        }
    }
}

void ClientCrawler::ViderGrille()
{
    scene->clear();
}


void ClientCrawler::EnvoyerCommande(QChar commande)
{
    quint16 taille=0;

    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    // association du tampon au flux de sortie
    QDataStream out(&tampon);
    // construction de la trame
    out<<taille<<commande;
    // calcul de la taille de la trame
    taille=tampon.size() - sizeof(taille);

    // placement sur la premiere position du flux pour pouvoir modifier la taille
    tampon.seek(0);
    //modification de la trame avec la taille reel de la trame
    out<<taille;
    // envoi du QByteArray du tampon via la socket
    socketDeDialogueAvecServeur->write(tampon.buffer());
}


void ClientCrawler::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()){
    case Qt::Key_A:
        EnvoyerCommande('L');
        break;
    case Qt::Key_D:
        EnvoyerCommande('R');
        break;
    case Qt::Key_W:
        EnvoyerCommande('U');
        break;
    case Qt::Key_S:
        EnvoyerCommande('D');
        break;
    }
}

