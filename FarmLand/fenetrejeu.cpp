#include "fenetrejeu.h"
#include "ui_fenetrejeu.h"
#include <QMessageBox>
#include <QCursor>
#include <QPoint>
#include <iostream>
#include <QTimer>

FenetreJeu::FenetreJeu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FenetreJeu)
{
    ui->setupUi(this);
    socketFarmLand=new QTcpSocket(this);
    connect(socketFarmLand,&QTcpSocket::connected,this,&FenetreJeu::onQTcpSocket_connected);
    connect(socketFarmLand,&QTcpSocket::disconnected,this,&FenetreJeu::onQTcpSocket_disconnected);
    connect(socketFarmLand,&QTcpSocket::readyRead,this,&FenetreJeu::onQTcpSocket_readyRead);
    connect(socketFarmLand,&QTcpSocket::errorOccurred,this,&FenetreJeu::onQTcpSocket_errorOccured);

    ui->pushButtonNouvelleZone->setStyleSheet("background-position: center;"
                                              "background : no-repeat;"
                                              "background-image: url('/home/USERS/ELEVES/SNIR2022/tperichet/Images/images.jpg')");

  /*  ui->graphicsView->setStyleSheet("background-position: center;"
                                    "background : no-repeat;"
                                    "background-image: url('/home/USERS/ELEVES/SNIR2022/tperichet/Documents/C++/Challenge_Noel/FarmLand/plan.webp')");
*/
    maScene.setSceneRect(0,0,800,462);

    maVue = new QGraphicsViewPerso(this);
    maVue->setGeometry(10,20,800,462);
    maVue->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    maVue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    maVue->fitInView(maScene.sceneRect(),Qt::KeepAspectRatio);
    connect(maVue,&QGraphicsViewPerso::positionSouris,this,&FenetreJeu::onQGraphicsViewPerso_positionSouris);
    maVue->setScene(&maScene);
    maVue->setStyleSheet("background-position: center;"
                         "background : no-repeat;"
                         "background-image: url('/home/USERS/ELEVES/SNIR2022/tperichet/Documents/C++/Challenge_Noel/FarmLand/plan.webp')");
    setFocus();
}
FenetreJeu::~FenetreJeu()
{
    delete ui;
    delete socketFarmLand;;
}

void FenetreJeu::on_pushButtonNouvelleZone_clicked()
{

    zoneCommune = 0;

    if(ui->pushButtonNouvelleZone->text()== "")

    {
        if(zoneCommune == 1)
        {
            ui->labelNouvelleZone->setText("Zone 2 débloqué");
        }
        else{

            ui->labelNouvelleZone->setText("Vous n'avez pas les ressources suffisantes");
        }

    }
}

void FenetreJeu::onQTcpSocket_connected()
{
    ui->pushButtonNouvelleZone->setEnabled(true);
    maVue->setEnabled(true);

    ui->spinBoxPort->setDisabled(true);
    ui->lineEditPrenom->setDisabled(true);
    setFocus();

    ui->pushButtonConnexionServeur->setText("Déconnexion");

    quint16 taille=0;
    QBuffer tampon;
    QChar commande = 'u';
    QString username = ui->lineEditPrenom->text();
    tampon.open(QIODevice::WriteOnly);
    // association du tampon au flux de sortie
    QDataStream out(&tampon);
    // construction de la trame
    out<<taille<<commande<<username;
    // calcul de la taille de la trame
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    // placement sur la premiere position du flux pour pouvoir modifier la taille
    tampon.seek(0);
    //modification de la trame avec la taille reel de la trame
    out<<taille;
    // envoi du QByteArray du tampon via la socket
    socketFarmLand->write(tampon.buffer());
}

void FenetreJeu::onQTcpSocket_disconnected()
{
    ui->pushButtonNouvelleZone->setEnabled(false);
    maVue->setEnabled(false);
    ui->lineEditAdresse->setEnabled(false);
    ui->spinBoxPort->setEnabled(false);
}

void FenetreJeu::onQTcpSocket_readyRead()
{
    quint16 taille=0;
    QChar commande;
    // Il y a au moins le champs taille d'arrive
    if (socketFarmLand->bytesAvailable() >= (qint64)sizeof(taille))
    {
        //if()

    }
}

void FenetreJeu::EnvoyerCommande(QChar commande)
{
    quint16 taille=0;
    QBuffer tampon;
    tampon.open(QIODevice::WriteOnly);
    // association du tampon au flux de sortie
    QDataStream out(&tampon);
    // construction de la trame
    out<<taille<<commande;
    // calcul de la taille de la trame
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    // placement sur la premiere position du flux pour pouvoir modifier la taille
    tampon.seek(0);
    //modification de la trame avec la taille reel de la trame
    out<<taille;
    // envoi du QByteArray du tampon via la socket
    socketFarmLand->write(tampon.buffer());
}

void FenetreJeu::keyPressEvent(QKeyEvent *event)
{
    switch ( event->key() )
    {
    case Qt::Key_4:
        EnvoyerCommande('4');
        qDebug()<<"gauche";
        break;
    case Qt::Key_6:
        EnvoyerCommande('6');
        qDebug()<<"droit";
        break;
    case Qt::Key_8:
        EnvoyerCommande('8');
        qDebug()<<"haut";
        break;
    case Qt::Key_2:
        EnvoyerCommande('2');
        qDebug()<<"bas";
        break;
    case Qt::Key_7:
        EnvoyerCommande('7');
        qDebug()<<"DiagonaleHautGauche";
        break;
    case Qt::Key_9:
        EnvoyerCommande('9');
        qDebug()<<"DiagonaleHautDroite";

        break;
    case Qt::Key_1:
        EnvoyerCommande('1');
        qDebug()<<"DiagonaleBasGauche";
        break;
    case Qt::Key_3:
        EnvoyerCommande('3');
        qDebug()<<"DiagonaleBasDroite";
        break;
    }
}



void FenetreJeu::onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // le paramètre n'est pas utilisé dans cette méthode
    qDebug() << qt_error_string();
}


void FenetreJeu::on_pushButtonConnexionServeur_clicked()
{
    if(!ui->lineEditPrenom->text().isEmpty())
    {
        if(ui->pushButtonConnexionServeur->text() == "Connexion serveur"){
            socketFarmLand->connectToHost(ui->lineEditAdresse->text(),ui->spinBoxPort->text().toInt());
            ui->lineEditPrenom->text();
            qDebug() << ui->lineEditPrenom->text();

        }
        else
        {
            socketFarmLand->disconnectFromHost();
            ui->pushButtonConnexionServeur->setText("Connexion serveur");
        }
    }
    else{
        QMessageBox msgBox;
        msgBox.setText("Veuillez compléter votre prénom");
        msgBox.exec();
    }
}

void FenetreJeu::obtenirCoordonneesCurseur()
{
    // Obtenez la position actuelle du curseur de la souris
    QPoint cursorPos = QCursor::pos();

    // Affichez les coordonnées du curseur (vous pouvez ajuster cela en fonction de vos besoins)
    qDebug() << "Coordonnées du curseur de la souris : " << cursorPos.x() << ", " << cursorPos.y();

    // Faites ce que vous voulez avec les coordonnées ici...
}

void FenetreJeu::onQGraphicsViewPerso_positionSouris(QPoint pos)
{

}

