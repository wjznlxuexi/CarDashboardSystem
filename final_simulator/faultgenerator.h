#ifndef FAULTGENERATOR_H
#define FAULTGENERATOR_H

#include <QObject>

class FaultGenerator : public QObject
{
    Q_OBJECT
public:
    explicit FaultGenerator(QObject *parent = 0);

signals:

public slots:

};

#endif // FAULTGENERATOR_H
