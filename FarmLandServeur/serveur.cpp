#include "serveur.h"
#include "ui_serveur.h"

Serveur::Serveur(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Serveur)
{
    ui->setupUi(this);

    connect(&socketServeur, &QTcpServer::newConnection, this, &Serveur::onQTcpServer_newConnection);
}

Serveur::~Serveur()
{
    foreach(QTcpSocket *clientCourant, listeSocketsClients)
    {
        clientCourant->close();
        delete clientCourant;
    }

    delete ui;
}

void Serveur::onQTcpServer_newConnection()
{
    // récupération de la socket de communication du client
    QTcpSocket *client=socketServeur.nextPendingConnection();

    // connection signal/slot pour la socket
    connect(client,&QTcpSocket::disconnected,this,&Serveur::onQTcpSocket_disconnected);
    connect(client,&QTcpSocket::readyRead,this,&Serveur::onQTcpSocket_readyRead);
    connect(client,&QTcpSocket::errorOccurred,this,&Serveur::onQTcpSocket_errorOccured);
    // création et ajout du client dans la liste des clients
    listeSocketsClients.append(client);

    ui->textEditLogs->append("Nouvelle connexion " + client->peerAddress().toString()
                             + ":" + QString::number(client->peerPort()));
}

void Serveur::onQTcpSocket_readyRead()
{
    quint16 taille = 0;
    QChar commande;

    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());

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

                break;

            }
        }
    }
}

void Serveur::onQTcpSocket_disconnected()
{
    // récupérer la socket du client se déconnectant
    QTcpSocket *client=qobject_cast<QTcpSocket *>(sender());
    // récupérer l'index de ce client dans la liste
    int index= listeSocketsClients.indexOf(client);
    // supprimer le client de la liste
    if (index!=-1)
    {
        listeSocketsClients.removeAt(index);
    }
    // afficher un message avec l'ip et le port du client deconnecté
    ui->textEditLogs->append("Le client " + client->peerAddress().toString() + ":" +
                             QString::number(client->peerPort()) + " s'est déconnecté");
}

void Serveur::onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *client = qobject_cast<QTcpSocket * >(sender());
    Q_UNUSED(socketError);
    ui->textEditLogs->append("Client : " + client->errorString());
}


void Serveur::on_pushButtonLancerServeur_clicked()
{
    if(!socketServeur.listen(QHostAddress::Any, ui->spinBoxPort->value()))
    {
        QString message = "Impossible de démarrer le serveur " + socketServeur.errorString();
        ui->textEditLogs->append(message);
    }
    else
    {
        QList<QHostAddress> listeAdresses = QNetworkInterface::allAddresses();
        QList<QHostAddress>::iterator it;
        for(it = listeAdresses.begin(); it != listeAdresses.end();it++)
        {
            if(it->toIPv4Address())
                ui->textEditLogs->append("Adresse serveur : " + it->toString());
        }
        ui->textEditLogs->append("N° du port : " + QString::number(socketServeur.serverPort()));

        // Désactive les boutons relatifs au lancement du serveur s'il est déjà lancé
        ui->pushButtonLancerServeur->setDisabled(true);
        ui->spinBoxPort->setDisabled(true);
    }
}

void Serveur::EnvoyerDonnees(QTcpSocket *client)
{
    quint16 taille=0;
    QBuffer tampon;
    // construction de la trame à envoyer au client
    tampon.open(QIODevice::WriteOnly);
    // association du tampon au flux de sortie
    QDataStream out(&tampon);
    // construction de la trame
    out<<taille;
    // calcul de la taille de la trame
    taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
    // placement sur la premiere position du flux pour pouvoir modifier la taille
    tampon.seek(0);
    //modification de la trame avec la taille reel de la trame
    out << taille;
    // envoi du QByteArray du tampon via la socket
    client->write(tampon.buffer());
}

void Serveur::CreerJoueur(QTcpSocket *client)
{
    Joueur *nouveauJoueur = new Joueur;
    nouveauJoueur->setSockClient(client);
    nouveauJoueur->setEpee(0);
    nouveauJoueur->setPioche(1);
    nouveauJoueur->setHache(1);
    nouveauJoueur->setHoue(1);

    listeJoueurs.append(nouveauJoueur);
}

int Serveur::getIndexClient(QTcpSocket *client)
{
    int cmp = 0;

    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        if (joueurCourant->getSockClient() == client)
        {
            return cmp;
        }
        cmp ++;
    }

    return -1;
}

