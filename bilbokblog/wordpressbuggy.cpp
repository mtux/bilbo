/*
  This file is part of the kblog library.

  Copyright (c) 2006-2007 Christian Weilbach <christian_weilbach@web.de>

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

#include "bilbokblog/wordpressbuggy.h"
#include "bilbokblog/wordpressbuggy_p.h"

#include "bilbokblog/blogpost.h"

#include <KDebug>
#include <KLocale>
#include <KDateTime>

#include <kio/http.h>
#include <kio/job.h>

#include <QtCore/QStringList>

using namespace KBlog;

WordpressBuggy::WordpressBuggy( const KUrl &server, QObject *parent )
  : MovableType( server, *new WordpressBuggyPrivate, parent )
{
  kDebug();
}

WordpressBuggy::WordpressBuggy( const KUrl &server, WordpressBuggyPrivate &dd,
                                QObject *parent )
  : MovableType( server, dd, parent )
{
  kDebug();
}

WordpressBuggy::~WordpressBuggy()
{
  kDebug();
}

void WordpressBuggy::createPost( KBlog::BlogPost *post )
{
  kDebug();
  Q_D( WordpressBuggy );
  if ( !post ) {
    kError() << "WordpressBuggy::createPost: post is a null pointer";
    emit error ( Other, i18n( "Post is a null pointer." ) );
    return;
  }
  kDebug() << "Creating new Post with blogId" << blogId();

  QString xmlMarkup = "<?xml version=\"1.0\"?>";
  xmlMarkup += "<methodCall>";
  xmlMarkup += "<methodName>metaWeblog.newPost</methodName>";
  xmlMarkup += "<params><param>";
  xmlMarkup += "<value><string><![CDATA["+blogId()+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param>";
  xmlMarkup += "<value><string><![CDATA["+username()+"]]></string></value>";
  xmlMarkup += "</param><param>";
  xmlMarkup += "<value><string><![CDATA["+password()+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param><value><struct>";
  xmlMarkup += "<member><name>description</name>";
  xmlMarkup += "<value><string><![CDATA["+post->content()+"]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>title</name>";
  xmlMarkup += "<value><string><![CDATA["+post->title()+"]]></string></value>";
  xmlMarkup += "</member><member>";

  QList<QString> catList = post->categories();
  if ( !catList.empty() ){
    xmlMarkup += "<name>categories</name>";
    xmlMarkup += "<value><array><data>";
    QList<QString>::ConstIterator it = catList.constBegin();
    QList<QString>::ConstIterator end = catList.constEnd();
    for ( ; it != end; ++it ){
      xmlMarkup += "<value><string><![CDATA[" + ( *it ) + "]]></string></value>";
    }
    xmlMarkup += "</data></array></value>";
    xmlMarkup += "</member><member>";
  }
  xmlMarkup += "<name>dateCreated</name>";
  xmlMarkup += "<value><dateTime.iso8601>" +
              post->creationDateTime().dateTime().toUTC().toString( "yyyyMMddThh:mm:ssZ" ) +
              "</dateTime.iso8601></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_allow_comments</name>";
  xmlMarkup += QString( "<value><int>%1</int></value>" ).arg( (int)post->isCommentAllowed() );
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_allow_pings</name>";
  xmlMarkup += QString( "<value><int>%1</int></value>" ).arg( (int)post->isTrackBackAllowed() );
  xmlMarkup += "</member><member>";
  if( !post->additionalContent().isEmpty() ) {
    xmlMarkup += "<name>mt_text_more</name>";
    xmlMarkup += "<value><string><![CDATA[" + post->additionalContent() + "]]></string></value>";
    xmlMarkup += "</member><member>";
  }
  xmlMarkup += "<name>wp_slug</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->slug() + "]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_excerpt</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->summary() + "]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_keywords</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->tags().join(",") + "]]></string></value>";
  xmlMarkup += "</member></struct></value></param>";
  xmlMarkup += "<param><value><boolean>" +
               QString( "%1" ).arg( (int)(!post->isPrivate() ) ) +
               "</boolean></value></param>";
  xmlMarkup += "</params></methodCall>";

  QByteArray postData;
  QDataStream stream( &postData, QIODevice::WriteOnly );
  stream.writeRawData( xmlMarkup.toUtf8(), xmlMarkup.toUtf8().length() );

  KIO::TransferJob *job = KIO::http_post( url(), postData, KIO::HideProgressInfo );

  d->mCreatePostMap[ job ] = post;

  if ( !job ) {
    kWarning() << "Failed to create job for: " << url().url();
  }

  job->addMetaData(
    "customHTTPHeader", "X-hacker: Shame on you Wordpress, " + QString() +
    "you took another 4 hours of my life to work around the stupid dateTime bug." );
  job->addMetaData( "content-type", "Content-Type: text/xml; charset=utf-8" );
  job->addMetaData( "ConnectTimeout", "50" );
  job->addMetaData( "UserAgent", userAgent() );

  connect( job, SIGNAL(data(KIO::Job *,const QByteArray &)),
           this, SLOT(slotCreatePostData(KIO::Job *,const QByteArray &)) );
  connect( job, SIGNAL(result(KJob *)),
           this, SLOT(slotCreatePost(KJob *)) );
}

void WordpressBuggy::modifyPost( KBlog::BlogPost *post )
{
  kDebug();
  Q_D( WordpressBuggy );
  if ( !post ) {
    kError() << "WordpressBuggy::modifyPost: post is a null pointer";
    emit error ( Other, i18n( "Post is a null pointer." ) );
    return;
  }

  kDebug() << "Uploading Post with postId" << post->postId();

  QString xmlMarkup = "<?xml version=\"1.0\"?>";
  xmlMarkup += "<methodCall>";
  xmlMarkup += "<methodName>metaWeblog.editPost</methodName>";
  xmlMarkup += "<params><param>";
  xmlMarkup += "<value><string><![CDATA["+post->postId()+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param>";
  xmlMarkup += "<value><string><![CDATA["+username()+"]]></string></value>";
  xmlMarkup += "</param><param>";
  xmlMarkup += "<value><string><![CDATA["+password()+"]]></string></value>";
  xmlMarkup += "</param>";
  xmlMarkup += "<param><value><struct>";
  xmlMarkup += "<member><name>description</name>";
  xmlMarkup += "<value><string><![CDATA["+post->content()+"]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>title</name>";
  xmlMarkup += "<value><string><![CDATA["+post->title()+"]]></string></value>";
  xmlMarkup += "</member><member>";

  QList<QString> catList = post->categories();
  if ( !catList.empty() ){
    xmlMarkup += "<name>categories</name>";
    xmlMarkup += "<value><array><data>";
    QList<QString>::ConstIterator it = catList.constBegin();
    QList<QString>::ConstIterator end = catList.constEnd();
    for ( ; it != end; ++it ){
      xmlMarkup += "<value><string><![CDATA[" + ( *it ) + "]]></string></value>";
    }
    xmlMarkup += "</data></array></value>";
    xmlMarkup += "</member><member>";
  }

  xmlMarkup += "<name>lastModified</name>";
  xmlMarkup += "<value><dateTime.iso8601>" +
               post->modificationDateTime().dateTime().toUTC().toString( "yyyyMMddThh:mm:ssZ" ) +
               "</dateTime.iso8601></value>";
  xmlMarkup += "</member><member>";

  xmlMarkup += "<name>dateCreated</name>";
  xmlMarkup += "<value><dateTime.iso8601>" +
              post->creationDateTime().dateTime().toUTC().toString( "yyyyMMddThh:mm:ssZ" ) +
              "</dateTime.iso8601></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_allow_comments</name>";
  xmlMarkup += QString( "<value><int>%1</int></value>" ).arg( (int)post->isCommentAllowed() );
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_allow_pings</name>";
  xmlMarkup += QString( "<value><int>%1</int></value>" ).arg( (int)post->isTrackBackAllowed() );
  xmlMarkup += "</member><member>";
  if( !post->additionalContent().isEmpty() ) {
      xmlMarkup += "<name>mt_text_more</name>";
      xmlMarkup += "<value><string><![CDATA[" + post->additionalContent() + "]]></string></value>";
      xmlMarkup += "</member><member>";
  }
  xmlMarkup += "<name>wp_slug</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->slug() + "]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_excerpt</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->summary() + "]]></string></value>";
  xmlMarkup += "</member><member>";
  xmlMarkup += "<name>mt_keywords</name>";
  xmlMarkup += "<value><string><![CDATA[" + post->tags().join( "," ) + "]]></string></value>";
  xmlMarkup += "</member></struct></value></param>";
  xmlMarkup += "<param><value><boolean>" +
               QString( "%1" ).arg( (int)( !post->isPrivate() ) ) +
               "</boolean></value></param>";
  xmlMarkup += "</params></methodCall>";

  QByteArray postData;
  QDataStream stream( &postData, QIODevice::WriteOnly );
  stream.writeRawData( xmlMarkup.toUtf8(), xmlMarkup.toUtf8().length() );

  KIO::TransferJob *job = KIO::http_post( url(), postData, KIO::HideProgressInfo );

  d->mModifyPostMap[ job ] = post;

  if ( !job ) {
    kWarning() << "Failed to create job for: " << url().url();
  }

  job->addMetaData(
    "customHTTPHeader", "X-hacker: Shame on you Wordpress, " + QString() +
    "you took another 4 hours of my life to work around the stupid dateTime bug." );
  job->addMetaData( "content-type", "Content-Type: text/xml; charset=utf-8" );
  job->addMetaData( "ConnectTimeout", "50" );
  job->addMetaData( "UserAgent", userAgent() );

  connect( job, SIGNAL(data(KIO::Job*,const QByteArray&)),
           this, SLOT(slotModifyPostData(KIO::Job*,const QByteArray&)) );
  connect( job, SIGNAL(result(KJob*)),
           this, SLOT(slotModifyPost(KJob*)) );
}

void WordpressBuggy::createCategory( const QMap<QString, QVariant> &category )
{
  Q_D( WordpressBuggy );
  if ( category.value( "name" ).toString().isEmpty()) {
    kError() << "category name is not sets";
    emit error ( Other, i18n( "Category name isn't set." ) );
    return;
  }
  unsigned int i = d->mCallCounter++;
  d->mCreateCategoryMap[ i ] = category;
  kDebug() << "name="<< category.value( "name" ).toString();
  QList<QVariant> args( d->defaultArgs( blogId() ) );
  args << category;
  d->mXmlRpcClient->call(
    "wp.newCategory", args,
    this, SLOT(slotCreateCategory(const QList<QVariant>&,const QVariant&)),
    this, SLOT(slotCategoryError(int,const QString&,const QVariant&)),
    QVariant( i ) );
}

QString WordpressBuggy::interfaceName() const
{
  return QLatin1String( "Movable Type" );
}

WordpressBuggyPrivate::WordpressBuggyPrivate()
{
}

WordpressBuggyPrivate::~WordpressBuggyPrivate()
{
  kDebug();
}

QList<QVariant> WordpressBuggyPrivate::defaultArgs( const QString &id )
{
  Q_Q( WordpressBuggy );
  QList<QVariant> args;
  if ( !id.isEmpty() ) {
    args << QVariant( id );
  }
  args << QVariant( q->username() )
       << QVariant( q->password() );
  return args;
}

void WordpressBuggyPrivate::slotCreatePostData( KIO::Job *job, const QByteArray &data )
{
  kDebug();
  mCreatePostBuffer[ job ].append(data);
}

void WordpressBuggyPrivate::slotCreatePost( KJob *job )
{
  kDebug();
  const QString data = QString::fromUtf8( mCreatePostBuffer[ job ].data(),
                                          mCreatePostBuffer[ job ].size() );
  mCreatePostBuffer[ job ].resize( 0 );

  Q_Q( WordpressBuggy );

  KBlog::BlogPost *post = mCreatePostMap[ job ];
  mCreatePostMap.remove( job );

  if ( job->error() != 0 ) {
    kError() << "slotCreatePost error:" << job->errorString();
    emit q->errorPost( WordpressBuggy::Atom, job->errorString(), post );
    return;
  }

  QRegExp rxError( "faultString" );
  if ( rxError.indexIn( data ) != -1 ){
    rxError = QRegExp( "<string>(.+)</string>" );
    if ( rxError.indexIn( data ) != -1 ) {
      kDebug() << "RegExp of faultString failed.";
    }
    kDebug() << rxError.cap(1);
    emit q->errorPost( WordpressBuggy::XmlRpc, rxError.cap(1), post );
    return;
  }

  QRegExp rxId( "<string>(.+)</string>" );
  if ( rxId.indexIn( data ) == -1 ){
    kError() << "Could not regexp the id out of the result:" << data;
    emit q->errorPost( WordpressBuggy::XmlRpc,
                       i18n( "Could not regexp the id out of the result." ), post );
    return;
  }
  kDebug() << "QRegExp rx( \"<string>(.+)</string>\" ) matches" << rxId.cap( 1 );

  post->setPostId( rxId.cap( 1 ) );
  post->setStatus( BlogPost::Created );
  kDebug() << "Emitting createdPost()";
  emit q->createdPost( post );
}

void WordpressBuggyPrivate::slotModifyPostData( KIO::Job *job, const QByteArray &data )
{
  kDebug();
  mModifyPostBuffer[ job ].append(data);
}

void WordpressBuggyPrivate::slotModifyPost( KJob *job )
{
  kDebug();
  const QString data = QString::fromUtf8( mModifyPostBuffer[ job ].data(),
                                          mModifyPostBuffer[ job ].size() );
  mModifyPostBuffer[ job ].resize( 0 );

  KBlog::BlogPost *post = mModifyPostMap[ job ];
  mModifyPostMap.remove( job );
  Q_Q( WordpressBuggy );
  if ( job->error() != 0 ) {
    kError() << "slotModifyPost error:" << job->errorString();
    emit q->errorPost( WordpressBuggy::Atom, job->errorString(), post );
    return;
  }

  QRegExp rxError( "faultString" );
  if ( rxError.indexIn( data ) != -1 ){
    rxError = QRegExp( "<string>(.+)</string>" );
    if ( rxError.indexIn( data ) != -1 ) {
      kDebug() << "RegExp of faultString failed.";
    }
    kDebug() << rxError.cap(1);
    emit q->errorPost( WordpressBuggy::XmlRpc, rxError.cap(1), post );
    return;
  }

  QRegExp rxId( "<boolean>(.+)</boolean>" );
  if ( rxId.indexIn( data ) == -1 ) {
    kError() << "Could not regexp the id out of the result:" << data;
    emit q->errorPost( WordpressBuggy::XmlRpc,
                       i18n( "Could not regexp the id out of the result." ), post );
    return;
  }
  kDebug() << "QRegExp rx( \"<boolean>(.+)</boolean>\" ) matches" << rxId.cap( 1 );

  if ( rxId.cap( 1 ).toInt() == 1 ) {
    kDebug() << "Post successfully updated.";
    post->setStatus( BlogPost::Modified );
    emit q->modifiedPost( post );
  }
}

void WordpressBuggyPrivate::slotCategoryError( int number, const QString &errorString, const QVariant &id )
{
  Q_Q(WordpressBuggy  );
  Q_UNUSED( number );
  mCreateCategoryMap.remove( id.toInt() );

  emit q->error( Blogger1::XmlRpc, errorString);
}

void WordpressBuggyPrivate::slotCreateCategory(const QList<QVariant>& result, const QVariant& id)
{
  kDebug()<<result;
  kDebug()<<result.at(0).typeName();
}

#include "wordpressbuggy.moc"
