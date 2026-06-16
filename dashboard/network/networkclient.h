#ifndef NETWORKCLIENT_H
#define NETWORKCLIENT_H

#include <QObject>

class networkclient : public QObject
{
    Q_OBJECT
public:
    explicit networkclient(QObject *parent = 0);

signals:

public slots:

};

#endif // NETWORKCLIENT_H
