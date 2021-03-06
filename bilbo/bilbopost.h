/***************************************************************************
 *   This file is part of the Bilbo Blogger.                               *
 *   Copyright (C) 2008-2009 Mehrdad Momeny <mehrdad.momeny@gmail.com>     *
 *   Copyright (C) 2008-2009 Golnaz Nilieh <g382nilieh@gmail.com>          *
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

#ifndef BILBOPOST_H
#define BILBOPOST_H

// #include <QDateTime>
// #include <QUrl>

// #include <kurl.h>
// #include <kdatetime.h>
#include <bilbokblog/blogpost.h>
#include "category.h"
class QUrl;
class QDateTime;
class KUrl;
class KDateTime;
// class KBlog::BlogPost;

/**
Definition of a blog post!
it's implemented to decrease dependency to KBlog :)
 @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
 @author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class BilboPost : public KBlog::BlogPost
{
public:
//  enum Position{
//   Published=0, Draft, Local
//  };
    BilboPost();
    BilboPost( const KBlog::BlogPost& );
    BilboPost( const BilboPost& );
    ~BilboPost();

    QString author() const;
    void setAuthor( const QString& );

    int id() const;
    void setId( const int );

//  Position position() const;
//  void setPosition( const Position);

    KBlog::BlogPost * toKBlogPost();
    QString toString() const;

    bool isModifyTimeStamp() const;
    void setModifyTimeStamp( bool willModify );

    QList<Category> categoryList() const;
    void setCategoryList( const QList<Category> &list );

    /**
     * Set all properties of post to new one, instead of Title and Content!
    */
    void setProperties( const BilboPost& postProp );
private:
    QString mAuthor;
    int mId;///id in DB
//  Position mPosition;
    bool mModifyTimeStamp;///Just for toolbox entry!
    QList<Category> mCategoryList;
};

#endif
