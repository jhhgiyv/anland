/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "anland_dbus.h"

namespace KWin
{

AnlandConsumerStatusAdaptor::AnlandConsumerStatusAdaptor(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
}

bool AnlandConsumerStatusAdaptor::active() const
{
    return m_active;
}

void AnlandConsumerStatusAdaptor::setActive(bool active)
{
    if (m_active == active) {
        return;
    }

    m_active = active;
    Q_EMIT ActiveChanged(m_active);
}

} // namespace KWin
