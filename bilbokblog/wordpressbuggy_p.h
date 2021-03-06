/*
  This file is part of the kblog library.

  Copyright (c) 2007 Mike Arthur <mike@mikearthur.co.uk>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#ifndef WORDPRESSBUGGY_P_H
#define WORDPRESSBUGGY_P_H

#include "bilbokblog/wordpressbuggy.h"
#include "bilbokblog/movabletype_p.h"

#include <kxmlrpcclient/client.h>

class KJob;
class QByteArray;
template <class T,class S>class QMap;

namespace KIO
{
  class Job;
}

namespace KBlog {

class WordpressBuggyPrivate : public MovableTypePrivate
{
  public:
    QMap<KJob *,QByteArray> mCreatePostBuffer;
    QMap<KJob *,KBlog::BlogPost *> mCreatePostMap;
    QMap<KJob *,QByteArray> mModifyPostBuffer;
    QMap<KJob *,KBlog::BlogPost *> mModifyPostMap;
    QMap<int, QMap<QString, QVariant> > mCreateCategoryMap;
    WordpressBuggyPrivate();
    virtual ~WordpressBuggyPrivate();
    virtual QList<QVariant> defaultArgs( const QString &id = QString() );

    virtual void slotCreatePost( KJob * );
    virtual void slotCreatePostData( KIO::Job *, const QByteArray & );
    virtual void slotModifyPost( KJob * );
    virtual void slotModifyPostData( KIO::Job *, const QByteArray & );
    virtual void slotCategoryError( int, const QString&, const QVariant& );

    void slotCreateCategory(const QList<QVariant>& result,
                             const QVariant& id);
    Q_DECLARE_PUBLIC( WordpressBuggy )
};

}

#endif
