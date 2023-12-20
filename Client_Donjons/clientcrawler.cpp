#include "clientcrawler.h"
#include "ui_clientcrawler.h"

ClientCrawler::ClientCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientCrawler)
{
    ui->setupUi(this);
    socketClient = new QTcpSocket(this);
    connect(socketClient, &QTcpSocket::connected, this, &ClientCrawler::onQTcpSocketConnected);
    connect(socketClient, &QTcpSocket::disconnected,this, &ClientCrawler::onQTcpSocketDisconnect);
    connect(socketClient, &QTcpSocket::readyRead, this, &ClientCrawler::onQTcpSocketReadyRead);
    connect(socketClient, &QTcpSocket::errorOccurred, this, &ClientCrawler::onQTcpSocketErrorOccured);

    scene.setSceneRect(0,0,531,431);
    ui->vue->fitInView(scene.sceneRect(),Qt::KeepAspectRatio);
    ui->vue->setScene(&scene);
    joueur = new QGraphicsRectItem(0,0,20,20);
    QBrush interieurRec(Qt::red);
    joueur->setBrush(interieurRec);
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
    QPoint pos;
    int x = 0;
    int y = 0;
    quint16 taille=0;
    QString message;
    double distance;


    // Il y a au moins le champs taille d'arrive
    if (socketClient->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Lecture de la taille de la trame
        QDataStream in(socketClient);
        // Le reste de la trame est disponible
        if (socketClient->bytesAvailable() >= (qint64)taille)
        {
            in>>taille>>pos>>message>>distance;
            x=pos.x();
            y=pos.y();
            // Lecture de la commande
            if(message=="start"){
                scene.addItem(joueur);
            }
            if(message=="vide"){
                /*grille->itemAtPosition(y,x)->widget()->setStyleSheet("background-color : black");
                ui->labelInformations->setText(message);
                ui->labelInformations->setStyleSheet("color : black")*/;
            }
            if(message=="collision"){
                /*grille->itemAtPosition(y,x)->widget()->setStyleSheet("background-color : magenta");
                ui->labelInformations->setText(message);
                ui->labelInformations->setStyleSheet("color : magenta")*/;
            }
            if(QPoint(-1,1)==pos){
//                if(message.contains("victoire de " + socketClient->peerAddress().toString())){
//                    ui->labelInformations->setText(message);
//                    ui->labelInformations->setStyleSheet("color : green");
//                } else{
//                    ui->labelInformations->setText(message);
//                    ui->labelInformations->setStyleSheet("color : red");
//                }

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
           case Qt::Key_Left:
               qDebug()<<"gauche";
               EnvoyerCommande('L');
               break;
           case Qt::Key_Right:
               qDebug()<<"droit";
               EnvoyerCommande('R');
               break;
           case Qt::Key_Up:
               qDebug()<<"haut";
               EnvoyerCommande('U');
               break;
           case Qt::Key_Down:
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


