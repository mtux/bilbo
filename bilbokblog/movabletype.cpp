/*
  This file is part of the kblog library.

  Copyright (c) 2004 Reinhold Kainhofer <reinhold@kainhofer.com>
  Copyright (c) 2006-2007 Christian Weilbach <christian_weilbach@web.de>
  Copyright (c) 2007-2008 Mike Arthur <mike@mikearthur.co.uk>

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

#include "bilbokblog/movabletype.h"
#include "bilbokblog/movabletype_p.h"
#include "bilbokblog/blogpost.h"

#include <kxmlrpcclient/client.h>
#include <kio/job.h>

#include <KDebug>
#include <KLocale>
#include <KDateTime>

#include <QtCore/QStringList>

using namespace KBlog;

MovableType::MovableType( const KUrl &server, QObject *parent )
  : MetaWeblog( server, *new MovableTypePrivate, parent )
{
  kDebug() << "MovableType()";
}

MovableType::MovableType( const KUrl &server, MovableTypePrivate &dd,
                        QObject *parent )
  : MetaWeblog( server, dd, parent )
{
  kDebug() << "MovableType()";
}

MovableType::~MovableType()
{
  kDebug() << "~MovableType()";
}

QString MovableType::interfaceName() const
{
  return QLatin1String( "Movable Type" );
}

void MovableType::listRecentPosts( int number )
{
    Q_D( MovableType );
    kDebug() << "Fetching List of Posts...";
    QList<QVariant> args( d->defaultArgs( blogId() ) );
    args << QVariant( number );
    d->mXmlRpcClient->call(
      "metaWeblog.getRecentPosts", args,
      this, SLOT(slotListRecentPosts(const QList<QVariant>&,const QVariant&)),
      this, SLOT(slotError(int,const QString&,const QVariant&)),
      QVariant( number ) );
}

void MovableType::listTrackBackPings( KBlog::BlogPost *post )
{
  Q_D( MovableType );
  kDebug() << "List trackback pings...";
  QList<QVariant> args;
  args << QVariant( post->postId() );
  unsigned int i = d->mCallCounter++;
  d->mCallMap[ i ] = post;
  d->mXmlRpcClient->call(
    "mt.getTracebackPings", args,
    this, SLOT(slotListTrackbackPings(const QList<QVariant>&,const QVariant&)),
    this, SLOT(slotError(int,const QString&,const QVariant&)),
    QVariant( i ) );
}

MovableTypePrivate::MovableTypePrivate()
{
}

MovableTypePrivate::~MovableTypePrivate()
{
  kDebug() << "~MovableTypePrivate()";
}

QList<QVariant> MovableTypePrivate::defaultArgs( const QString &id )
{
  Q_Q( MovableType );
  QList<QVariant> args;
  if( !id.isEmpty() ) {
    args << QVariant( id );
  }
  args << QVariant( q->username() )
       << QVariant( q->password() );
  return args;
}

bool MovableTypePrivate::readPostFromMap( BlogPost *post, const QMap<QString, QVariant> &postInfo )
{

  // FIXME: integrate error handling
  kDebug() << "readPostFromMap()";
  if ( !post ) {
    return false;
  }
  QStringList mapkeys = postInfo.keys();
  kDebug() << endl << "Keys:" << mapkeys.join( ", " );
  kDebug() << endl;

  KDateTime dt =
    KDateTime( postInfo["dateCreated"].toDateTime(), KDateTime::UTC );
  if ( dt.isValid() && !dt.isNull() ) {
    post->setCreationDateTime( dt.toLocalZone() );
  }

  dt =
    KDateTime( postInfo["lastModified"].toDateTime(), KDateTime::UTC );
  if ( dt.isValid() && !dt.isNull() ) {
    post->setModificationDateTime( dt.toLocalZone() );
  }

  post->setPostId( postInfo["postid"].toString() );

  QString title( postInfo["title"].toString() );
  QString description( postInfo["description"].toString() );
  QStringList categories( postInfo["categories"].toStringList() );
  //TODO 2 new keys are:
  // String mt_convert_breaks, the value for the convert_breaks field
  post->setWpSlug( postInfo["wp_slug"].toString() );
  post->setAdditionalContent( postInfo["mt_text_more"].toString() );
  post->setTitle( title );
  post->setContent( description );
  post->setCommentAllowed( (bool)postInfo["mt_allow_comments"].toInt() );
  post->setTrackBackAllowed( (bool)postInfo["mt_allow_pings"].toInt() );
  post->setSummary( postInfo["mt_excerpt"].toString() );
  post->setTags( postInfo["mt_keywords"].toStringList() );
  post->setLink( postInfo["link"].toString() );
  post->setPermaLink( postInfo["permaLink"].toString() );
  QString postStatus = postInfo["post_status"].toString();
  if( postStatus != "publish" && !postStatus.isEmpty() ){
    /**
     * Maybe this field wasn't set by server! so, on that situation, we will assume it as non-Private,
     * The postStatus.isEmpty() check is for that!
     * I found this field on Wordpress output! it's value can be: publish, private, draft (as i see)
    */
    post->setPrivate(true);
  }
  if ( !categories.isEmpty() ){
    kDebug() << "Categories:" << categories;
    post->setCategories( categories );
  }
  return true;
}

