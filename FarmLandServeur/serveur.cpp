#include "serveur.h"
#include "ui_serveur.h"

Serveur::Serveur(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Serveur)
{
    ui->setupUi(this);
}

Serveur::~Serveur()
{
    delete ui;
}

