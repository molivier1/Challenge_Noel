#ifndef CLIENTCRAWLER_H
#define CLIENTCRAWLER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class ClientCrawler; }
QT_END_NAMESPACE

class ClientCrawler : public QWidget
{
    Q_OBJECT

public:
    ClientCrawler(QWidget *parent = nullptr);
    ~ClientCrawler();

private:
    Ui::ClientCrawler *ui;
};
#endif // CLIENTCRAWLER_H
