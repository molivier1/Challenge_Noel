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
    zone3Verif = false;

    timer = new QTimer(this);
    timer->start(50);
    connect(timer, &QTimer::timeout, this, &Serveur::onQTimer_timeout);
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

    QString message;

    int pasX = 20;
    int pasY = 20;

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
            in>>commande>>message;
            switch (commande.toLatin1()) {
            // Haut
            case '8':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut");
                newPos.setY(newPos.y()-pasY);
                break;

                // Bas
            case '2':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas");
                newPos.setY(newPos.y()+pasY);
                break;

                // Gauche
            case '4':
                ui->textEditLogs->append(joueur->getUsername() + ": Gauche");
                newPos.setX(newPos.x()-pasX);
                break;

                // Droite
            case '6':
                ui->textEditLogs->append(joueur->getUsername() + ": Droite");
                newPos.setX(newPos.x()+pasX);
                break;

                // Haut-Gauche
            case '7':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Gauche");
                newPos.setX(newPos.x()-pasX);
                newPos.setY(newPos.y()-pasY);
                break;

                // Bas-Gauche
            case '1':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Gauche");
                newPos.setX(newPos.x()-pasX);
                newPos.setY(newPos.y()+pasY);
                break;

                // Haut-Droit
            case '9':
                ui->textEditLogs->append(joueur->getUsername() + ": Haut-Droit");
                newPos.setX(newPos.x()+pasX);
                newPos.setY(newPos.y()-pasY);
                break;

                // Bas-Droit
            case '3':
                ui->textEditLogs->append(joueur->getUsername() + ": Bas-Droit");
                newPos.setX(newPos.x()+pasX);
                newPos.setY(newPos.y()+pasY);
                break;

                // Username
            case 'u':
                joueur->setUsername(message);
                ui->textEditLogs->append(message);
                break;

            case 'f':
                if(message == "ble")
                {
                    if (joueur->houe==1) {
                        coffreCommun.setBle(coffreCommun.getBle()+1);
                    }
                    if (joueur->houe==2) {
                        coffreCommun.setBle(coffreCommun.getBle()+2);
                    }
                    if (joueur->houe==3) {
                        coffreCommun.setBle(coffreCommun.getBle()+3);
                    }
                    if (joueur->houe==4) {
                        coffreCommun.setBle(coffreCommun.getBle()+4);
                    }
                }

                if(message == "roche")
                {
                    if (joueur->pioche==1) {
                        coffreCommun.setRoche(coffreCommun.getRoche()+1);
                    }
                    if (joueur->pioche==2) {
                        coffreCommun.setRoche(coffreCommun.getRoche()+2);
                    }
                    if (joueur->pioche==3) {
                        coffreCommun.setRoche(coffreCommun.getRoche()+3);
                    }
                    if (joueur->pioche==4) {
                        coffreCommun.setRoche(coffreCommun.getRoche()+4);
                    }                }

                if(message == "chene")
                {
                    if (joueur->hache==1) {
                        coffreCommun.setChene(coffreCommun.getChene()+1);
                    }
                    if (joueur->hache==2) {
                        coffreCommun.setChene(coffreCommun.getChene()+2);
                    }
                    if (joueur->hache==3) {
                        coffreCommun.setChene(coffreCommun.getChene()+3);
                    }
                    if (joueur->hache==4) {
                        coffreCommun.setChene(coffreCommun.getChene()+4);
                    }
                }

                if(message == "carotte")
                {
                    if (joueur->houe==1) {
                        coffreCommun.setCarotte(coffreCommun.getCarotte()+0);
                    }
                    if (joueur->houe==2) {
                        coffreCommun.setCarotte(coffreCommun.getCarotte()+0);
                    }
                    if (joueur->houe==3) {
                        coffreCommun.setCarotte(coffreCommun.getCarotte()+1);
                    }
                    if (joueur->houe==4) {
                        coffreCommun.setCarotte(coffreCommun.getCarotte()+2);
                    }
                }

                if(message == "fer")
                {
                    if (joueur->pioche==1) {
                        coffreCommun.setFer(coffreCommun.getFer()+0);
                    }
                    if (joueur->pioche==2) {
                        coffreCommun.setFer(coffreCommun.getFer()+0);
                    }
                    if (joueur->pioche==3) {
                        coffreCommun.setFer(coffreCommun.getFer()+1);
                    }
                    if (joueur->pioche==4) {
                        coffreCommun.setFer(coffreCommun.getFer()+2);
                    }
                }

                if(message == "bouleau")
                {
                    if (joueur->hache==1) {
                        coffreCommun.setBouleau(coffreCommun.getBouleau()+0);
                    }
                    if (joueur->hache==2) {
                        coffreCommun.setBouleau(coffreCommun.getBouleau()+0);
                    }
                    if (joueur->hache==3) {
                        coffreCommun.setBouleau(coffreCommun.getBouleau()+1);
                    }
                    if (joueur->hache==4) {
                        coffreCommun.setBouleau(coffreCommun.getBouleau()+1);
                    }
                }

                if(message == "patate")
                {
                    if (joueur->houe==1) {
                        coffreCommun.setPatate(coffreCommun.getPatate()+0);
                    }
                    if (joueur->houe==2) {
                        coffreCommun.setPatate(coffreCommun.getPatate()+0);
                    }
                    if (joueur->houe==3) {
                        coffreCommun.setPatate(coffreCommun.getPatate()+0);
                    }
                    if (joueur->houe==4) {
                        coffreCommun.setPatate(coffreCommun.getPatate()+1);
                    }
                }

                if(message == "diamant")
                {
                    if (joueur->pioche==1) {
                        coffreCommun.setDiamant(coffreCommun.getDiamant()+0);
                    }
                    if (joueur->pioche==2) {
                        coffreCommun.setDiamant(coffreCommun.getDiamant()+0);
                    }
                    if (joueur->pioche==3) {
                        coffreCommun.setDiamant(coffreCommun.getDiamant()+0);
                    }
                    if (joueur->pioche==4) {
                        coffreCommun.setDiamant(coffreCommun.getDiamant()+1);
                    }
                }

                if(message == "sapin")
                {
                    if (joueur->hache==1) {
                        coffreCommun.setSapin(coffreCommun.getSapin()+0);
                    }
                    if (joueur->hache==2) {
                        coffreCommun.setSapin(coffreCommun.getSapin()+0);
                    }
                    if (joueur->hache==3) {
                        coffreCommun.setSapin(coffreCommun.getSapin()+0);
                    }
                    if (joueur->hache==4) {
                        coffreCommun.setSapin(coffreCommun.getSapin()+1);
                    }
                }
                break;

            case 'r':
                if(zone2Verif == false && coffreCommun.getBle() >= 50 && coffreCommun.getRoche() >= 50)
                {
                    coffreCommun.setBle(coffreCommun.getBle()-50);
                    coffreCommun.setRoche(coffreCommun.getRoche()-50);
                    zone2Verif = true;
                }

                if(zone3Verif == false && coffreCommun.getCarotte() >= 50 && coffreCommun.getFer() >= 50)
                {
                    coffreCommun.setCarotte(coffreCommun.getCarotte()-50);
                    coffreCommun.setFer(coffreCommun.getFer()-50);
                    zone3Verif = true;
                }

                break;
            case 'h':

                if (joueur->getHoue()==1) {
                    if(coffreCommun.getBle() >= 100 )
                    {
                        joueur->setHoue(joueur->getHoue()+1);
                        coffreCommun.setBle(coffreCommun.getBle() - 100);
                    }

                }
                if (joueur->getHoue()==2) {
                    if(coffreCommun.getBle() >= 300 )
                    {
                        joueur->setHoue(joueur->getHoue()+1);
                        coffreCommun.setBle(coffreCommun.getBle() - 300);
                    }

                }
                if (joueur->getHoue()==3) {
                    if(coffreCommun.getCarotte() >= 500 )
                    {
                        joueur->setHoue(joueur->getHoue()+1);
                        coffreCommun.setCarotte(coffreCommun.getCarotte() - 500);
                    }

                }
                break;

            case 'i':

                if (joueur->getPioche()==1) {
                    if(coffreCommun.getRoche() >= 100 )
                    {
                        joueur->setPioche(joueur->getPioche()+1);
                        coffreCommun.setRoche(coffreCommun.getRoche() - 100);
                    }

                }
                if (joueur->getPioche()==2) {
                    if(coffreCommun.getRoche() >= 300 )
                    {
                        joueur->setPioche(joueur->getPioche()+1);
                        coffreCommun.setRoche(coffreCommun.getRoche() - 300);
                    }

                }
                if (joueur->getPioche()==3) {
                    if(coffreCommun.getFer() >= 500 )
                    {
                        joueur->setPioche(joueur->getPioche()+1);
                        coffreCommun.setFer(coffreCommun.getFer() - 500);
                    }

                }
                break;

            case 'o':

                if (joueur->getHache()==1) {
                    if(coffreCommun.getChene() >= 100 )
                    {
                        joueur->setHache(joueur->getHache()+1);
                        coffreCommun.setChene(coffreCommun.getChene() - 100);
                    }

                }
                if (joueur->getHache()==2) {
                    if(coffreCommun.getChene() >= 300 )
                    {
                        joueur->setHache(joueur->getHache()+1);
                        coffreCommun.setChene(coffreCommun.getChene() - 300);
                    }

                }
                if (joueur->getHache()==3) {
                    if(coffreCommun.getBouleau() >= 500 )
                    {
                        joueur->setHache(joueur->getHache()+1);
                        coffreCommun.setBouleau(coffreCommun.getBouleau() - 500);
                    }

                }
                break;
            case 'p':
                if (joueur->getEpee() == 0 &&
                        coffreCommun.getChene() >= 100 &&
                        coffreCommun.getRoche() >= 100 &&
                        coffreCommun.getBle() >= 100) {

                    joueur->setEpee(joueur->getEpee() + 1);
                    coffreCommun.setChene(coffreCommun.getChene() - 100);
                    coffreCommun.setRoche(coffreCommun.getRoche() - 100);
                    coffreCommun.setBle(coffreCommun.getBle() - 100);
                }

                if (joueur->getEpee() == 1 &&
                        coffreCommun.getChene() >= 200 &&
                        coffreCommun.getRoche() >= 200 &&
                        coffreCommun.getBle() >= 200 &&
                        coffreCommun.getBouleau() >= 100 &&
                        coffreCommun.getCarotte() >= 100 &&
                        coffreCommun.getFer() >= 100) {

                    joueur->setEpee(joueur->getEpee() + 1);
                    coffreCommun.setChene(coffreCommun.getChene() - 200);
                    coffreCommun.setRoche(coffreCommun.getRoche() - 200);
                    coffreCommun.setBle(coffreCommun.getBle() - 200);
                    coffreCommun.setBouleau(coffreCommun.getBouleau() - 100);
                    coffreCommun.setFer(coffreCommun.getFer() - 100);
                    coffreCommun.setCarotte(coffreCommun.getCarotte() - 100);
                }


                if (joueur->getEpee() == 2 &&
                        coffreCommun.getChene() >= 300 &&
                        coffreCommun.getRoche() >= 300 &&
                        coffreCommun.getBle() >= 300 &&
                        coffreCommun.getBouleau() >= 200 &&
                        coffreCommun.getCarotte() >= 200 &&
                        coffreCommun.getFer() >= 200) {

                    joueur->setEpee(joueur->getEpee() + 1);
                    coffreCommun.setChene(coffreCommun.getChene() - 300);
                    coffreCommun.setRoche(coffreCommun.getRoche() - 300);
                    coffreCommun.setBle(coffreCommun.getBle() - 300);
                    coffreCommun.setBouleau(coffreCommun.getBouleau() - 200);
                    coffreCommun.setFer(coffreCommun.getFer() - 200);
                    coffreCommun.setCarotte(coffreCommun.getCarotte() - 200);
                }

                if (joueur->getEpee() == 3 &&
                        coffreCommun.getChene() >= 500 &&
                        coffreCommun.getRoche() >= 500 &&
                        coffreCommun.getBle() >= 500 &&
                        coffreCommun.getBouleau() >= 300 &&
                        coffreCommun.getCarotte() >= 300 &&
                        coffreCommun.getFer() >= 300 &&
                        coffreCommun.getSapin() >= 200 &&
                        coffreCommun.getPatate() >= 200 &&
                        coffreCommun.getDiamant() >= 200) {

                    joueur->setEpee(joueur->getEpee() + 1);
                    coffreCommun.setChene(coffreCommun.getChene() - 500);
                    coffreCommun.setRoche(coffreCommun.getRoche() - 500);
                    coffreCommun.setBle(coffreCommun.getBle() - 500);
                    coffreCommun.setBouleau(coffreCommun.getBouleau() - 300);
                    coffreCommun.setFer(coffreCommun.getFer() - 300);
                    coffreCommun.setCarotte(coffreCommun.getCarotte() - 300);
                    coffreCommun.setSapin(coffreCommun.getSapin() - 200);
                    coffreCommun.setPatate(coffreCommun.getPatate() - 200);
                    coffreCommun.setDiamant(coffreCommun.getDiamant() - 200);
                }
                break;
            }
        }
        if(checkZone(joueur) != -1)
        {
            joueur->setPos(newPos);
        }
        else
        {
            //joueur->setPos(QPoint(20, 120));
        }
        envoyerDonneesAll();
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

