/*
    KWin - the KDE window manager
    This file is part of the KDE project.

    SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <QDBusAbstractAdaptor>

namespace KWin
{

class AnlandConsumerStatusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.anland.Consumer")
    Q_PROPERTY(bool Active READ active NOTIFY ActiveChanged)

public:
    explicit AnlandConsumerStatusAdaptor(QObject *parent);

    bool active() const;
    void setActive(bool active);

Q_SIGNALS:
    void ActiveChanged(bool active);

private:
    bool m_active = false;
};

} // namespace KWin
