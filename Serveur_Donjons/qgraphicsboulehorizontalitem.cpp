#include "qgraphicsboulehorizontalitem.h"

QGraphicsBouleHorizontalItem::QGraphicsBouleHorizontalItem(qreal x,qreal y, qreal width, qreal height,QObject *parentObj, QGraphicsItem *parent):
    QObject(parentObj),QGraphicsEllipseItem(x,y,width,height,parent),dx(2),dy(2)
{

}

void QGraphicsBouleHorizontalItem::advance(int step)
{
    moveBy(dx,dy); // deplacement de dx pixel horizontalement
    emit positionBoule(pos());

    qreal largeurBoule = rect().width();
    qreal largeurScene = scene()->width();
    qreal hauteurBoule= rect().height();
    qreal hauteurScene = scene()->height();

    //boule arrive sur bord gauche, on avance
    if(x() <= 2){
        dx = 2;
    }
    //boule arrive sur bord droite, on recule
    if(x() >= largeurScene-largeurBoule){
        dx = -2;
    }

    if(y() <= 2){
        dy = 2;
    }
    //boule arrive sur bord droite, on recule
    if(y() >= hauteurScene-hauteurBoule){
        dy = -2;
    }

}
