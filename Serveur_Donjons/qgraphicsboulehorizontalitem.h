#ifndef QGRAPHICSBOULEHORIZONTALITEM_H
#define QGRAPHICSBOULEHORIZONTALITEM_H

#include <QGraphicsEllipseItem>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QMediaPlayer>
#include <QAudioOutput>

class QGraphicsBouleHorizontalItem : public QGraphicsEllipseItem
{
public:
    QGraphicsBouleHorizontalItem(qreal x,qreal y, qreal width, qreal height, QGraphicsItem *parent = nullptr);
protected:
    void advance(int step) override;
private:
    qreal dx,dy;
    QMediaPlayer player;
    QAudioOutput sortieAudio;
};

#endif // QGRAPHICSBOULEHORIZONTALITEM_H
