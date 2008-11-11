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
#ifndef BILBOENGINE_H
#define BILBOENGINE_H



#include <QObject>
#include <QMap>
// #include <kurl.h>
#include <kblog/blog.h>

class KUrl;
class BilboBlog;
class BilboPost;
/**
Engine of application, communicate with plugins and UI.
this is heart and brain of app. ;)

	@author Mehrdad Momeny <mehrdad.momeny@gmail.com>
	@author Golnaz Nilieh <g382nilieh@gmail.com>
*/
class Backend : public QObject
{
	Q_OBJECT
public:
	Backend( int blog_id, QObject* parent=0);

    ~Backend();
	
	/**
	 * Request to Fetch categories list from server.
	 * and after it's fetched, categoriesListed() SLOT will insert them to database, and update list in database
	 * and emit categoriesFetched() SIGNAL.
	 * @param blog_id Id of blog in DB!
	 */
	void getCategoryListFromServer();
	void getEntriesListFromServer(int count);
	void publishPost(BilboPost *post);
    
protected slots:
	void categoriesListed(const QList< QMap< QString, QString > > &   categories   );
	
	void entriesListed(const QList< KBlog::BlogPost > &posts);
	
	void postPublished(KBlog::BlogPost *post);
	
	
signals:
    /**
     * emit when a categoriesListed() Done and Categories added to DB
     * @param blog_id id of Blog owner of categories.
     */
    void sigCategoryListFetched( int blog_id );
	/**
	 * emit when a entriesListed() Done and Entries added to DB
	 * @param blog_id id of Blog owner of Entries.
	 */
	void sigEntriesListFetched( int blog_id );
	/**
	 * Emits when a post published and added to Database.
	 * @param blog_id blog id.
	 * @param post_id post id in Database.
	 * @param  
	 */
	void sigPostPublished(int blog_id, int post_id);
	
private:
	KBlog::Blog *mBlog;
	BilboBlog *bBlog;
};

#endif
