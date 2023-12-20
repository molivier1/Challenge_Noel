#ifndef QGRAPHICSVIEWPERSO_H
#define QGRAPHICSVIEWPERSO_H

#include "qtypes.h"
#include <QKeyEvent>
#include <QMouseEvent>
#include <QGraphicsView>
#include <QGraphicsEllipseItem>

class QGraphicsViewPerso: public QGraphicsView
{

    Q_OBJECT
public:
    QGraphicsViewPerso(QWidget *parent = nullptr);
    void mouseMoveEvent(QMouseEvent *event);
signals:
    void positionSouris(QPoint pos);

};


#endif // QGRAPHICSVIEWPERSO_H
