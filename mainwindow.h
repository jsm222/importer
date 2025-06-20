/*
 * SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QMenu>
#include <QAction>
#include <QMap>
#include <QMutex>
#include "appmenuregistrar.h"
#include <QThread>
#include <QDebug>
#include "dbusi.h"
#include <X11/Xlib.h>
#include <X11/Xatom.h>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class X11Thread;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    void getLayoutGtk(QStringList,QList<uint> menuIds,QMenu *subMenu);
    void updateAction(QAction *action,QVariantMap properties);
    void getLayout(QAction *action,int parentId);
    Ui::MainWindow *ui;
    QMenu * menu;
    AppMenuRegistrar* a;
    void recurse(Layout layout,QMenu *subMenu);
    void pff(QList<MenuItem> items);
    uint cWinId;
    QMutex mutex;
    X11Thread *aThread;
    QMap<int ,bool> mConnected;

private Q_SLOTS:
    void onLayoutUpdated(uint revision, int parentId);
    void onItemsPropertiesUpdated(QDBusMenuItemList updates,QDBusMenuItemKeysList removed);

signals:
    void windowChanged(unsigned long winId,QStringList gtk);
    void windowSeen(uint aWinId);

};
#endif // MAINWINDOW_H
#ifndef X11THREAD_H
#define X11THREAD_H
#include <X11/Xlib.h>
#include <X11/Xatom.h>


class X11Thread : public QThread
{
    Q_OBJECT
signals:
    void windowChanged(unsigned long winId,QList<QString> gtk_settings);
protected:
    void run() override {
        Display *dsp = XOpenDisplay(NULL);
        Window root = DefaultRootWindow(dsp);

        XSelectInput(dsp,root, PropertyChangeMask);
        XEvent e;

        while (1) {
            XNextEvent(dsp,&e);
            QStringList gtk_settings;
            switch(e.type)
            {
            case PropertyNotify: {


                if(e.xproperty.atom==XInternAtom(dsp, "_NET_ACTIVE_WINDOW", True)) {



                        int format;
                        Atom return_type;
                        unsigned long after, nitems, window,bytes_after;
                        unsigned char *data;
                        unsigned char *gtk_dbus_service = NULL;
                        unsigned char *gtk_object_path = NULL;
                        Atom actual_type;
                        int actual_format;
                        qDebug() << XGetWindowProperty(dsp,root,XInternAtom(dsp, "_NET_ACTIVE_WINDOW", False),0,1,False,XA_WINDOW,&return_type,&format,&after,&nitems,&data);
                        qDebug() << *(unsigned long*)data <<__LINE__;
                        int status = XGetWindowProperty(dsp,(Window)*(unsigned long*)data, XInternAtom(dsp, "_GTK_UNIQUE_BUS_NAME", True), 0, 1024, False,
                                                         XInternAtom(dsp, "UTF8_STRING", False), &actual_type, &actual_format,
                                                        &nitems, &bytes_after, &gtk_dbus_service);

                        qDebug() << (Window)*(unsigned long*)data <<__LINE__ << return_type <<status;
                        if(gtk_dbus_service!=NULL) {

                            gtk_settings << QString::fromUtf8(gtk_dbus_service);
                            status = XGetWindowProperty(dsp,(Window)*(unsigned long*)data, XInternAtom(dsp, "_GTK_MENUBAR_OBJECT_PATH", True), 0, 1024, False,
                                                            XInternAtom(dsp, "UTF8_STRING", False), &actual_type, &actual_format,
                                                            &nitems, &bytes_after, &gtk_object_path);


                            if(status==Success)
                                gtk_settings << QString::fromUtf8(gtk_object_path);
                        }

                        emit windowChanged(((unsigned long*)data)[0],gtk_settings);



                    }

                }


            }
        }
    }



};


#endif
