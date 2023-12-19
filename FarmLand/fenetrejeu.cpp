#include "fenetrejeu.h"
#include "ui_fenetrejeu.h"

FenetreJeu::FenetreJeu(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FenetreJeu)
{
    ui->setupUi(this);
}

FenetreJeu::~FenetreJeu()
{
    delete ui;
}

