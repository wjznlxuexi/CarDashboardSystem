#ifndef SIGNALGENERATOR_H
#define SIGNALGENERATOR_H

#include <QObject>

class SignalGenerator : public QObject
{
    Q_OBJECT
public:
    explicit SignalGenerator(QObject *parent = 0);

signals:

public slots:

};

#endif // SIGNALGENERATOR_H
