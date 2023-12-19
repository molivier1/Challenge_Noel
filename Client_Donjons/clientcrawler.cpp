#include "clientcrawler.h"
#include "ui_clientcrawler.h"

ClientCrawler::ClientCrawler(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ClientCrawler)
{
    ui->setupUi(this);
}

ClientCrawler::~ClientCrawler()
{
    delete ui;
}

