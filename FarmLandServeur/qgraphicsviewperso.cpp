#include "qgraphicsviewperso.h"
#include "qevent.h"
#include "qgraphicsscene.h"
#include "qgraphicsview.h"
#include "qtypes.h"

QGraphicsViewPerso::QGraphicsViewPerso(QWidget *parent):QGraphicsView (parent)
{
    setMouseTracking(false);
    setAlignment(Qt::AlignLeft|Qt::AlignTop);
}

void QGraphicsViewPerso::mouseMoveEvent(QMouseEvent *event)
{
    qDebug() << "dans event souris de la vue : " << event->pos();
}