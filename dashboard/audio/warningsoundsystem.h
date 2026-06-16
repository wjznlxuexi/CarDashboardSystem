#ifndef WARNINGSOUNDSYSTEM_H
#define WARNINGSOUNDSYSTEM_H

#include <QObject>
#include <QSoundEffect>


class WarningSoundSystem : public QObject
{
    Q_OBJECT
public:
    explicit WarningSoundSystem(QObject *parent = nullptr);

    // 信号区
signals:
    void currentSoundChanged(int warningType);

    // 对外槽接口（任务书规定全部保留）
public slots:
    void playWarningSound(int warningType);
    void stopCurrentSound();
    void setMuted(bool muted);
    void setVolume(int volume);
    void testSound(int warningType);

    // 私有成员，全部m_开头
private:
    QSoundEffect *m_audioPlayer;
    QString m_soundPath[8];
    bool m_isMuted;
    int m_currentVol;
};

#endif // WARNINGSOUNDSYSTEM_H
