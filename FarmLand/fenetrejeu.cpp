#include "fenetrejeu.h"
#include "ui_fenetrejeu.h"
#include "inventaire.h"
#include <QMessageBox>
#include <QCursor>
#include <QPoint>
#include <iostream>
#include <QTimer>
#include <QGridLayout>

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

    /*joueur = new QGraphicsPixmapItem(QPixmap("/home/USERS/ELEVES/SNIR2022/tperichet/Documents/C++/Challenge_Noel/FarmLand/candy.png"));
    joueur->setScale(0.25);
    maScene.addItem(joueur);
    joueur->hide();*/

    joueur = new QGraphicsPixmapItem(QPixmap("/home/USERS/ELEVES/SNIR2022/tperichet/Documents/C++/Challenge_Noel/FarmLand/Gorgious.png"));
    joueur->setScale(0.25);
    maScene.addItem(joueur);
    joueur->hide();
    setFocus();

    QPixmap ble("../FarmLand/ble.png");
    QPixmap tailleImageBle = ble.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelBle->setPixmap(tailleImageBle);

    QPixmap Carotte("../FarmLand/Carotte.png");
    QPixmap tailleImageCarotte = Carotte.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelCarotte->setPixmap(tailleImageCarotte);

    QPixmap patate("../FarmLand/patate.png");
    QPixmap tailleImagePatate= patate.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelPatate->setPixmap(tailleImagePatate);

    QPixmap BucheSapin("../FarmLand/bucheSapin.png");
    QPixmap tailleImageBuche = BucheSapin.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelSapin->setPixmap(tailleImageBuche);

    QPixmap bucheBouleau("../FarmLand/bucheBouleau.png");
    QPixmap tailleImageBucheBouleau = bucheBouleau.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelBouleau->setPixmap(tailleImageBucheBouleau);

    QPixmap bucheChene("../FarmLand/BucheChene.png");
    QPixmap tailleImageBucheChene= bucheChene.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelChene->setPixmap(tailleImageBucheChene);

    QPixmap fer("../FarmLand/fer.png");
    QPixmap tailleImageFer = fer.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelFer->setPixmap(tailleImageFer);

    QPixmap Roche("../FarmLand/Roche.png");
    QPixmap tailleImageRoche = Roche.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelRoche->setPixmap(tailleImageRoche);

    QPixmap Diamant("../FarmLand/diamant.png");
    QPixmap tailleImageDiamant = Diamant.scaled(QSize(30, 30), Qt::KeepAspectRatio);
    ui->labelDiamant->setPixmap(tailleImageDiamant);

    zone2 = false;
    zone3 = false;

    QPolygon polyZone2;
    QPolygon polyZone3;
    polyZone2.setPoints(4,
                        414, 45,
                        737, 45,
                        737, 313,
                        414, 313);

    polyZone3.setPoints(4,
                        740,314,
                        740,448,
                        90,447,
                        90,313);

    zone2Item = new QGraphicsPolygonItem(polyZone2);
    zone3Item = new QGraphicsPolygonItem(polyZone3);

    QBrush semiTransparentBrush(QColor(0, 0, 0, 200));
    zone2Item->setBrush(semiTransparentBrush);
    zone3Item->setBrush(semiTransparentBrush);

    QPen contour(Qt::black);
    contour.setWidth(2);
    zone2Item->setPen(contour);
    zone3Item->setPen(contour);

    maScene.addItem(zone2Item);
    maScene.addItem(zone3Item);
}
FenetreJeu::~FenetreJeu()
{
    delete ui;
    delete socketFarmLand;;
}

void FenetreJeu::on_pushButtonNouvelleZone_clicked()
{

    /*
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
    */

    EnvoyerCommande('r');
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

    joueur->show();
}

void FenetreJeu::onQTcpSocket_disconnected()
{
    ui->pushButtonNouvelleZone->setEnabled(false);
    maVue->setEnabled(false);
    ui->lineEditAdresse->setEnabled(false);
    ui->spinBoxPort->setEnabled(false);

    zone2Item->show();
    zone3Item->show();
}

