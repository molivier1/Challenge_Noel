#include "fenetrejeu.h"
#include "ui_fenetrejeu.h"

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

    QString imagePath = "/home/USERS/ELEVES/SNIR2022/tperichet/Images/plus.jpg";
    QString styleSheet = "QPushButton#pushButtonNouvelleZone {"
                         "  background-image: url('" + imagePath + "');"
                         "  background-repeat: no-repeat;"
                         "  background-position: center;"
                        "  background-size: 31px 31px;"
                         "}";
    ui->pushButtonNouvelleZone->setStyleSheet(styleSheet);
}

FenetreJeu::~FenetreJeu()
{
    delete ui;
    delete socketFarmLand;
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
    ui->graphicsView->setEnabled(true);
    ui->graphicsView_2->setEnabled(true);

}

void FenetreJeu::onQTcpSocket_disconnected()
{
    ui->pushButtonNouvelleZone->setEnabled(false);
    ui->graphicsView->setEnabled(false);
    ui->graphicsView_2->setEnabled(false);
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

void FenetreJeu::EnvoyerCommande(QString commande)
{
    quint16 taille=0;
    QBuffer tampon;
    qDebug() << "touches";
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

void FenetreJeu::onMouvementBas()
{
    EnvoyerCommande("2");
}

void FenetreJeu::onMouvementHaut()
{
    EnvoyerCommande("8");
}

void FenetreJeu::onMouvementDroite()
{
    EnvoyerCommande("6");
}

void FenetreJeu::onMouvementGauche()
{
    EnvoyerCommande("4");
}

void FenetreJeu::onMouvementDiagonaleHautDroite()
{
    EnvoyerCommande("9");
}

void FenetreJeu::onMouvementDiagonaleBasDroite()
{
    EnvoyerCommande("3");
}

void FenetreJeu::onMouvementDiagonaleHautGauche()
{
    EnvoyerCommande("7");
}

void FenetreJeu::onMouvementDiagonaleBasGauche()
{
    EnvoyerCommande("1");
}

void FenetreJeu::onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError); // le paramètre n'est pas utilisé dans cette méthode
    qDebug() << qt_error_string();
}


void FenetreJeu::on_pushButtonConnexionServeur_clicked()
{
    if(ui->pushButtonConnexionServeur->text() == "Connexion serveur"){
        socketFarmLand->connectToHost(ui->lineEditAdresse->text(),ui->spinBoxPort->text().toInt());
        ui->pushButtonConnexionServeur->setText("Déconnexion");
    }
    else
    {
        socketFarmLand->disconnectFromHost();
        ui->pushButtonConnexionServeur->setText("Connexion serveur");
    }

}

