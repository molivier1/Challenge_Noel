#ifndef QGRAPHICSBOULEHORIZONTALITEM_H
#define QGRAPHICSBOULEHORIZONTALITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QObject>

class QGraphicsBouleHorizontalItem : public QObject, public QGraphicsEllipseItem
{
    Q_OBJECT
public:
    QGraphicsBouleHorizontalItem(qreal x,qreal y, qreal width, qreal height,QObject *parentObj=nullptr, QGraphicsItem *parent = nullptr);
protected:
    void advance(int step) override;
signals:
    void positionBoule(QPointF p);

private:
    qreal dx,dy;
    QMediaPlayer player;
    QAudioOutput sortieAudio;
};

#endif // QGRAPHICSBOULEHORIZONTALITEM_H
