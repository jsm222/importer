/*
 * SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef DBUSI_H
#define DBUSI_H
#include <QDBusArgument>

struct SubMenu{
    uint subScriptionGroup;
    uint menuNumber;

};
struct SectionItem {
    uint subScriptionGroup;
    uint menuNumber;

};
struct MenuItem {
    uint subscriptionGroup;
    uint menuNumber;
    QList<QVariantMap> properties;
};
struct Layout {
    int id;
    QMap<QString,QVariant> properties;
    QVariantList children;

};
typedef QPair<int, QVariantMap> QDBusMenuItem;
typedef QList<QDBusMenuItem> QDBusMenuItemList;

typedef QPair<int, QStringList> QDBusMenuItemKeys;
typedef QList<QDBusMenuItemKeys> QDBusMenuItemKeysList;


QDBusArgument &operator<<(QDBusArgument &arg, const SectionItem &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, SectionItem &item);
QDBusArgument &operator<<(QDBusArgument &arg, const SubMenu &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, SubMenu &item);
QDBusArgument &operator<<(QDBusArgument &arg, const Layout &layout);
const QDBusArgument &operator>>(const QDBusArgument &arg, Layout &layout);
QDBusArgument &operator<<(QDBusArgument &arg, const MenuItem &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, MenuItem &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, QDBusMenuItem &item);
QDBusArgument &operator<<(QDBusArgument &arg, const QDBusMenuItem &item);
const QDBusArgument &operator>>(const QDBusArgument &arg, QDBusMenuItemKeys &item);
QDBusArgument &operator<<(QDBusArgument &arg, const QDBusMenuItemKeys &item);
Q_DECLARE_METATYPE(MenuItem)
Q_DECLARE_METATYPE(SubMenu)
Q_DECLARE_METATYPE(SectionItem);
Q_DECLARE_METATYPE(QList<MenuItem>)
Q_DECLARE_METATYPE(Layout)
Q_DECLARE_METATYPE(QDBusMenuItem)
Q_DECLARE_METATYPE(QDBusMenuItemList)
Q_DECLARE_METATYPE(QDBusMenuItemKeys)
Q_DECLARE_METATYPE(QDBusMenuItemKeysList)
#endif // DBUSI_H
