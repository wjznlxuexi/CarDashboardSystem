#include "warningsoundsystem.h"

#include "common/warningtype.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSoundEffect>
#include <QStringList>
#include <QUrl>
#include <QtGlobal>

static QUrl soundUrlForFile(const QString &fileName)
{
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString currentDir = QDir::currentPath();
    const QStringList candidates = QStringList()
            << appDir + "/../resources/sounds/" + fileName
            << appDir + "/resources/sounds/" + fileName
            << currentDir + "/resources/sounds/" + fileName
            << currentDir + "/dashboard/resources/sounds/" + fileName;

    for(int i = 0; i < candidates.size(); ++i)
    {
        const QFileInfo info(QDir::cleanPath(candidates[i]));
        if(info.exists())
            return QUrl::fromLocalFile(info.absoluteFilePath());
    }

    return QUrl("qrc:/sounds/" + fileName);
}

WarningSoundSystem::WarningSoundSystem(QObject *parent)
    : QObject(parent),
      m_currentWarningType(-1),
      m_volume(80),
      m_muted(false)
{
    loadSounds();
}

void WarningSoundSystem::loadSounds()
{
    const char *paths[Warning_Count] = {
        "warning_engine.wav",
        "warning_oil.wav",
        "warning_battery.wav",
        "warning_temp.wav",
        "warning_fuel.wav",
        "warning_brake.wav",
        "warning_airbag.wav",
        "warning_abs.wav"
    };

    m_sounds.reserve(Warning_Count);
    for(int i = 0; i < Warning_Count; ++i)
    {
        QSoundEffect *sound = new QSoundEffect(this);
        sound->setSource(soundUrlForFile(paths[i]));
        sound->setLoopCount(QSoundEffect::Infinite);
        sound->setVolume(m_volume / 100.0);
        m_sounds.append(sound);
    }
}

bool WarningSoundSystem::isValidWarningType(int warningType) const
{
    return warningType >= 0 && warningType < m_sounds.size();
}

void WarningSoundSystem::playWarningSound(int warningType)
{
    if(!isValidWarningType(warningType))
        return;

    if(m_currentWarningType == warningType
            && m_sounds[warningType]->isPlaying())
        return;

    stopCurrentSound();
    m_currentWarningType = warningType;
    m_sounds[warningType]->setLoopCount(QSoundEffect::Infinite);

    if(!m_muted)
        m_sounds[warningType]->play();

    emit currentSoundChanged(warningType);
}

void WarningSoundSystem::stopCurrentSound()
{
    if(isValidWarningType(m_currentWarningType))
        m_sounds[m_currentWarningType]->stop();

    if(m_currentWarningType != -1)
    {
        m_currentWarningType = -1;
        emit currentSoundChanged(-1);
    }
}

void WarningSoundSystem::setMuted(bool muted)
{
    if(m_muted == muted)
        return;

    m_muted = muted;
    if(m_muted)
    {
        for(int i = 0; i < m_sounds.size(); ++i)
            m_sounds[i]->stop();
    }
    else if(isValidWarningType(m_currentWarningType))
    {
        m_sounds[m_currentWarningType]->play();
    }
}

void WarningSoundSystem::setVolume(int volume)
{
    m_volume = qBound(0, volume, 100);
    const qreal realVolume = m_volume / 100.0;

    for(int i = 0; i < m_sounds.size(); ++i)
        m_sounds[i]->setVolume(realVolume);
}

void WarningSoundSystem::testSound(int warningType)
{
    if(!isValidWarningType(warningType))
        return;

    stopCurrentSound();
    m_sounds[warningType]->setLoopCount(1);

    if(!m_muted)
        m_sounds[warningType]->play();

    emit currentSoundChanged(warningType);
}
