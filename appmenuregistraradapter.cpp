// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appmenuregistraradapter.h"
#include <QtCore/QMetaObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include "appmenuregistrar.h"
/*
 * Implementation of adaptor class MenuImporterAdaptor
 */

AppMenuRegistrarAdapter::AppMenuRegistrarAdapter(QObject *parent)
    : QDBusAbstractAdaptor(parent)
{
    // constructor
    setAutoRelaySignals(true);
}

AppMenuRegistrarAdapter::~AppMenuRegistrarAdapter()
{
    // destructor
}

    Q_NOREPLY void  AppMenuRegistrarAdapter::GetMenuForWindow(uint windowId)
{
    qDebug() << __func__;
    // handle method cal    l com.canonical.AppMenu.Registrar.GetMenuForWindow
    QMetaObject::invokeMethod(parent(), "GetMenuForWindow", Q_ARG(uint, windowId));

}

Q_NOREPLY void AppMenuRegistrarAdapter::RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath)
{
    qDebug() << __func__<<parent() << __LINE__;
    // handle method call com.canonical.AppMenu.Registrar.RegisterWindow
    QMetaObject::invokeMethod(parent(), "RegisterWindow", Q_ARG(uint, windowId), Q_ARG(QDBusObjectPath, menuObjectPath));
}

Q_NOREPLY void AppMenuRegistrarAdapter::UnregisterWindow(uint windowId)
{
    qDebug() << __func__<<parent() << __LINE__;

    // handle method call com.canonical.AppMenu.Registrar.UnregisterWindow
    QMetaObject::invokeMethod(parent(), "UnregisterWindow", Q_ARG(uint, windowId));
}

