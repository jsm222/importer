// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "mainwindow.h"
#include <QScreen>
#include "appmenuregistraradapter.h"
#include "./ui_mainwindow.h"
#include "mainwindow.h"
#include "dbusi.h"

static const char *DBUS_SERVICE = "com.canonical.AppMenu.Registrar";
static const char *DBUS_OBJECT_PATH = "/com/canonical/AppMenu/Registrar";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    aThread = new X11Thread();
    QObject::connect(aThread, &X11Thread::windowChanged,this,[this](unsigned long aWinId,QStringList gtk) {
        qDebug() << __func__ << __LINE__ << gtk;
     emit this->windowChanged(aWinId,gtk);

 });
    const QRect screenGeometry = qApp->screens()[0]->availableGeometry();


    aThread->start();
    QPalette palette = this->palette();

    // Get the background color (for the 'Window' role)
    QColor bgColor = palette.color(QPalette::Window);
    qDebug() << bgColor;
    this->setStyleSheet("QMenuBar::item{background-color:"+bgColor.name()+";}");
    move(0,0);
    setFixedWidth(screenGeometry.width());
    setWindowFlags(Qt::FramelessWindowHint| Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_X11DoNotAcceptFocus,true);
    setAttribute(Qt::WA_X11NetWmWindowTypeDock,true);
    setFocusPolicy(Qt::NoFocus);
    Display *dsp = XOpenDisplay(NULL);
    setFixedHeight(22);
    setFixedWidth(screenGeometry.width());
    long strut0[4] = { 0, 0,size().height(), 0 }; // top = 30
    long strut1[12] = {
                       0, 0,size().height(), 0,
                       0, 0, 0, 0,
                       0,screenGeometry.width()-1,0,0};

    Atom netWmStrut = XInternAtom(dsp, "_NET_WM_STRUT", False);
    Atom atom = XInternAtom(dsp, "_NET_WM_STRUT_PARTIAL", False);
    XChangeProperty(dsp, winId(), netWmStrut, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char*)strut0, 4);
    XChangeProperty(dsp, winId(), atom, XA_CARDINAL, 32, PropModeReplace,
                    (unsigned char*)strut1, 12);
    menu = new QMenu("gtk");
    XCloseDisplay(dsp);


    QDBusConnection::sessionBus().registerService(DBUS_SERVICE);
    qDebug()<<QDBusConnection::sessionBus().lastError();
    a = new AppMenuRegistrar(this);
    AppMenuRegistrarAdapter *impl = new AppMenuRegistrarAdapter(a);
    QDBusConnection::sessionBus().registerObject(DBUS_OBJECT_PATH, a);
    qDebug()<<QDBusConnection::sessionBus().lastError();
    qRegisterMetaType<Layout>("Layout");
    qDBusRegisterMetaType<Layout>();
    ui->setupUi(this);

    ui->menubar->setFocusPolicy(Qt::NoFocus);

    connect(a,&AppMenuRegistrar::windowUnregistered,this,[this](int aWinId) {
        qDebug() << __LINE__;
        if(a->regWindowServices.contains(aWinId)) {
        a->regWindowServices.remove(aWinId);
        a->regWindowObjectPaths.remove(aWinId);
        a->cachedMenus.remove(aWinId);
        a->cachedActions.remove(aWinId);
        ui->menubar->clear();
        }

    });
    connect(a,&AppMenuRegistrar::windowRegistered,this,[this](int aWinId,QString service,QString path) {
        qDebug() << aWinId << " " << " " << cWinId << __LINE__;
        if(aWinId == winId()) {
            qDebug() << "returning do not register myself window";
            return;
        }
        mutex.lock();
        qDebug() <<a->regWindowObjectPaths << a->regWindowServices;
        a->regWindowServices[aWinId]=service;
        a->regWindowObjectPaths[aWinId]=QDBusObjectPath(path);
        mutex.unlock();
        bool connected = QDBusConnection::sessionBus().connect(
            service,            // service name (empty means any)
            path,
            "com.canonical.dbusmenu",
            "LayoutUpdated","ui",
            this,
            SLOT(onLayoutUpdated(uint, int)));



    });
    connect(this,&MainWindow::windowChanged,this,[this](uint aWinId,QStringList gtk) {
        mutex.lock();
        cWinId = aWinId;
        mutex.unlock();
        if(aWinId == 0)
            return;
        if(gtk.size()==2 && !a->regWindowServices.contains(aWinId)) {
        a->regWindowServices[aWinId]=gtk[0];
        a->regWindowObjectPaths[aWinId]=QDBusObjectPath(gtk[1]);
        }
        if(gtk.size()==2) {
            ui->menubar->clear();
            if (!a->cachedMenus.contains(aWinId)) {
                a->cachedMenus[aWinId]=new QMenu("top",this);
            getLayoutGtk(gtk,{0},a->cachedMenus[aWinId]);

            }
            ui->menubar->addActions(a->cachedMenus[aWinId]->actions());
            return;
        }
        if(aWinId == winId())
            return;

        cWinId = aWinId;
        // emit windowSeen(aWinId);
        mutex.lock();
        cWinId = aWinId;
        qDebug()<<a->regWindowServices<<  a->regWindowObjectPaths << aWinId;
        if(a->regWindowServices.contains(aWinId) && a->regWindowObjectPaths.contains(aWinId)) {
            if(!mConnected.contains(aWinId) || !mConnected[aWinId] ) {

        mConnected[aWinId] = QDBusConnection::sessionBus().connect(
            a->regWindowServices[aWinId],
            a->regWindowObjectPaths[aWinId].path(),
            "com.canonical.dbusmenu",
            "ItemsPropertiesUpdated",    "a(ia{sv})a(ias)",
            this,
                SLOT(onItemsPropertiesUpdated(QDBusMenuItemList,QDBusMenuItemKeysList)));
            qDebug() << mConnected[aWinId] << __LINE__<< QDBusConnection::sessionBus().lastError();

        }
        }
                mutex.unlock();

        if(a->cachedMenus.contains(aWinId)) {
            ui->menubar->clear();
            ui->menubar->addActions(a->cachedMenus[aWinId]->actions().at(0)->menu()->actions());
            return;
            }
        if(!(a->regWindowServices.contains(cWinId)))
            return;
     QDBusInterface iface(
        a->regWindowServices[cWinId],
        a->regWindowObjectPaths[cWinId].path(),
        "com.canonical.dbusmenu",        // Interface name
        QDBusConnection::sessionBus(),  // Bus connection
        nullptr);
qDebug()<<QDBusConnection::sessionBus().lastError();
    if (!iface.isValid()) {
        qDebug() << "D-Bus interface is not valid!";
        return;
    }

    qDebug()<<QDBusConnection::sessionBus().lastError();

    QVariantList args;
    args << 0                  // parentId
         << -1                   // recursionDepth
         << QStringList();  // propertyNames

    QDBusPendingCall pCall= iface.asyncCallWithArgumentList(
                                  "GetLayout", args
        );
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this,aWinId](QDBusPendingCallWatcher *call) {
        QDBusMessage msg = call->reply();
        if(msg.arguments().size()>1) {
        QDBusArgument arg1 = msg.arguments().at(1).value<QDBusArgument>();


    Layout layout;
    arg1 >> layout;
    QVariantList arg12;
/*    arg12 << layout.id;
    QDBusInterface iface(
        a->regWindowServices[cWinId],
        a->regWindowObjectPaths[cWinId].path(),
        "com.canonical.dbusmenu",        // Interface name
        QDBusConnection::sessionBus(),  // Bus connection
        nullptr);
    iface.callWithArgumentList(QDBus::Block,"AboutToShow",arg12);
*/

    // qDebug() << layout.children.at(0).value<QDBusArgument>().currentSignature();
    // qDebug() << layout.id  << layout.properties << layout.children;
    ui->menubar->clear();
    a->cachedMenus[aWinId]=new QMenu("tasdsad",this);
    a->cachedMenus[aWinId]->setFocusPolicy(Qt::NoFocus);
    recurse(layout,a->cachedMenus[aWinId]);
    a->cachedActions[cWinId][layout.id]=a->cachedMenus[aWinId]->menuAction();
    //ui->menubar->addMenu(a->cachedMenus[aWinId]);
    ui->menubar->addActions(a->cachedMenus[aWinId]->actions().at(0)->menu()->actions());

        }
//ui->menubar->addMenu(menu);
    });

    });
}
void MainWindow::getLayoutGtk(QStringList gtk,QList<uint> menuIds,QMenu *subMenu) {
    QString service = gtk[0];
    QString path = gtk[1];
    QString interface = "org.gtk.Menus";
    QString method = "Start";


    QDBusInterface iface1(service, path, interface, QDBusConnection::sessionBus());

    // Prepare options dictionary (empty)
    QVariantMap options;

    // Wrap write_fd in QDBusUnixFileDescriptor


    // Call the method synchronously
    QVariantList args1;

    QList<MenuItem> items;
    args1 << QVariant::fromValue(menuIds);
    QDBusMessage msg = iface1.callWithArgumentList(QDBus::Block,method, args1);
    if (msg.type() == QDBusMessage::ReplyMessage) {
        qDebug()<< msg.arguments().size();
        QVariant arg = msg.arguments().at(0);

        qDebug() << msg.arguments().at(0).typeName();
        const QDBusArgument dbusArg = qvariant_cast<QDBusArgument>(arg);
        qDebug() << dbusArg.currentSignature();
        dbusArg >> items;

    }
    QList<quint32> menuIdsList;
    QMap<int,QList<QAction*>> acts;
    QMenu *subMenu1 = new QMenu(this);


    for (int i = 0; i < items.size(); i++) {


        for (int j = 0; j < items[i].properties.size(); j++) {


            if(items[i].properties[j].contains(":section")) {
                const QDBusArgument arg1 = items[i].properties[j][":section"].value<QDBusArgument>();
                SectionItem aSection;
                arg1 >> aSection;
                qDebug() <<aSection.subScriptionGroup<<aSection.menuNumber << __LINE__ <<items[i].properties[j];

            } else {
            QString title = items[i].properties[j]["label"].value<QString>().replace("_","&");
            subMenu1->setTitle(items[i].properties[j]["label"].value<QString>().replace("_","&"));







            if(items[i].properties[j].contains(":submenu")) {

                subMenu1->setTitle(items[i].properties[j]["label"].value<QString>().replace("_","&"));
                const QDBusArgument arg1 = items[i].properties[j][":submenu"].value<QDBusArgument>();
                SubMenu aSection;
                arg1 >> aSection;
                menuIdsList << aSection.subScriptionGroup;
                qDebug() <<aSection.subScriptionGroup<<aSection.menuNumber;
                //items[i].properties[j][":submenu"]=QVariant::fromValue(aSection);
                qDebug() << aSection.subScriptionGroup << aSection.menuNumber << __LINE__<< items[i].properties[j];

                getLayoutGtk(gtk,{aSection.subScriptionGroup},subMenu1);
                subMenu->addMenu(subMenu1);
                subMenu1 = new QMenu(this);


            } else {
                QString aTitle = items[i].properties[j]["label"].value<QString>().replace("_","&");
                QAction *a1 = new QAction(aTitle);
              //  qDebug() << items[i].menuNumber << items[i].subscriptionGroup << __LINE__;
                a1->setData(QVariantList{items[i].properties[j]["action"],cWinId});
                int aWinId = cWinId;
                connect(a1,&QAction::triggered,this,[this,aWinId] {
                    QString service = a->regWindowServices[aWinId];
                    QString  path = a->regWindowObjectPaths[aWinId].path();
                    QAction *triggeredgtk = qobject_cast<QAction*>(sender());
                    QString action = triggeredgtk->data().value<QVariantList>().at(0).value<QString>();
                    QDBusInterface iface2(service, path, "org.gtk.Actions", QDBusConnection::sessionBus());
                    QVariantList args2;
                    QList<QVariant> l;
                    QVariantMap m;

                    args2        << QVariant::fromValue(action.replace("unity.","")) << QVariant::fromValue(l) << QVariant::fromValue(m);
                    iface2.callWithArgumentList(QDBus::Block,"Activate",args2);
                    qDebug() << iface2.lastError();
                });
                acts[items[i].menuNumber].append(a1);
            }

            }













        }
/*
        */

    }

    for ( auto [k,v]: acts.asKeyValueRange()) {
        subMenu->addActions(acts[k]);
        subMenu->addSeparator();
    }









    //pff(items);
    //qDebug() << menuIdsList << __LINE__;
    if(menuIdsList.size()>0) {

    }
}
void MainWindow::pff(QList<MenuItem> items) {
    for(MenuItem item : items)  {
        qDebug() << item.properties << item.subscriptionGroup << item.menuNumber <<__LINE__;

    }

}
void MainWindow::onLayoutUpdated(uint revision, int parentId) {

    //unsigned long aWinId = X11Property().getactiveWindow();
    if(a->cachedActions.contains(cWinId) && a->cachedActions[cWinId].contains(parentId)) {
        qDebug()<<a->cachedActions[cWinId][parentId] << __LINE__;
        getLayout(a->cachedActions[cWinId][parentId],parentId);

    }
}
void MainWindow::getLayout(QAction *action,int parentId) {
    qDebug() << action<< parentId << __LINE__;
    if(action->data().value<int>()==parentId) {
        qDebug() << action<<__LINE__;
     QDBusInterface iface(
        a->regWindowServices[cWinId],
        a->regWindowObjectPaths[cWinId].path(),
        "com.canonical.dbusmenu",        // Interface name
        QDBusConnection::sessionBus(),  // Bus connection
        nullptr);
qDebug()<<QDBusConnection::sessionBus().lastError();
    if (!iface.isValid()) {
        qDebug() << "D-Bus interface is not valid!";
        return;
    }

    qDebug()<<QDBusConnection::sessionBus().lastError();

    QVariantList args;
    args << parentId                  // parentId
         << -1                   // recursionDepth
         << QStringList();  // propertyNames

    QDBusPendingCall pCall= iface.asyncCallWithArgumentList(
                                  "GetLayout", args
        );
    int aWinId = cWinId;
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pCall, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this,aWinId,action,parentId](QDBusPendingCallWatcher *call) {
        QDBusMessage msg = call->reply();
        qDebug()<<msg.arguments().size() << __LINE__;
       // qDebug()<<msg.arguments().at(0) << __LINE__;
        if(msg.arguments().size()>1) {
        QDBusArgument arg1 = msg.arguments().at(1).value<QDBusArgument>();


    Layout layout;
    arg1 >> layout;


    // qDebug() << layout.children.at(0).value<QDBusArgument>().currentSignature();
    qDebug() << layout.id  << layout.properties << layout.children<<__LINE__ << action;
    qDebug()<< action->menu()->title() << __LINE__;
    action->menu()->clear();
    recurse(layout,action->menu());

        }






    });
    } else if(action->menu()) {
        for(QAction *b : action->menu()->actions()) {
            getLayout(b,parentId);
        }
    }
    if(action->menu())
        action->menu()->blockSignals(false);
}
void MainWindow::updateAction(QAction *action,QVariantMap properties) {


    //if(action->data().value<int>()==id) {
        if (properties.contains("visible") && properties["visible"].isValid())
            action->setVisible(properties["visible"].value<bool>());
        if(properties.contains("enabled") &&  properties["enabled"].isValid()) {
            action->setEnabled(properties["enabled"].value<bool>());
            //qDebug() << action->data().value<int>() << " " << id <<" "<< __LINE__ << action << properties<< properties["enabled"].value<bool>();
        }
        if(properties.contains("label") &&  properties["label"].isValid()) {
            action->setText(properties["label"].value<QString>().replace("_","&"));
        }
   //         qDebug() << action->data().value<int>() << " " << id <<" "<< __LINE__ << action << properties<< properties["label"].value<QString>();
        //}
//qDebug() ()==id) {<< action->data().value<int>() << " " << id <<" "<< __LINE__ << action << properties;
        return;
    //}
        /*
    if(action->menu()) {
        for(QAction *a:action->menu()->actions()) {


            updateAction(a,id,properties);


        }
    }

    */



    //return;

}
void MainWindow::onItemsPropertiesUpdated(QDBusMenuItemList updates,QDBusMenuItemKeysList removed) {

        qDebug() << updates << __LINE__;
        qDebug() << removed << __LINE__;

        int aWinId = cWinId;

    for (auto update :updates) {

        if(a->cachedActions.contains(aWinId) && a->cachedActions[aWinId].contains(update.first))
            updateAction(a->cachedActions[aWinId][update.first],update.second);



    }
}




