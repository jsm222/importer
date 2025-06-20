// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dbusi.h"


QDBusArgument &operator<<(QDBusArgument &arg, const Layout &layout) {
    arg.beginStructure();
    arg << layout.id << layout.properties << layout.children;


    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, Layout &layout) {
    arg.beginStructure();

    arg >> layout.id >> layout.properties >> layout.children;

    arg.endStructure();
    return arg;
}


const QDBusArgument &operator>>(const QDBusArgument &arg, MenuItem &item)
{

    arg.beginStructure();
    arg >> item.subscriptionGroup >> item.menuNumber >> item.properties;

    arg.endStructure();
    return arg;
}
QDBusArgument &operator<<(QDBusArgument &arg, const MenuItem &item)
{

    arg.beginStructure();
    arg << item.subscriptionGroup<<item.menuNumber << item.properties;
        ///<< item.sectionItem <<item.subMenu;
    arg.endStructure();

    return arg;
}
QDBusArgument &operator<<(QDBusArgument &arg, const SubMenu &item)
{

    arg.beginStructure();
    arg << item.subScriptionGroup<<item.menuNumber;
        ///<< item.sectionItem <<item.subMenu;
    arg.endStructure();

    return arg;
}
const QDBusArgument &operator>>(const QDBusArgument &arg, SubMenu &item)
{

    arg.beginStructure();
    arg >> item.subScriptionGroup >> item.menuNumber;
    arg.endStructure();

    return arg;
}
QDBusArgument &operator<<(QDBusArgument &arg, const SectionItem &item)
{

    arg.beginStructure();
    arg << item.subScriptionGroup<<item.menuNumber;

    arg.endStructure();

    return arg;
}


const QDBusArgument &operator>>(const QDBusArgument &arg, QList<MenuItem> &itemlist)
{


    arg.beginArray();
    while(!arg.atEnd()) {


        MenuItem item;

        arg >> item;





        itemlist.append(item);
    }
    arg.endArray();


    return arg;
}


const QDBusArgument &operator>>(const QDBusArgument &arg, SectionItem &item)
{

    arg.beginStructure();
    arg >> item.subScriptionGroup >> item.menuNumber;
    arg.endStructure();

    return arg;
}
const QDBusArgument &operator>>(const QDBusArgument &arg, QDBusMenuItem &item) {
    arg.beginStructure();
    arg >> item.first >> item.second;
    arg.endStructure();
    return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const QDBusMenuItem &item) {
    arg.beginStructure();
    arg << item.first << item.second;
    arg.endStructure();
    return arg;
}

const QDBusArgument &operator>>(const QDBusArgument &arg, QDBusMenuItemKeys &item) {
    arg.beginStructure();
    arg >> item.first >> item.second;
    arg.endStructure();
    return arg;
}

QDBusArgument &operator<<(QDBusArgument &arg, const QDBusMenuItemKeys &item) {
    arg.beginStructure();
    arg << item.first << item.second;
    arg.endStructure();
    return arg;
}
