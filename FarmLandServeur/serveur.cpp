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
    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        joueurCourant->getSockClient()->close();
        delete joueurCourant;
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
    CreerJoueur(client);

    ui->textEditLogs->append("Nouvelle connexion " + client->peerAddress().toString()
                             + ":" + QString::number(client->peerPort()));
}

void Serveur::onQTcpSocket_readyRead()
{
    quint16 taille = 0;
    QChar commande;

    QTcpSocket *client = qobject_cast<QTcpSocket *>(sender());

    int index = getIndexClient(client);

    Joueur *joueur = listeJoueurs.at(index);

    QPoint newPos = joueur->getPos();

    QString username;

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
            // Haut
            case '8':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut");
                newPos.setY(newPos.y()-10);
                break;

                // Bas
            case '2':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas");
                newPos.setY(newPos.y()+10);
                break;

                // Gauche
            case '4':
                ui->textEditLogs->append(joueur->getUsername() + ": Gauche");
                newPos.setX(newPos.x()-10);
                break;

                // Droite
            case '6':
                ui->textEditLogs->append(joueur->getUsername() + ": Droite");
                newPos.setX(newPos.x()+10);
                break;

                // Haut-Gauche
            case '7':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Gauche");
                newPos.setX(newPos.x()-10);
                newPos.setY(newPos.y()-10);
                break;

                // Bas-Gauche
            case '1':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Gauche");
                newPos.setX(newPos.x()-10);
                newPos.setY(newPos.y()+10);
                break;

                // Haut-Droit
            case '9':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Droit");
                newPos.setX(newPos.x()+10);
                newPos.setY(newPos.y()-10);
                break;

                // Bas-Droit
            case '3':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Droit");
                newPos.setX(newPos.x()+10);
                newPos.setY(newPos.y()+10);
                break;

                // Username
            case 'u':
                in>>username;
                joueur->setUsername(username);
                ui->textEditLogs->append(username);
                break;
            }
            joueur->setPos(newPos);
            envoyerDonneesAll();
        }
    }
}

void Serveur::onQTcpSocket_disconnected()
{
    // récupérer la socket du client se déconnectant
    QTcpSocket *client=qobject_cast<QTcpSocket *>(sender());
    // récupérer l'index de ce client dans la liste
    int index= getIndexClient(client);
    // supprimer le client de la liste
    if (index!=-1)
    {
        listeJoueurs.removeAt(index);
    }
    // afficher un message avec l'ip et le port du client deconnecté
    ui->textEditLogs->append("Le client " + client->peerAddress().toString() + ":" +
                             QString::number(client->peerPort()) + " s'est déconnecté");
}

void Serveur::onQTcpSocket_errorOccured(QAbstractSocket::SocketError socketError)
{
    QTcpSocket *client = qobject_cast<QTcpSocket * >(sender());
    Q_UNUSED(socketError);
    ui->textEditLogs->append(client->peerAddress().toString() + ":" +
                             QString::number(client->peerPort()) + client->errorString());
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
    nouveauJoueur->setPos(QPoint(0, 0));

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

void Serveur::envoyerDonneesAll()
{
    quint16 taille;
    QBuffer tampon;
    QChar commande = 'a';
    QList <QPoint> listePosition;
    int index;
    // generer la liste de position
    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        listePosition.append(joueurCourant->getPos());
    }

    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        taille = 0;
        // construction de la trame à envoyer au client
        tampon.open(QIODevice::WriteOnly);
        // association du tampon au flux de sortie
        QDataStream out(&tampon);

        index = listePosition.indexOf(joueurCourant->getPos());

        // construction de la trame
        out<<taille<<commande<<index<<listePosition;
        // calcul de la taille de la trame
        taille=(static_cast<quint16>(tampon.size()))-sizeof(taille);
        // placement sur la premiere position du flux pour pouvoir modifier la taille
        tampon.seek(0);
        //modification de la trame avec la taille reel de la trame
        out << taille;
        // envoi du QByteArray du tampon via la socket
        joueurCourant->getSockClient()->write(tampon.buffer());

        tampon.close();
    }
}