void MainWindow::recurse(Layout layout,QMenu *subMenu) {
    QMenu * subMenu1;
    qDebug() << layout.children << __LINE__ << layout.properties;
    if(layout.properties.contains("children-display") && layout.properties["children-display"].value<QString>() == QStringLiteral("submenu")) {
        subMenu1 = new QMenu();

        QString title = layout.properties["label"].value<QString>().replace("_","&");
        subMenu1->setTitle(title);

        qDebug() <<layout.id << __LINE__ << title;
        if (layout.properties.contains("visibile") && layout.properties["visible"].isValid())
            subMenu1->menuAction()->setVisible(layout.properties["visible"].value<bool>());
        if(layout.properties.contains("enabled") &&  layout.properties["enabled"].isValid())
            subMenu1->menuAction()->setEnabled(layout.properties["enabled"].value<bool>());
        int winId = cWinId;
        connect(subMenu1,&QMenu::aboutToShow,this,[this,layout,winId,subMenu1]() {

            QVariantList args1;
            args1 << QVariant::fromValue(layout.id);
            QDBusInterface iface(
                a->regWindowServices[winId],
                a->regWindowObjectPaths[winId].path(),
                "com.canonical.dbusmenu",        // Interface name
                QDBusConnection::sessionBus(),  // Bus connection
                nullptr);
            QDBusPendingCall pCall= iface.asyncCallWithArgumentList(
                "AboutToShow", args1
                );
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(pCall, this);
            //qApp->blockSignals(true);
            connect(watcher, &QDBusPendingCallWatcher::finished, this, [this,winId](QDBusPendingCallWatcher *call) {

                QDBusReply<bool> reply = call->reply();
                qDebug() << reply << __LINE__;

            });
        });


        QAction * ma = subMenu->addMenu(subMenu1);
        ma->setData(QVariant::fromValue(layout.id));
        a->cachedActions[cWinId][layout.id]=ma;
        qDebug() << __LINE__ << " " <<layout.id;
    }

        for (QVariant child : layout.children) {


        Layout layout1;
        child.value<QDBusArgument>() >> layout1;

        if(layout1.properties.contains("label") && !layout1.properties.contains("children-display")) {

            QString aTitle = layout1.properties["label"].value<QString>().replace("_","&");
            qDebug() <<layout1.id << __LINE__ << aTitle;
            QAction *a1 = new QAction(aTitle);

            a1->setData(QVariant::fromValue(layout1.id));
            int myWinId = cWinId;
            connect(a1,&QAction::triggered,this,[this,myWinId] {

                QAction *triggered1 = qobject_cast<QAction*>(sender());
                int qvid = triggered1->data().value<int>();
                QDBusVariant v("");
                uint timestamp = 0;
                QVariantList args;
                qDebug() <<qvid;
                args            <<  QVariant::fromValue(qvid) << QVariant::fromValue(QString("clicked")) << QVariant::fromValue(v) << timestamp;
                qDebug() << args<<__LINE__;
                QDBusInterface iface(
                    a->regWindowServices[myWinId],
                    a->regWindowObjectPaths[myWinId].path(),
                    "com.canonical.dbusmenu",        // Interface name
                    QDBusConnection::sessionBus(),  // Bus connection
                    nullptr);
                iface.callWithArgumentList(QDBus::Block,"Event",args);
                qDebug() <<iface.lastError()<<__LINE__;
            });
            if(layout1.properties["visible"].isValid())
                a1->setVisible(layout1.properties["visible"].value<bool>());
            if(layout1.properties["enabled"].isValid())
                a1->setEnabled(layout1.properties["enabled"].value<bool>());
            a->cachedActions[cWinId][layout1.id]=a1;
            subMenu1->addAction(a1);



        } else if (layout1.properties.contains("type") && layout1.properties["type"].value<QString>()==QStringLiteral("separator")) {
            if(layout1.properties.contains("visible") && layout1.properties["visible"].value<bool>())     {
                subMenu1->addSeparator();
            }
        }


        recurse(layout1,subMenu1);
        }




   //  qDebug() << layout.properties << __LINE__;




    //qDebug() << layout.id << layout.properties.keys();
}
MainWindow::~MainWindow()
{

    aThread->terminate();
    delete ui;
}
