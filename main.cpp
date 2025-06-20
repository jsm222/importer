// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mainwindow.h"

#include <QApplication>
#include <QApplication>


#include "mainwindow.h"
#include "HsQApplication.h"


int main(int argc, char *argv[])
{
    qputenv("QT_QPA_NO_NATIVE_MENUBAR", "1");



    qDBusRegisterMetaType<MenuItem>();
    qDBusRegisterMetaType<SectionItem>();
    qDBusRegisterMetaType<SubMenu>();
    qDBusRegisterMetaType<QList<MenuItem>>();
    qDBusRegisterMetaType<QDBusMenuItem>();
    qDBusRegisterMetaType<QDBusMenuItemList>();

    qDBusRegisterMetaType<QDBusMenuItemKeys>();
    qDBusRegisterMetaType<QDBusMenuItemKeysList>();
    HsQApplication a(argc, argv);

    MainWindow w;




     /*   QDBusInterface iface2(service, path, "org.gtk.Actions", QDBusConnection::sessionBus());
        QVariantList args2;
        QList<QVariant> l;
        QVariantMap m;

        args2        << "-Hj--lp-1" << QVariant::fromValue(l) << QVariant::fromValue(m);
        iface2.callWithArgumentList(QDBus::Block,"Activate",args2);
        qDebug() << iface2.lastError();
*/







    w.show();
    /*    w.show();
    if (replyMsg.type() == QDBusMessage::ReplyMessage &&
        replyMsg.arguments().size() == 2) {

        // First return value: uint
        uint revision = replyMsg.arguments().at(0).toUInt();

        // Second return value: QDBusArgument containing layout
        QDBusArgument dbusArg = qvariant_cast<QDBusArgument>(replyMsg.arguments());

        Layout layout;


        //qDebug() << layout.id;
    } else {
        qDebug() << "Call failed or unexpected return format:";
    }
*/
    return a.exec();

}



