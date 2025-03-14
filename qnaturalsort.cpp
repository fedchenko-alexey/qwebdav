/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

// naturalCompare from http://qt.gitorious.org/qt/qt/blobs/4.7/src/gui/dialogs/qfilesystemmodel.cpp

#include "qnaturalsort.h"

static inline QChar getNextChar(const QString &s, int location)
{
    return (location < s.length()) ? s.at(location) : QChar();
}

/*!
    Natural number sort, skips spaces.

    Examples:
    1, 2, 10, 55, 100
    01.jpg, 2.jpg, 10.jpg

    Note on the algorithm:
    Only as many characters as necessary are looked at and at most they all
    are looked at once.

    Slower then QString::compare() (of course)
  */
int QNaturalSort::naturalCompare(const QString &s1, const QString &s2, Qt::CaseSensitivity cs)
{
    for (int l1 = 0, l2 = 0; l1 <= s1.size() && l2 <= s2.size(); ++l1, ++l2) {
        // skip spaces, tabs and 0's
        QChar c1 = getNextChar(s1, l1);
        while (c1.isSpace()) {
            c1 = getNextChar(s1, ++l1);
        }
        QChar c2 = getNextChar(s2, l2);
        while (c2.isSpace()) {
            c2 = getNextChar(s2, ++l2);
        }
        if (c1.isDigit() && c2.isDigit()) {
            while (c1.digitValue() == 0) {
                c1 = getNextChar(s1, ++l1);
            }
            while (c2.digitValue() == 0) {
                c2 = getNextChar(s2, ++l2);
            }
            int lookAheadLocation1 = l1;
            int lookAheadLocation2 = l2;
            int currentReturnValue = 0;
            // find the last digit, setting currentReturnValue as we go if it isn't equal
            for (QChar lookAhead1 = c1, lookAhead2 = c2;
                 (lookAheadLocation1 <= s1.length() && lookAheadLocation2 <= s2.length());
                 lookAhead1 = getNextChar(s1, ++lookAheadLocation1),
                       lookAhead2 = getNextChar(s2, ++lookAheadLocation2)) {
                bool is1ADigit = !lookAhead1.isNull() && lookAhead1.isDigit();
                bool is2ADigit = !lookAhead2.isNull() && lookAhead2.isDigit();
                if (!is1ADigit && !is2ADigit) {
                    break;
                }
                if (!is1ADigit) {
                    return -1;
                }
                if (!is2ADigit) {
                    return 1;
                }
                if (currentReturnValue == 0) {
                    if (lookAhead1 < lookAhead2) {
                        currentReturnValue = -1;
                    } else if (lookAhead1 > lookAhead2) {
                        currentReturnValue = 1;
                    }
                }
            }
            if (currentReturnValue != 0) {
                return currentReturnValue;
            }
        }

        if (cs == Qt::CaseInsensitive) {
            if (!c1.isLower()) {
                c1 = c1.toLower();
            }
            if (!c2.isLower()) {
                c2 = c2.toLower();
            }
        }
        int r = QString::localeAwareCompare(c1, c2);
        if (r < 0) {
            return -1;
        }
        if (r > 0) {
            return 1;
        }
    }
    // The two strings are the same (02 == 2) so fall back to the normal sort
    return QString::compare(s1, s2, cs);
}