void FenetreJeu::onQTcpSocket_readyRead()
{
    quint16 taille = 0;
    QChar commande;
    bool valide;
    int index;
    QPoint newPos;
    QString username;
    Inventaire nouveauCoffre;

    QList <QPoint> listePosition;

    // Il y a au moins le champs taille d'arrive
    if (socketFarmLand->bytesAvailable() >= (qint64)sizeof(taille))
    {
        // Lecture de la taille de la trame
        QDataStream in(socketFarmLand);
        in >> taille;
        // Le reste de la trame est disponible
        if (socketFarmLand->bytesAvailable() >= (qint64)taille)
        {
            // Lecture de la commande
            in>>commande;
            switch (commande.toLatin1()) {
            // Haut
            case 'a':
                in>>index>>listePosition >> nouveauCoffre>>valide>>message>>zone2>>zone3;

                // log debug
                qDebug() << index << " + " << listePosition.at(index);
                qDebug() << valide << message;

                qDebug() << "zone 2 : " << zone2;
                qDebug() << "zone 3 : " << zone3;

                coffre = nouveauCoffre;

                newPos = listePosition.at(index);
                joueur->setPos(newPos);

                if (zone2 == true)
                {
                    zone2Item->hide();
                }
                else
                {
                    zone2Item->show();
                }

                if (zone3 == true) {
                    zone3Item->hide();
                }
                else
                {
                    zone3Item->show();
                }
                actualiserRessources();
                break;

            }
        }
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
    out<<taille<<commande<<message;
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
        //joueur->moveBy(-5,0);
        qDebug()<<"gauche";
        break;
    case Qt::Key_6:
        EnvoyerCommande('6');
        //joueur->moveBy(5,0);
        qDebug()<<"droit";
        // if(joueur)
        break;
    case Qt::Key_8:
        EnvoyerCommande('8');
        //joueur->moveBy(0,-5);
        qDebug()<<"haut";
        break;
    case Qt::Key_2:
        EnvoyerCommande('2');
        //joueur->moveBy(0,5);
        qDebug()<<"bas";
        break;
    case Qt::Key_7:
        EnvoyerCommande('7');
        //joueur->moveBy(-5,5);
        qDebug()<<"DiagonaleHautGauche";
        break;
    case Qt::Key_9:
        EnvoyerCommande('9');
        //joueur->moveBy(5,5);
        qDebug()<<"DiagonaleHautDroite";
        break;
    case Qt::Key_1:
        EnvoyerCommande('1');
        //joueur->moveBy(-5,-5);
        qDebug()<<"DiagonaleBasGauche";
        break;
    case Qt::Key_3:
        EnvoyerCommande('3');
        //joueur->moveBy(5,-5);
        qDebug()<<"DiagonaleBasDroite";
        break;

    case Qt::Key_F:
        EnvoyerCommande('f');
        qDebug() << "Touche farm pressé";
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
    QPoint cursorPos = QCursor::pos();
    qDebug() << "Coordonnées du curseur de la souris : " << cursorPos.x() << ", " << cursorPos.y();
}

void FenetreJeu::onQGraphicsViewPerso_positionSouris(QPoint pos)
{

}

void FenetreJeu::actualiserRessources()
{
    ui->labelValBle->setText(QString::number(coffre.getBle()));
    ui->labelValBouleau->setText(QString::number(coffre.getBouleau()));
    ui->labelValCarotte->setText(QString::number(coffre.getCarotte()));
    ui->labelValChene->setText(QString::number(coffre.getChene()));
    ui->labelValDiamant->setText(QString::number(coffre.getDiamant()));
    ui->labelValFer->setText(QString::number(coffre.getFer()));
    ui->labelValPatate->setText(QString::number(coffre.getPatate()));
    ui->labelValRoche->setText(QString::number(coffre.getRoche()));
    ui->labelValSapin->setText(QString::number(coffre.getSapin()));
}

