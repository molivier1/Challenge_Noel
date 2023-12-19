#ifndef FENETREJEU_H
#define FENETREJEU_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class FenetreJeu; }
QT_END_NAMESPACE

class FenetreJeu : public QWidget
{
    Q_OBJECT

public:
    FenetreJeu(QWidget *parent = nullptr);
    ~FenetreJeu();

private:
    Ui::FenetreJeu *ui;
};
#endif // FENETREJEU_H
