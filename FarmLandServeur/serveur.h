#ifndef SERVEUR_H
#define SERVEUR_H

#include <QWidget>
#include <QTcpSocket>
#include <QTcpServer>

QT_BEGIN_NAMESPACE
namespace Ui { class Serveur; }
QT_END_NAMESPACE

class Serveur : public QWidget
{
    Q_OBJECT

public:
    Serveur(QWidget *parent = nullptr);
    ~Serveur();

private slots:


private:
    Ui::Serveur *ui;

    QList <QTcpSocket *> listeSocketsClients;
    QTcpServer *socketServeur;
};
#endif // SERVEUR_H
