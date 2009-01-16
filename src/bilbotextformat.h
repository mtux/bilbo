/***************************************************************************
 *   Copyright (C) 2008 by Mehrdad Momeny, Golnaz Nilieh   *
 *   mehrdad.momeny@gmail.com, g382nilieh@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef BILBOTEXTFORMAT_H
#define BILBOTEXTFORMAT_H

#include <qtextformat.h>

/**
 * Adds some textformat attributes which don't exist in QTextFormat class.
 * this class may be removed in future, if all editor related staff be ordered as a lib.
 *
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
*/
// class BilboTextCharFormat : public QTextCharFormat
class BilboTextFormat
{
    //Q_OBJECT
public:
//   BilboTextCharFormat();
//
//   ~BilboTextCharFormat();

    //BilboTextCharFormat& operator =(const BilboTextCharFormat& other);

    enum Property {
        /// Anchor properties
        AnchorTitle = 0x100010,
        AnchorTarget = 0x100011,

        /// Image properties
        ImageTitle = 0x100020,
        ImageAlternateText = 0x100021,
    };
//   enum TargetStyle {
//    OpenInCurrentWindow = 1,
//       OpenInNewWindow = 2
//   };

//   bool isValid() const;
//
//   inline void setAnchorTitle(const QString &title) {
//    setProperty(AnchorTitle, title);
//   }
//   inline QString anchorTitle() const {
//    return stringProperty(AnchorTitle);
//   }
//   inline void setAnchorTarget(TargetStyles style) {
//    setProperty(AnchorTarget, style);
//   }
//   inline TargetStyles anchorTarget() const {
//    return static_cast<TargetStyles>(intProperty(AnchorTitle));
//   }
//
//  protected:
//   explicit BilboTextCharFormat(const QTextFormat &format);
//
//   friend class QTextFormat;
};

#endif
