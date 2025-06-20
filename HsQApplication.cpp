// SPDX-FileCopyrightText: 2025 Jesper Schmitz Mourdisen
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "HsQApplication.h"
bool HsQApplication::notify(QObject *object, QEvent * event) {
    //Workaround for focusloss on dynamic menus.. it ends up deactivting,
    // which closes all popups.
    if(event->type()==QEvent::ApplicationDeactivate) {
        return true;

    }

    return QApplication::notify(object,event);

}
