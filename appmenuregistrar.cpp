// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "appmenuregistrar.h"
#include <QtDBus/QtDBus>
AppMenuRegistrar::AppMenuRegistrar(QObject *parent)
    : QObject{parent}
{}
void AppMenuRegistrar::RegisterWindow(uint id, const QDBusObjectPath &path) {


    qDebug() << id << path.path() << __FILE__ ":" << __LINE__ << __func__<< message().service();

    emit windowRegistered(id,message().service(),path.path());
    return;
}

void AppMenuRegistrar::UnregisterWindow(uint windowId) {
    QVariantList replyArgs;

    QDBusMessage reply = message().createReply(replyArgs);
    connection().send(reply);
    emit windowUnregistered(windowId);

    return;
}


void AppMenuRegistrar::GetMenuForWindow(uint windowId) {
    qDebug() << __func__;
    QVariantList replyArgs;
    replyArgs << QVariant(regWindowServices[windowId])
              << QVariant::fromValue(regWindowObjectPaths[windowId] );

    QDBusMessage reply = message().createReply(replyArgs);
    connection().send(reply);

}





