#include "clientcrawler.h"
#include "ui_clientcrawler.h"

ClientCrawler::ClientCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientCrawler),
      grille(new QGridLayout(this))
{
    ui->setupUi(this);
    socketClient = new QTcpSocket(this);
    connect(socketClient, &QTcpSocket::connected, this, &ClientCrawler::onQTcpSocketConnected);
    connect(socketClient, &QTcpSocket::disconnected,this, &ClientCrawler::onQTcpSocketDisconnect);
    connect(socketClient, &QTcpSocket::readyRead, this, &ClientCrawler::onQTcpSocketReadyRead);
    connect(socketClient, &QTcpSocket::errorOccurred, this, &ClientCrawler::onQTcpSocketErrorOccured);

    scene=new QGraphicsScene();
    vue = new QGraphicsView(this);
    scene->setSceneRect(0,0,800,462);
    setWindowState(Qt::WindowMaximized);
    vue->setGeometry(0,0,800,462);
    vue->fitInView(scene->sceneRect());
    vue->setScene(scene);
    vue->setWindowState(Qt::WindowMaximized);
    vue->show();
    grille->addWidget(vue);
    grille->addWidget(ui->labelAdresseServeur,TAILLE,0,1,5);
    grille->addWidget(ui->labelNumeroPort,TAILLE,6,1,5);
    grille->addWidget(ui->lineEditAdresseServeur,TAILLE+1,0,1,5);
    grille->addWidget(ui->spinBoxPortServeur,TAILLE+1,6,1,5);
    grille->addWidget(ui->pushButtonConnexion,TAILLE+2,0,1,5);
    grille->addWidget(ui->pushButtonQuitter,TAILLE+2,6,1,5);
    vue->setFocus();


}
ClientCrawler::~ClientCrawler()
{
    if(socketClient!= nullptr){
        socketClient->close();
        delete socketClient;
    }
    delete ui;
}


void ClientCrawler::onQTcpSocketReadyRead()
{

    quint16 taille=0;
    QString message;
    double distance;


    // Il y a au moins le champs taille d'arrive
    if (socketClient->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Lecture de la taille de la trame
        QDataStream in(socketClient);
        in >> taille;
        // Le reste de la trame est disponible
        if (socketClient->bytesAvailable() >= (qint64)taille)
        {
            in>>position>>message>>distance;
            ViderGrille();
            if(position != QPoint(-1,-1)){
                QGraphicsPixmapItem *recPerso = new QGraphicsPixmapItem(QPixmap("/home/USERS/ELEVES/SNIR2022/tsoulaimana/Qt/Snir 3 C++/Projet Noel/Projet_Noel2022/cam.jpg"));
                recPerso->setPos(position);
                recPerso->setScale(0.25);
                //for (int x = 10; x<500; x+=20){
                boule=new QGraphicsBouleHorizontalItem(0,0,21,21);
                //boule->setPos(x+20,50-x());
                boule->setPos(20,50);
                //position de la boule
                scene->addItem(boule);
                //}
                scene->addItem(recPerso);
            }
        }
    }
}

void ClientCrawler::onQTcpSocketConnected()
{
    qDebug() <<"Etat de la socket : A connection is etablished";
    ui->pushButtonConnexion->setText("Deconnexion");
    ui->lineEditAdresseServeur->setDisabled(true);
    ui->spinBoxPortServeur->setDisabled(true);
}

void ClientCrawler::onQTcpSocketDisconnect()
{
    qDebug() << "Etat de la socket : The socket is not connected";
    ui->lineEditAdresseServeur->setDisabled(false);
    ui->spinBoxPortServeur->setDisabled(false);
    ui->pushButtonConnexion->setText("Connexion");
}

void ClientCrawler::onQTcpSocketErrorOccured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // le paramètre n'est pas utilisé dans cette méthode
    qDebug() << "Etat de la socket : The socket is not connected";
}

void ClientCrawler::on_pushButtonConnexion_clicked()
{
    if(ui->pushButtonConnexion->text()=="Connexion"){
        socketClient->connectToHost(ui->lineEditAdresseServeur->text(),ui->spinBoxPortServeur->text().toInt());
    }else{
        socketClient->disconnectFromHost();
    }
}

void ClientCrawler::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
    case Qt::Key_A:
        qDebug()<<"gauche";
        EnvoyerCommande('L');
        break;
    case Qt::Key_D:
        qDebug()<<"droit";
        EnvoyerCommande('R');
        break;
    case Qt::Key_W:
        qDebug()<<"haut";
        EnvoyerCommande('U');
        break;
    case Qt::Key_S:
        qDebug()<<"bas";
        EnvoyerCommande('D');
        break;
    }

}

void ClientCrawler::EnvoyerCommande(QChar commande)
{
    quint16 taille=0;
    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    QDataStream out(&tampon);
    out<<taille<<commande;
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    tampon.seek(0);
    out<<taille;
    socketClient->write(tampon.buffer());
}

void ClientCrawler::ViderGrille()
{
    scene->clear();
}

