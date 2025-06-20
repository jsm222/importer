/*
 * SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef HSQAPPLICATION_H
#define HSQAPPLICATION_H
#include <QApplication>
class HsQApplication: public QApplication {
    Q_OBJECT
public:
    //bool eventFilter(QObject *obj,QEvent *e) override;
    bool notify(QObject *, QEvent *) override;
    HsQApplication(int &argc, char **argv, int = ApplicationFlags):QApplication(argc,argv,ApplicationFlags) {

    }



};

#endif // HSQAPPLICATION_H
