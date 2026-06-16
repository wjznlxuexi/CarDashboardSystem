#include "warningsoundsystem.h"
#include "common/warningtype.h"
#include <QDebug>

WarningSoundSystem::WarningSoundSystem(QObject *parent) :
    QObject(parent),
    m_isMuted(false),
    m_currentVol(80)
{
    // 创建音频播放器实例
    m_audioPlayer = new QSoundEffect(this);
    // 初始音量换算 0~1
    m_audioPlayer->setVolume(m_currentVol / 100.0);

    // 绑定8组报警音效资源路径
    m_soundPath[Warning_Engine]    = ":/sounds/warning_engine.wav";
    m_soundPath[Warning_Oil]       = ":/sounds/warning_oil.wav";
    m_soundPath[Warning_Battery]   = ":/sounds/warning_battery.wav";
    m_soundPath[Warning_Temp]      = ":/sounds/warning_temp.wav";
    m_soundPath[Warning_Fuel]      = ":/sounds/warning_fuel.wav";
    m_soundPath[Warning_Brake]     = ":/sounds/warning_brake.wav";
    m_soundPath[Warning_Airbag]    = ":/sounds/warning_airbag.wav";
    m_soundPath[Warning_ABS]       = ":/sounds/warning_abs.wav";
}

// 循环播放对应故障报警音
void WarningSoundSystem::playWarningSound(int warningType)
{
    if (m_isMuted)
        return;
    if (warningType < 0 || warningType > 7)
    {
        qDebug() << "无效报警编号:" << warningType;
        return;
    }
    // 停止正在播放的旧声音
    m_audioPlayer->stop();
    m_audioPlayer->setSource(QUrl(m_soundPath[warningType]));
    m_audioPlayer->setLoopCount(QSoundEffect::Infinite);
    m_audioPlayer->play();
    emit currentSoundChanged(warningType);
}

// 停止所有音效
void WarningSoundSystem::stopCurrentSound()
{
    m_audioPlayer->stop();
}

// 静音开关
void WarningSoundSystem::setMuted(bool muted)
{
    m_isMuted = muted;
    if (m_isMuted)
        m_audioPlayer->stop();
}

// 设置音量 0~100
void WarningSoundSystem::setVolume(int volume)
{
    if (volume < 0) volume = 0;
    if (volume > 100) volume = 100;
    m_currentVol = volume;
    m_audioPlayer->setVolume(m_currentVol / 100.0);
}

// 单次测试音效，不循环
void WarningSoundSystem::testSound(int warningType)
{
    if (m_isMuted || warningType < 0 || warningType > 7)
        return;
    m_audioPlayer->stop();
    m_audioPlayer->setSource(QUrl(m_soundPath[warningType]));
    m_audioPlayer->setLoopCount(1);
    m_audioPlayer->play();
}
