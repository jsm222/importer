/*
 * SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPMENUREGISTRAR_H
#define APPMENUREGISTRAR_H
#include <QtDBus/QtDBus>
#include <QObject>
#include <QVariant>
#include <QObject>
#include <QDebug>
#include <QMenu>
#include <QtDBus/QDBusContext>
#include <QtDBus/QDBusAbstractInterface>
#include <QtDBus/QtDBus>

class AppMenuRegistrar : public QObject,protected QDBusContext
{
    Q_OBJECT

public:
    explicit AppMenuRegistrar(QObject *parent = nullptr);
    QMap<int ,QString> regWindowServices;

    QMap<int ,QMenu*> cachedMenus;
    QMap<int ,QMap<int,QAction*>> cachedActions;
    QMap<int ,QDBusObjectPath> regWindowObjectPaths;
public slots:
    void GetMenuForWindow(uint windowId);
    Q_NOREPLY void RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath);
    Q_NOREPLY void UnregisterWindow(uint windowId);
signals:
    void windowRegistered(uint winId, QString service,QString path);
    void windowUnregistered(uint winId);
};

#endif // APPMENUREGISTRAR_H