void Serveur::onQTimer_timeout()
{

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
        out<<taille<<commande<<index<<listePosition<<coffreCommun
          <<checkPositions(joueurCourant)<<messageValide
         <<zone2Verif<<zone3Verif;
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

bool Serveur::checkPositions(Joueur *joueur)
{
    int valid = 0;
    messageValide = "no";

    QPoint pos = joueur->getPos();
    //Commande -> f
    // ble
    if (pos.x() >= 243 && pos.x() <= 243 + 100 && pos.y() <= 245 && pos.y() >= 245-80)
    {
        qDebug() << joueur->getUsername() << " est dans le blé";
        messageValide = "ble";
        valid++;
    }

    // roche
    if (pos.x() >= 90 && pos.x() <= 90 + 100 && pos.y() <= 240 && pos.y() >= 240-60)
    {
        qDebug() << joueur->getUsername() << " est dans la roche";
        messageValide = "roche";
        valid++;
    }

    // chene
    if (pos.x() >= 225 && pos.x() <= 225 + 85 && pos.y() <= 100 && pos.y() >= 100-60)
    {
        qDebug() << joueur->getUsername() << " est dans le bois de chêne";
        messageValide = "chene";
        valid++;
    }

    // carotte
    if (pos.x() >= 605 && pos.x() <= 605 + 80 && pos.y() <= 115 && pos.y() >= 115-70)
    {
        qDebug() << joueur->getUsername() << " est dans la carotte";
        messageValide = "carotte";
        valid++;
    }

    // fer
    if (pos.x() >= 615 && pos.x() <= 615 + 75 && pos.y() <= 240 && pos.y() >= 240-70)
    {
        qDebug() << joueur->getUsername() << " est dans le fer";
        messageValide = "fer";
        valid++;
    }

    // bouleau
    if (pos.x() >= 400 && pos.x() <= 400 + 85 && pos.y() <= 115 && pos.y() >= 115-70)
    {
        qDebug() << joueur->getUsername() << " est dans le bois de bouleau";
        messageValide = "bouleau";
        valid++;
    }

    // patate
    if (pos.x() >= 590 && pos.x() <= 590 + 80 && pos.y() <= 377 && pos.y() >= 377-70)
    {
        qDebug() << joueur->getUsername() << " est dans la patate";
        messageValide = "patate";
        valid++;
    }

    // diamant
    if (pos.x() >= 415 && pos.x() <= 415 + 100 && pos.y() <= 375 && pos.y() >= 375-65)
    {
        qDebug() << joueur->getUsername() << " est dans le diamant";
        messageValide = "diamant";
        valid++;
    }

    // sapin
    if (pos.x() >= 232 && pos.x() <= 232 + 85 && pos.y() <= 376 && pos.y() >= 376-70)
    {
        qDebug() << joueur->getUsername() << " est dans le bois de sapin";
        messageValide = "sapin";
        valid++;
    }


    if(valid == 0)
    {
        // Faire truc
        // faire fonction pour chaque case qui va enable bouton farm chez client
        // si valid == 0 alors disable chez le joueur
        return false;
    }
    else
    {
        return true;
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
    if ((zone2Verif == false && pos.x() >= 414) || (zone2Verif == false && pos.x()+40 >= 414))
    {
        pos.setX(pos.x()-20);
        joueur->setPos(pos);
        return -1;
    }

    if ((zone3Verif == false && pos.y() >= 313) || (zone3Verif == false && pos.y()+70 >= 313))
    {
        pos.setY(pos.y()-20);
        joueur->setPos(pos);
        return -1;
    }

    return 0;
}
