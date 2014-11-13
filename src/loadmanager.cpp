#include "loadmanager.h"
#include "utils.h"

LoadManager::LoadManager(QObject *parent)
    : QObject(parent)
    , m_taskContextMenuRequested(!useDelayedLoading())
    , m_aboutPageRequested(!useDelayedLoading())
    , m_configurePageRequested(!useDelayedLoading())
    , m_configurePopupRequested(!useDelayedLoading())
    , m_archiveRequested(!useDelayedLoading())
    , m_taskListRequested(true)
    , m_questionPopupRequested(!useDelayedLoading())
    , m_inlineEditorRequested(!useDelayedLoading())
{
}

void LoadManager::setTaskContextMenuRequested(bool taskContextMenuRequested)
{
    if (taskContextMenuRequested != m_taskContextMenuRequested) {
        m_taskContextMenuRequested = taskContextMenuRequested;
        emit taskContextMenuRequestedChanged();
    }
}

bool LoadManager::taskContextMenuRequested() const
{
    return m_taskContextMenuRequested;
}

bool LoadManager::useDelayedLoading() const
{
    return Utils::isMobile();
}

void LoadManager::setAboutPageRequested(bool aboutPageRequested)
{
    if (aboutPageRequested != m_aboutPageRequested) {
        m_aboutPageRequested = aboutPageRequested;
        emit aboutPageRequestedChanged();
    }
}

bool LoadManager::aboutPageRequested() const
{
    return m_aboutPageRequested;
}

void LoadManager::setConfigurePageRequested(bool configurePageRequested)
{
    if (configurePageRequested != m_configurePageRequested) {
        m_configurePageRequested = configurePageRequested;
        emit configurePageRequestedChanged();
    }
}

bool LoadManager::configurePageRequested() const
{
    return m_configurePageRequested;
}

void LoadManager::setConfigurePopupRequested(bool configurePopupRequested)
{
    if (configurePopupRequested != m_configurePopupRequested) {
        m_configurePopupRequested = configurePopupRequested;
        emit configurePopupRequestedChanged();
    }
}

bool LoadManager::configurePopupRequested() const
{
    return m_configurePopupRequested;
}

void LoadManager::setTaskListRequested(bool taskListRequested)
{
    if (taskListRequested != m_taskListRequested) {
        m_taskListRequested = taskListRequested;
        emit taskListRequestedChanged();
    }
}

bool LoadManager::taskListRequested() const
{
    return m_taskListRequested;
}

void LoadManager::setArchiveRequested(bool archiveRequested)
{
    if (archiveRequested != m_archiveRequested) {
        m_archiveRequested = archiveRequested;
        emit archiveRequestedChanged();
    }
}

bool LoadManager::archiveRequested() const
{
    return m_archiveRequested;
}

void LoadManager::setQuestionPopupRequested(bool questionPopupRequested)
{
    if (questionPopupRequested != m_questionPopupRequested) {
        m_questionPopupRequested = questionPopupRequested;
        emit questionPopupRequestedChanged();
    }
}

bool LoadManager::questionPopupRequested() const
{
    return m_questionPopupRequested;
}

void LoadManager::setInlineEditorRequested(bool inlineEditorRequested)
{
    if (inlineEditorRequested != m_inlineEditorRequested) {
        m_inlineEditorRequested = inlineEditorRequested;
        emit inlineEditorRequestedChanged();
    }
}

bool LoadManager::inlineEditorRequested() const
{
    return m_inlineEditorRequested;
}