void MovableTypePrivate::slotListTrackBackPings(
    const QList<QVariant> &result, const QVariant &id )
{
  Q_Q( MovableType );
  kDebug() << "slotTrackbackPings()";
  BlogPost *post = mCallMap[ id.toInt() ];
  mCallMap.remove( id.toInt() );
  QList<QMap<QString,QString> > trackBackList;
  if ( result[0].type() != QVariant::List ) {
    kError() << "Could not fetch list of trackback pings out of the"
                 << "result from the server.";
    emit q->error( MovableType::ParsingError,
                   i18n( "Could not fetch list of trackback pings out of the "
                         "result from the server." ) );
    return;
  }
  const QList<QVariant> trackBackReceived = result[0].toList();
  QList<QVariant>::ConstIterator it = trackBackReceived.begin();
  QList<QVariant>::ConstIterator end = trackBackReceived.end();
  for ( ; it != end; ++it ) {
    QMap<QString,QString> tping;
    kDebug() << "MIDDLE:" << ( *it ).typeName();
    const QMap<QString, QVariant> trackBackInfo = ( *it ).toMap();
    tping[ "title" ] = trackBackInfo[ "pingTitle"].toString();
    tping[ "url" ] = trackBackInfo[ "pingURL"].toString();
    tping[ "ip" ] = trackBackInfo[ "pingIP"].toString();
    trackBackList << tping;
  }
  kDebug() << "Emitting listedTrackBackPings()";
  emit q->listedTrackBackPings( post, trackBackList );
}

bool MovableTypePrivate::readArgsFromPost( QList<QVariant> *args, const BlogPost &post )
{
  //TODO 3 new keys are:
  // String mt_convert_breaks, the value for the convert_breaks field
  // String mt_text_more, the value for the additional entry text
  // array mt_tb_ping_urls, the list of TrackBack ping URLs for this entry
  if ( !args ) {
    return false;
  }
  QMap<QString, QVariant> map;
  map["categories"] = post.categories();
  map["description"] = post.content();
  if( !post.additionalContent().isEmpty() )
    map["mt_text_more"] = post.additionalContent();
  map["title"] = post.title();
  map["dateCreated"] = post.creationDateTime().toUtc().dateTime();
  map["mt_allow_comments"] = (int)post.isCommentAllowed();
  map["mt_allow_pings"] = (int)post.isTrackBackAllowed();
  map["mt_excerpt"] = post.summary();
  map["mt_keywords"] = post.tags().join(",");
  //map["mt_tb_ping_urls"] check for that, i think this should only be done on the server.
  *args << map;
  *args << QVariant( !post.isPrivate() );
  return true;
}

