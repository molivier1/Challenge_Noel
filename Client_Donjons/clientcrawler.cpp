#include "clientcrawler.h"
#include "ui_clientcrawler.h"

ClientCrawler::ClientCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientCrawler),
      grille(new QGridLayout(this))
{
    ui->setupUi(this);

    // partie des connect
    socketClient = new QTcpSocket(this);
    connect(socketClient, &QTcpSocket::connected, this, &ClientCrawler::onQTcpSocketConnected);
    connect(socketClient, &QTcpSocket::disconnected,this, &ClientCrawler::onQTcpSocketDisconnect);
    connect(socketClient, &QTcpSocket::readyRead, this, &ClientCrawler::onQTcpSocketReadyRead);
    connect(socketClient, &QTcpSocket::errorOccurred, this, &ClientCrawler::onQTcpSocketErrorOccured);

    //creation et ignitialisation de la vue et la scene
    scene=new QGraphicsScene();
    vue = new QGraphicsView(this);
    scene->setSceneRect(0,0,800,462);
    vue->setBackgroundBrush(QBrush(QPixmap(":/img/map.png")));
    vue->setFixedSize(800,462);
    vue->setGeometry(10,20,800,462);
    vue->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    vue->fitInView(scene->sceneRect());
    vue->setScene(scene);
    vue->show();
    grille->addWidget(vue);
    grille->addWidget(ui->labelAdresseServeur,TAILLE,0,1,5);
    grille->addWidget(ui->labelNumeroPort,TAILLE,6,1,5);
    grille->addWidget(ui->lineEditAdresseServeur,TAILLE+1,0,1,5);
    grille->addWidget(ui->spinBoxPortServeur,TAILLE+1,6,1,5);
    grille->addWidget(ui->pushButtonConnexion,TAILLE+2,0,1,5);
    grille->addWidget(ui->pushButtonQuitter,TAILLE+2,6,1,5);
    setFocus();
    recPerso = new QGraphicsPixmapItem(QPixmap(":/img/candy.png"));
    recPerso->setPos(0,0);
    recPerso->setScale(0.25);
    scene->addItem(recPerso);

    boule = new QGraphicsBouleHorizontalItem(0,0,40,40);
    boule->setPos(400,100);
    boule->setBrush(Qt::red);
    scene->addItem(boule);

    boule2 = new QGraphicsBouleHorizontalItem(0,0,40,40);
    boule2->setPos(200,300);
    boule2->setBrush(Qt::blue);
    scene->addItem(boule2);

    boule2->hide();
    boule->hide();
    connect(&timer,&QTimer::timeout,scene,&QGraphicsScene::advance);





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
    QChar message;
    double distance;
    QPoint positionBoule;
    QPoint positionJoueur;
qDebug()<<" in ready";

    // Il y a au moins le champs taille d'arrive
    if (socketClient->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Lecture de la taille de la trame
        QDataStream in(socketClient);
        in >> taille;
        // Le reste de la trame est disponible
        if (socketClient->bytesAvailable() >= (qint64)taille)
        {
            in>>message;
            qDebug()<<" commande "<<message;
            if (message=='J')
            {
                in>>positionJoueur>>positionBoule;;
                recPerso->setPos(positionJoueur);
            }
            if (message=='B')
            {
               /* in>>positionJoueur>>positionBoule;
                 recPerso->setPos(positionJoueur);
                boule->setPos(positionBoule);
                boule->show();*/
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
    boule->show();
    boule2->show();
    timer.start(5);
}

void ClientCrawler::onQTcpSocketDisconnect()
{

    qDebug() << "Etat de la socket : The socket is not connected";
    ui->lineEditAdresseServeur->setDisabled(false);
    ui->spinBoxPortServeur->setDisabled(false);
    ui->pushButtonConnexion->setText("Connexion");
    boule2->hide();
    boule->hide();
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
    case Qt::Key_Q:
        qDebug()<<"gauche";
        EnvoyerCommande('L');
        break;
    case Qt::Key_D:
        qDebug()<<"droit";
        EnvoyerCommande('R');
        break;
    case Qt::Key_Z:
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

