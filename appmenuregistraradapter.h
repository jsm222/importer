/*
 * SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef APPMENUREGISTRARADAPTER_H
#define APPMENUREGISTRARADAPTER_H

#include <QDBusAbstractAdaptor>
#include <QtDBus/QtDBus>



class AppMenuRegistrarAdapter : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.AppMenu.Registrar")
    Q_CLASSINFO("D-Bus Introspection",
                "  <interface name=\"com.canonical.AppMenu.Registrar\">\n"
                "    <method name=\"RegisterWindow\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
                "      <arg direction=\"in\" type=\"o\" name=\"menuObjectPath\"/>\n"
                "    </method>\n"
                "    <method name=\"UnregisterWindow\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
                "    </method>\n"
                "    <method name=\"GetMenuForWindow\">\n"
                "      <arg direction=\"in\" type=\"u\" name=\"windowId\"/>\n"
                "      <arg direction=\"out\" type=\"s\" name=\"service\"/>\n"
                "      <arg direction=\"out\" type=\"o\" name=\"menuObjectPath\"/>\n"
                "    </method>\n"
                "  </interface>\n"
                "")
public:
 AppMenuRegistrarAdapter (QObject *parent);
    virtual ~AppMenuRegistrarAdapter();


public: // PROPERTIES


public Q_SLOTS: // METHODS
Q_NOREPLY void GetMenuForWindow(uint windowId);
void RegisterWindow(uint windowId, const QDBusObjectPath &menuObjectPath);
Q_NOREPLY void UnregisterWindow(uint windowId);
Q_SIGNALS: // SIGNALS
};

#endif