void MovableType::setPostCategories(const QString &postId, const QMap<QString, bool> &categoriesList)
{
  Q_D( MovableType );
  kDebug();
  int count = categoriesList.count();
  if(count < 1){
    kError() << "MovableType::setPostCategories: the category list is empty";
    emit error ( Other, i18n( "The category list is empty" ) );
    return;
  }

  kDebug() << "Setting"<< count<<"Categories to Post "<< postId<<" with blogId" << blogId();

  QString xmlMarkup = "<?xml version=\"1.0\" encoding=\"utf-8\" ?>";
  xmlMarkup += "<methodCall>";
  xmlMarkup += "<methodName>mt.setPostCategories</methodName>";
  xmlMarkup += "<params><param>";
  xmlMarkup += "<value><string><![CDATA["+postId+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param>";
  xmlMarkup += "<value><string><![CDATA["+username()+"]]></string></value>";
  xmlMarkup += "</param><param>";
  xmlMarkup += "<value><string><![CDATA["+password()+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param><value><array><data>";

  QMap<QString, bool>::ConstIterator it = categoriesList.constBegin();
  QMap<QString, bool>::ConstIterator end = categoriesList.constEnd();
  for ( ; it != end; ++it ){
    xmlMarkup += "<value><struct><member><name>categoryId</name><value><string><![CDATA[" +
    ( it.key() ) + "]]></string></value></member>";
    xmlMarkup += "<member><name>isPrimary</name><value><boolean><![CDATA[" +
    ( QString::number(QVariant(it.value()).toInt()) ) +
                 "]]></boolean></value></member>""</struct></value>";
  }

  xmlMarkup += "</data></array></value></param></params></methodCall>";

  QByteArray postData;
  QDataStream stream( &postData, QIODevice::WriteOnly );
  stream.writeRawData( xmlMarkup.toUtf8(), xmlMarkup.toUtf8().length() );

  KIO::TransferJob *job = KIO::http_post( url(), postData, KIO::HideProgressInfo );

  d->mSetPostCategoriesMap[ job ] = postId;

  if ( !job ) {
    kWarning() << "Failed to create job for: " << url().url();
  }

  job->addMetaData( "content-type", "Content-Type: text/xml; charset=utf-8" );
  job->addMetaData( "ConnectTimeout", "50" );
  job->addMetaData( "UserAgent", userAgent() );

  connect( job, SIGNAL(result(KJob *)),
           this, SLOT(slotSetPostCategories(KJob *)) );
  connect( job, SIGNAL(data(KIO::Job *,const QByteArray &)),
           this, SLOT(slotSetPostCategoriesData(KIO::Job *,const QByteArray &)) );
}

void MovableTypePrivate::slotSetPostCategories(KJob *job)
{
  kDebug();
  const QString data = QString::fromUtf8( mSetPostCategoriesBuffer[ job ].data(),
                                          mSetPostCategoriesBuffer[ job ].size() );
  mSetPostCategoriesBuffer[ job ].resize( 0 );

  Q_Q( MovableType );

  QString postId = mSetPostCategoriesMap[ job ];
  mSetPostCategoriesMap.remove( job );

  if ( job->error() != 0 ) {
    kError() << "slotSetPostCategories error:" << job->errorString();
    emit q->error( MovableType::Other, job->errorString());
    return;
  }

  QRegExp rxId( "<boolean>(.+)</boolean>" );
  if ( rxId.indexIn( data ) == -1 ){
    kError() << "Could not regexp the result out of the result:" << data;
    emit q->error( MovableType::XmlRpc, i18n( "Could not regexp the result out of the result." ));
    return;
  }
  kDebug() << "QRegExp rx( \"<boolean>(.+)</boolean>\" ) matches " << rxId.cap( 1 );

  if( rxId.cap( 1 ) == "1" ){
    kDebug() << "Emitting settedPostCategories(const QString &postId)";
    emit q->settedPostCategories( postId );
  } else {
    emit q->error( MovableType::Other, i18n( "Error on server, Post categories couldn't sets" ));
  }
}

void MovableTypePrivate::slotSetPostCategoriesData(KIO::Job *job,const QByteArray &data)
{
  kDebug();
  mSetPostCategoriesBuffer[ job ].append( data );
}

#include "movabletype.moc"
