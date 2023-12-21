#include "serveur.h"
#include "ui_serveur.h"

Serveur::Serveur(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Serveur)
{
    ui->setupUi(this);

    connect(&socketServeur, &QTcpServer::newConnection, this, &Serveur::onQTcpServer_newConnection);


    maScene.setSceneRect(0,0,800,462);

    maVue = new QGraphicsViewPerso(this);
    maVue->setGeometry(10,20,800,462);
    maVue->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    maVue->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    maVue->fitInView(maScene.sceneRect(),Qt::KeepAspectRatio);
    maVue->setScene(&maScene);
    maVue->setStyleSheet("background-position: center;"
                         "background : no-repeat;"
                         "background-image: url('../FarmLandServeur/plan.webp')");

    QVBoxLayout *VerticalLayout = new QVBoxLayout(this);
    VerticalLayout->addWidget(maVue);
    VerticalLayout->addWidget(ui->pushButtonLancerServeur);
    VerticalLayout->addWidget(ui->spinBoxPort);
    VerticalLayout->addWidget(ui->textEditLogs);
    VerticalLayout->addWidget(ui->pushButtonQuitter);
    setFocus();

    QPolygon zone2;
    zone2.setPoints(4,
                    414, 45,
                    737, 45,
                    737, 313,
                    414, 313);

    QGraphicsPolygonItem *zone2Item = new QGraphicsPolygonItem(zone2);

    QPen contour(Qt::black);
    contour.setWidth(2);
    zone2Item->setPen(contour);

    maScene.addItem(zone2Item);


    zone2Verif = false;
    zone3Verif == false;
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
                newPos.setY(newPos.y()-5);
                break;

                // Bas
            case '2':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas");
                newPos.setY(newPos.y()+5);
                break;

                // Gauche
            case '4':
                ui->textEditLogs->append(joueur->getUsername() + ": Gauche");
                newPos.setX(newPos.x()-5);
                break;

                // Droite
            case '6':
                ui->textEditLogs->append(joueur->getUsername() + ": Droite");
                newPos.setX(newPos.x()+5);
                break;

                // Haut-Gauche
            case '7':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Gauche");
                newPos.setX(newPos.x()-5);
                newPos.setY(newPos.y()-5);
                break;

                // Bas-Gauche
            case '1':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Gauche");
                newPos.setX(newPos.x()-5);
                newPos.setY(newPos.y()+5);
                break;

                // Haut-Droit
            case '9':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Droit");
                newPos.setX(newPos.x()+5);
                newPos.setY(newPos.y()-5);
                break;

                // Bas-Droit
            case '3':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Droit");
                newPos.setX(newPos.x()+5);
                newPos.setY(newPos.y()+5);
                break;

                // Username
            case 'u':
                in>>username;
                joueur->setUsername(username);
                ui->textEditLogs->append(username);
                break;
            }
            if(checkZone(joueur) != -1)
            {
                joueur->setPos(newPos);
            }
            else
            {
                //joueur->setPos(QPoint(20, 120));
            }
            checkPositions();
            envoyerDonneesAll();
            envoyerInventaire();
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
    nouveauJoueur->setPos(QPoint(20, 120));

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

void Serveur::checkPositions()
{
    int valid;
    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        valid = 0;
        QPoint pos = joueurCourant->getPos();
        if (pos.x() >= 243 && pos.x() <= 243 + 100 && pos.y() <= 245 && pos.y() >= 245-80)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le blé";
            valid++;
        }

        if (pos.x() >= 90 && pos.x() <= 90 + 100 && pos.y() <= 240 && pos.y() >= 240-60)
        {
            qDebug() << joueurCourant->getUsername() << " est dans la roche";
            valid++;
        }

        if (pos.x() >= 225 && pos.x() <= 225 + 85 && pos.y() <= 100 && pos.y() >= 100-60)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le bois de chêne";
            valid++;
        }

        if (pos.x() >= 605 && pos.x() <= 605 + 80 && pos.y() <= 115 && pos.y() >= 115-70)
        {
            qDebug() << joueurCourant->getUsername() << " est dans la carotte";
            valid++;
        }

        if (pos.x() >= 615 && pos.x() <= 615 + 75 && pos.y() <= 240 && pos.y() >= 240-70)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le fer";
            valid++;
        }

        if (pos.x() >= 400 && pos.x() <= 400 + 85 && pos.y() <= 115 && pos.y() >= 115-70)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le bois de bouleau";
            valid++;
        }

        if (pos.x() >= 590 && pos.x() <= 590 + 80 && pos.y() <= 377 && pos.y() >= 377-70)
        {
            qDebug() << joueurCourant->getUsername() << " est dans la patate";
            valid++;
        }

        if (pos.x() >= 415 && pos.x() <= 415 + 100 && pos.y() <= 375 && pos.y() >= 375-65)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le diamant";
            valid++;
        }

        if (pos.x() >= 232 && pos.x() <= 232 + 85 && pos.y() <= 376 && pos.y() >= 376-70)
        {
            qDebug() << joueurCourant->getUsername() << " est dans le bois de sapin";
            valid++;
        }


        if(valid == 0)
        {
            // Faire truc
            // faire fonction pour chaque case qui va enable bouton farm chez client
            // si valid == 0 alors disable chez le joueur
        }
    }
}

int Serveur::checkZone(Joueur *joueur)
{
    QPoint pos = joueur->getPos();
    // Top
    if(pos.y() <= 46)
    {
        pos.setY(pos.y()+20);
        joueur->setPos(pos);
        return -1;
    }
    // Bottom
    if(pos.y()+70 >= 448)
    {
        pos.setY(pos.y()-20);
        joueur->setPos(pos);
        return -1;
    }
    // Left
    if(pos.x() <= 0)
    {
        pos.setX(pos.x()+20);
        joueur->setPos(pos);
        return -1;
    }
    // Right
    if(pos.x()+40 >= 738)
    {
        pos.setX(pos.x()-20);
        joueur->setPos(pos);
        return -1;
    }

    // Zone 2
    //if (pos.x() >= 414 && pos.x() <= 737 && pos.y() >= 45 && pos.y() <= 313)
    if (zone2Verif == false && pos.x() >= 414 || zone2Verif == false && pos.x()+40 >= 414)
    {
        pos.setX(pos.x()-20);
        joueur->setPos(pos);
        return -1;
    }

    if (zone3Verif == false && pos.y() >= 313 || zone3Verif == false && pos.y()+70 >= 313)
    {
        pos.setY(pos.y()-20);
        joueur->setPos(pos);
        return -1;
    }

    return 0;
}

void Serveur::envoyerInventaire()
{
    quint16 taille;
    QBuffer tampon;
    QChar commande = 'i';
    // generer la liste de position
    foreach(Joueur *joueurCourant, listeJoueurs)
    {
        taille = 0;
        // construction de la trame à envoyer au client
        tampon.open(QIODevice::WriteOnly);
        // association du tampon au flux de sortie
        QDataStream out(&tampon);

        // construction de la trame
        out<<taille<<commande<<coffreCommun;
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

