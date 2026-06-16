#ifndef WARNINGSOUNDSYSTEM_H
#define WARNINGSOUNDSYSTEM_H

#include <QObject>
#include <QVector>

class QSoundEffect;

class WarningSoundSystem : public QObject
{
    Q_OBJECT

public:
    explicit WarningSoundSystem(QObject *parent = NULL);

public slots:
    void playWarningSound(int warningType);
    void stopCurrentSound();
    void setMuted(bool muted);
    void setVolume(int volume);
    void testSound(int warningType);

signals:
    void currentSoundChanged(int warningType);

private:
    void loadSounds();
    bool isValidWarningType(int warningType) const;

    QVector<QSoundEffect *> m_sounds;
    int m_currentWarningType;
    int m_volume;
    bool m_muted;
};

#endif // WARNINGSOUNDSYSTEM_H
