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

#include <QtGui>
#include <QImage>
#include <QTextCharFormat>
#include <QWebView>
#include <klocalizedstring.h>
#include <ktoolbar.h>
#include <kaction.h>
#include <kactioncollection.h>
#include <kicon.h>
#include <kcolordialog.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include "kseparator.h"

#include "bilboeditor.h"
#include "dbman.h"
#include "multilinetextedit.h"
#include "addeditlink.h"
#include "addimagedialog.h"
#include "bilbomedia.h"
#include "global.h"
#include "bilboblog.h"
#include "bilbopost.h"
#include "medialistwidget.h"
#include "bilbotextformat.h"
#include "bilbotexthtmlimporter.h"

#include "htmlexporter.h"
#include "stylegetter.h"

BilboEditor::BilboEditor( QWidget *parent )
        : KTabWidget( parent )
{
    createUi();
    connect(editor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    connect(htmlEditor, SIGNAL(textChanged()), this, SIGNAL(textChanged()));
    editor->setFocus();
}


BilboEditor::~BilboEditor()
{
}

void BilboEditor::createUi()
{
    ///this:
    this->resize( 600, 400 );
    tabVisual = new QWidget( this );
    tabHtml = new QWidget( this );
    tabPreview = new QWidget( this );
    this->addTab( tabVisual, i18nc( "Software", "Visual Editor" ) );
    this->addTab( tabHtml, i18nc( "Software", "Html Editor" ) );
    this->addTab( tabPreview, i18nc( "preview of the edited post", "Post Preview" ) );
    connect( this, SIGNAL( currentChanged( int ) ), this, SLOT( sltSyncEditors( int ) ) );
    prev_index = 0;

    ///editor:
    editor = new MultiLineTextEdit( tabVisual );
    connect( editor, SIGNAL( sigRemoteImageArrived( const KUrl ) ), this, 
             SLOT( sltReloadImage( const KUrl ) ) );
    connect( editor, SIGNAL( sigMediaTypeFound( BilboMedia* ) ), this, 
             SLOT( sltMediaTypeFound( BilboMedia* ) ) );

    barVisual = new KToolBar( tabVisual );
    barVisual->setIconSize( QSize( 22, 22 ) );
    barVisual->setToolButtonStyle( Qt::ToolButtonIconOnly );

    QLabel *label = new QLabel( i18n( "Post media:" ), tabVisual );
    label->setMaximumHeight( 30 );

    lstMediaFiles = new MediaListWidget( tabVisual );
    lstMediaFiles->setViewMode( QListView::IconMode );
    lstMediaFiles->setIconSize( QSize( 32, 32 ) );
    lstMediaFiles->setGridSize( QSize( 60, 48 ) );
    lstMediaFiles->setDragDropMode( QAbstractItemView::NoDragDrop );
    lstMediaFiles->setResizeMode( QListView::Adjust );
    lstMediaFiles->setMaximumHeight( 60 );
    connect( lstMediaFiles, SIGNAL( sigSetProperties( const int, const int,
                                    const int, const QString, const QString, const QString ) ), 
            this, SLOT( sltSetImageProperties( const int, const int, const int, 
                        const QString, const QString, const QString ) ) );
    connect( lstMediaFiles, SIGNAL( sigRemoveMedia( const int ) ), this, SLOT( sltRemoveMedia( const int ) ) );

    kDebug() << lstMediaFiles->iconSize() << "icon size";

    QVBoxLayout *vLayout = new QVBoxLayout( tabVisual );
    vLayout->addWidget( barVisual );
    vLayout->addWidget( editor );
    vLayout->addWidget( label );
    vLayout->addWidget( lstMediaFiles );

    connect( editor, SIGNAL( checkSpellingChanged( bool ) ), this, SLOT( sltSyncSpellCheckingButton( bool ) ) );

    connect( editor, SIGNAL( cursorPositionChanged() ), this, SLOT( sltSyncToolbar() ) );

    ///htmlEditor:
    htmlEditor = new QPlainTextEdit( tabHtml );
    QGridLayout *hLayout = new QGridLayout( tabHtml );
    hLayout->addWidget( htmlEditor );

    ///preview:
    preview = new QWebView( tabPreview );
    btnGetStyle = new KPushButton ( tabPreview );
    btnGetStyle->setText( i18n( "Get blog style" ) );
    connect( btnGetStyle, SIGNAL( clicked( bool ) ), this, SLOT( sltGetBlogStyle() ) );

    QSpacerItem *horizontalSpacer = new QSpacerItem( 40, 20,
                    QSizePolicy::Expanding, QSizePolicy::Minimum );
    KSeparator *separator = new KSeparator( tabPreview );

    QVBoxLayout *pLayout1 = new QVBoxLayout( tabPreview );
    QHBoxLayout *pLayout2 = new QHBoxLayout();

    pLayout2->addItem( horizontalSpacer );
    pLayout2->addWidget( btnGetStyle );
    pLayout1->addLayout( pLayout2 );
    pLayout1->addWidget( separator );
    pLayout1->addWidget( preview );

    this->setCurrentIndex( 0 );

    ///defaultCharFormat
    defaultCharFormat = editor->currentCharFormat();

    const QFont defaultFont = editor->document()->defaultFont();
    defaultCharFormat.setFont( defaultFont );
    defaultCharFormat.setForeground( editor->currentCharFormat().foreground() );
    defaultCharFormat.setProperty( QTextFormat::FontSizeAdjustment, QVariant( 0 ) );

    ///defaultBlockFormat
    defaultBlockFormat = editor->textCursor().blockFormat();
    
    currentPostTitle = i18n( "Post Title" );
    createActions();
}

void BilboEditor::createActions()
{
    actCheckSpelling = new KAction( KIcon( "tools-check-spelling" ), i18n( "Enable Spell Checking"),
                           this );
    actCheckSpelling->setCheckable( true );
    connect( actCheckSpelling, SIGNAL( triggered( bool ) ), this, 
             SLOT( sltToggleSpellChecking() ) );
    barVisual->addAction( actCheckSpelling );

    barVisual->addSeparator();

    actBold = new KAction( KIcon( "format-text-bold" ), i18nc( "Makes text bold", "Bold" ),
                           this );
    actBold->setShortcut( Qt::CTRL + Qt::Key_B );
    actBold->setToolTip( i18nc( "Makes text bold, and its shortcut is (Ctrl+b)",
                                "Bold (Ctrl+b)" ) );
    actBold->setCheckable( true );
    connect( actBold, SIGNAL( triggered( bool ) ), editor, SLOT( setTextBold( bool ) ) );
    barVisual->addAction( actBold );

    actItalic = new KAction( KIcon( "format-text-italic" ), i18nc( "Makes text italic",
                             "Italic" ), this );
    actItalic->setShortcut( Qt::CTRL + Qt::Key_I );
    actItalic->setToolTip( i18nc( "Makes text italic, and its shortcut is (Ctrl+i)",
                                  "Italic (Ctrl+i)" ) );
    actItalic->setCheckable( true );
    connect( actItalic, SIGNAL( triggered( bool ) ), editor, SLOT( setTextItalic( bool ) ) );
    barVisual->addAction( actItalic );

    actUnderline = new KAction( KIcon( "format-text-underline" ), i18nc( "Makes text underlined", "Underline" ), this );
    actUnderline->setShortcut( Qt::CTRL + Qt::Key_U );
    actUnderline->setToolTip( i18nc( "Makes text underlined, and its shortcut is (Ctrl+u)",
                                     "Underline (Ctrl+u)" ) );
    actUnderline->setCheckable( true );
    connect( actUnderline, SIGNAL( triggered( bool ) ), editor, SLOT( setTextUnderline( bool ) ) );
    barVisual->addAction( actUnderline );

    actStrikeout = new KAction( KIcon( "format-text-strikethrough" ), i18nc( "Strikes the text out", "Strikeout" ), this );
    actStrikeout->setShortcut( Qt::CTRL + Qt::Key_L );
    actStrikeout->setToolTip( i18nc( "Strikes the text out, and its shortcut is (Ctrl+l)",
                                     "Underline (Ctrl+l)" ) );
    actStrikeout->setCheckable( true );
    connect( actStrikeout, SIGNAL( triggered( bool ) ), editor, SLOT( setTextStrikeOut( bool ) ) );
    barVisual->addAction( actStrikeout );

    actCode = new KAction( KIcon( "format-text-code" ), i18nc( "Sets text font to code style",
                           "Code" ), this );
    actCode->setCheckable( true );
    connect( actCode, SIGNAL( triggered( bool ) ), this, SLOT( sltToggleCode() ) );
    barVisual->addAction( actCode );

    barVisual->addSeparator();

    actFontIncrease = new KAction( KIcon( "format-font-size-more" ), i18n( "Increase font size" ), this );
    connect( actFontIncrease, SIGNAL( triggered( bool ) ), this, SLOT( sltFontSizeIncrease() ) );
    barVisual->addAction( actFontIncrease );

    actFontDecrease = new KAction( KIcon( "format-font-size-less" ), i18n( "Decrease font size" ), this );
    connect( actFontDecrease, SIGNAL( triggered( bool ) ), this, SLOT( sltFontSizeDecrease() ) );
    barVisual->addAction( actFontDecrease );

    actColorSelect = new KAction( KIcon( "format-text-color" ), i18nc( "verb, to select text color", "Select Color" ), this );
    connect( actColorSelect, SIGNAL( triggered( bool ) ), this, SLOT( sltSelectColor() ) );
    barVisual->addAction( actColorSelect );

    actRemoveFormatting = new KAction( KIcon( "draw-eraser" ), i18n( "Remove formatting" ), this );
    actRemoveFormatting->setShortcut( Qt::CTRL + Qt::Key_R );
    actRemoveFormatting->setToolTip( i18nc( "Remove formatting, and its shortcut is (Ctrl+r)",
                                            "Remove formatting (Ctrl+r)" ) );
    connect( actRemoveFormatting, SIGNAL( triggered( bool ) ), this, SLOT( sltRemoveFormatting() ) );
    barVisual->addAction( actRemoveFormatting );

    barVisual->addSeparator();

    actAlignLeft = new KAction( KIcon( "format-justify-left" ), i18nc( "verb, to align text from left", "Align left" ), this );
    connect( actAlignLeft, SIGNAL( triggered( bool ) ), this, SLOT( sltAlignLeft() ) );
    barVisual->addAction( actAlignLeft );

    actAlignCenter = new KAction( KIcon( "format-justify-center" ), i18nc( "verb, to align text from center", "Align center" ), this );
    connect( actAlignCenter, SIGNAL( triggered( bool ) ), editor, SLOT( alignCenter() ) );
    barVisual->addAction( actAlignCenter );

    actAlignRight = new KAction( KIcon( "format-justify-right" ), i18nc( "verb, to align text from right", "Align right" ), this );
    connect( actAlignRight, SIGNAL( triggered( bool ) ), this, SLOT( sltAlignRight() ) );
    barVisual->addAction( actAlignRight );

    actJustify = new KAction( KIcon( "format-justify-fill" ), i18nc( "verb, to justify text",
                              "Justify" ), this );
    connect( actJustify, SIGNAL( triggered( bool ) ), editor, SLOT( alignJustify() ) );
    barVisual->addAction( actJustify );

    actRightToLeft = new KAction( KIcon( "format-text-direction-rtl" ), i18nc( "Sets text direction to right to left", "Right to Left" ), this );
    actRightToLeft->setCheckable( true );
    connect( actRightToLeft, SIGNAL( triggered( bool ) ), this, SLOT( sltChangeLayoutDirection() ) );
    barVisual->addAction( actRightToLeft );

    barVisual->addSeparator();

    actAddLink = new KAction( KIcon( "insert-link" ), i18nc( "verb, to add a new link or edit an existing one", "Add/Edit Link" ), this );
    connect( actAddLink, SIGNAL( triggered( bool ) ), this, SLOT( sltAddEditLink() ) );
    barVisual->addAction( actAddLink );

    actRemoveLink = new KAction( KIcon( "remove-link" ), i18nc( "verb, to remove an existing link", "Remove Link" ), this );
    connect( actRemoveLink, SIGNAL( triggered( bool ) ), this, SLOT( sltRemoveLink() ) );
    barVisual->addAction( actRemoveLink );

    barVisual->addSeparator();

    actAddImage = new KAction( KIcon( "insert-image" ), i18nc( "verb, to insert an image",
                               "Add Image" ), this );
    connect( actAddImage, SIGNAL( triggered( bool ) ), this, SLOT( sltAddImage() ) );
    barVisual->addAction( actAddImage );
    
    actAddMedia = new KAction( KIcon( "mail-attachment" ), 
                i18nc( "verb, to add a media file to the post as an attachment", 
                "Attach Media" ), this );
    connect( actAddMedia, SIGNAL( triggered( bool ) ), this, SLOT( sltAddMedia() ) );
    barVisual->addAction( actAddMedia );

    barVisual->addSeparator();

    actOrderedList = new KAction( KIcon( "format-list-ordered" ), i18n( "Ordered List" ), this );
    connect( actOrderedList, SIGNAL( triggered( bool ) ), this, SLOT( sltAddOrderedList() ) );
    barVisual->addAction( actOrderedList );

    actUnorderedList = new KAction( KIcon( "format-list-unordered" ), i18n( "Unordered List" ), this );
    connect( actUnorderedList, SIGNAL( triggered( bool ) ), this, SLOT( sltAddUnorderedList() ) );
    barVisual->addAction( actUnorderedList );
    
    actSplitPost = new KAction( KIcon( "insert-more-mark" ), i18n( "Split text" ), this );
    connect( actSplitPost, SIGNAL( triggered( bool ) ), this, SLOT( sltAddPostSplitter() ) );
    barVisual->addAction( actSplitPost );
}

void BilboEditor::sltToggleSpellChecking()
{
    if ( actCheckSpelling->isChecked() ) {
        editor->setCheckSpellingEnabled( true );
    } else {
        editor->setCheckSpellingEnabled( false );
    }
}

void BilboEditor::sltSyncSpellCheckingButton( bool check )
{
    actCheckSpelling->setChecked( check );
}

void BilboEditor::sltToggleCode()
{
    //TODO
    static QString preFontFamily;
    if ( editor->fontFamily() != "Courier New,courier" ) {
        preFontFamily = editor->fontFamily();
        editor->setFontFamily( "Courier New,courier" );
    } else {
        editor->setFontFamily( preFontFamily );
    }
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltFontSizeIncrease()
{
    QTextCharFormat format;
    int idx = editor->currentCharFormat().intProperty( QTextFormat::FontSizeAdjustment );
    if ( idx < 3 ) {
        format.setProperty( QTextFormat::FontSizeAdjustment, QVariant( ++idx ) );
        editor->textCursor().mergeCharFormat( format );
    }
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltFontSizeDecrease()
{
    QTextCharFormat format;
    int idx = editor->currentCharFormat().intProperty( QTextFormat::FontSizeAdjustment );
    if ( idx > -1 ) {
        format.setProperty( QTextFormat::FontSizeAdjustment, QVariant( --idx ) );
        editor->textCursor().mergeCharFormat( format );
    }
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltAddEditLink()
{
    linkDialog = new AddEditLink( this );
    linkDialog->setAttribute( Qt::WA_DeleteOnClose );
    connect( linkDialog, SIGNAL( addLink( const QString&, const QString&, const QString& ) ),
             this, SLOT( sltSetLink( const QString&, const QString&, const QString& ) ) );

    QTextCharFormat f = editor->currentCharFormat();
    if ( !f.isAnchor() ) {
        linkDialog->show();
    } else {
        linkDialog->show( f.anchorHref(), f.stringProperty( BilboTextFormat::AnchorTitle )
                          , f.stringProperty( BilboTextFormat::AnchorTarget ) );
    }
}

void BilboEditor::sltSetLink( const QString& address, const QString& target,
                              const QString& title )
{
    QTextCharFormat f = editor->currentCharFormat();
    f.setAnchor( true );
    f.setAnchorHref( address );
    f.setProperty( BilboTextFormat::AnchorTitle, QVariant( title ) );
    f.setProperty( BilboTextFormat::AnchorTarget, QVariant( target ) );

    editor->textCursor().mergeCharFormat( f );
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltRemoveLink()
{
    QTextCharFormat f = editor->textCursor().charFormat();
    f.setAnchor( false );
    f.setUnderlineStyle( this->defaultCharFormat.underlineStyle() );
    f.setForeground( this->defaultCharFormat.foreground() );
    
    editor->textCursor().mergeCharFormat( f );
    editor->setFocus( Qt::MouseFocusReason );
}

void BilboEditor::sltSelectColor()
{
    QColor c;

    int result = KColorDialog::getColor( c, editor->textCursor().charFormat().foreground().color(), this );
    if ( result == KColorDialog::Accepted ) {
        editor->setTextForegroundColor( c );
    }
}

void BilboEditor::sltRemoveFormatting()
{
    editor->textCursor().mergeCharFormat( defaultCharFormat );
    editor->setFocus( Qt::OtherFocusReason );
}

// void BilboEditor::sltNewParagraph()
// {
//     editor->textCursor().insertBlock( editor->textCursor().blockFormat(), editor->textCursor().charFormat() );
//     editor->setFocus( Qt::OtherFocusReason );
// }
void BilboEditor::sltAlignRight()
{
    editor->setAlignment( Qt::AlignRight | Qt::AlignAbsolute );
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltAlignLeft()
{
    editor->setAlignment( Qt::AlignLeft | Qt::AlignAbsolute );
    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltChangeLayoutDirection()
{
    kDebug();
    
    QTextBlockFormat f = editor->textCursor().blockFormat();
    if ( f.layoutDirection() != Qt::RightToLeft ) {
        f.setLayoutDirection( Qt::RightToLeft );
    } else {
        f.setLayoutDirection( Qt::LeftToRight );
    }
    editor->textCursor().mergeBlockFormat( f );

    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltAddImage()
{
    AddImageDialog *imageDialog = new AddImageDialog( this );
//     imageDialog->setAttribute( Qt::WA_DeleteOnClose );
    
    connect( imageDialog, SIGNAL( sigAddImage( BilboMedia *, const int, const int, 
             const QString, const QString, const QString ) ), this, SLOT( sltSetImage( BilboMedia *, 
             const int, const int, const QString, const QString, const QString ) ) );
    connect( imageDialog, SIGNAL( sigMediaTypeFound( BilboMedia * ) ), this, 
             SLOT( sltMediaTypeFound( BilboMedia * ) ) );
    imageDialog->exec();
}

void BilboEditor::sltSetImage( BilboMedia *media, const int width, const int height, 
                        const QString title, const QString link, const QString Alt_text )
{
    kDebug();

            QTextImageFormat imageFormat;

            imageFormat.setName( media->remoteUrl().url() );
            if ( width != 0 ) {
                imageFormat.setWidth( width );
            }
            if ( height != 0 ) {;
                imageFormat.setHeight( height );
            }
            if ( !title.isEmpty() ) {
                imageFormat.setProperty( BilboTextFormat::ImageTitle, QVariant( title ) );
            }
            if ( !Alt_text.isEmpty() ) {
                imageFormat.setProperty( BilboTextFormat::ImageAlternateText, QVariant( Alt_text ) );
            }
            if ( !link.isEmpty() ) {
                imageFormat.setAnchor( true );
                imageFormat.setAnchorHref( link );
            }
            editor->textCursor().insertImage( imageFormat );

            editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltReloadImage( const KUrl imagePath )
{
    QString path = imagePath.url();

    if ( this->currentIndex() == 0 ) {

        this->editor->setFocus( Qt::OtherFocusReason );
        QTextCharFormat f;
        QTextCursor cursor = this->editor->textCursor();
        QTextBlock block = this->editor->document()->firstBlock();
        QTextBlock::iterator i;
        do {
            for ( i = block.begin(); !( i.atEnd() ); ++i ) {
                kDebug() << "start iterating";
                f = i.fragment().charFormat();
                if ( f.isImageFormat() ) {
                    kDebug() << "is image format";
                    QTextImageFormat imgFormat = f.toImageFormat();
                    if ( imgFormat.name() == path ) {
                        kDebug() << "image exists";
                        imgFormat.setName( path );

                        cursor.setPosition( i.fragment().position() );
                        cursor.movePosition( QTextCursor::NextCharacter,
                                            QTextCursor::KeepAnchor, i.fragment().length() );
                        if ( cursor.hasSelection() ) {
                            kDebug() << " mine hasSelection";
                            cursor.mergeCharFormat( imgFormat );
                        }
                    }
                }
            }
            block = block.next();
        } while ( block.isValid() );
    }
    if ( mMediaList->contains( path ) ) {
        QList < QListWidgetItem* > list;
        list = lstMediaFiles->findItems( imagePath.fileName(), ( Qt::MatchFixedString | 
                Qt::MatchCaseSensitive ) );
        if ( list.isEmpty() ) {
            kDebug() << "image isn't inserted";
        } else {
            for ( int i = 0; i < list.size(); i++ ) {
                if ( list.at( i )->toolTip() == path ) {
                    list.at( i )->setIcon( mMediaList->value( path )->icon() );
                    break;
                }
            }
        }
    }
}

///FIXME because AddMediaDialog gets files from remote server asyncronously, we can't delete the dialog when it's closed. try to find a good time for deleting it.

void BilboEditor::sltAddMedia()
{
    AddMediaDialog *mediaDialog = new AddMediaDialog( this );
//     mediaDialog->setAttribute( Qt::WA_DeleteOnClose );
    
    connect( mediaDialog, SIGNAL( sigAddMedia( BilboMedia * ) ), this, SLOT( sltSetMedia( BilboMedia * ) ) );
    connect( mediaDialog, SIGNAL( sigMediaTypeFound( BilboMedia * ) ), this, 
             SLOT( sltMediaTypeFound( BilboMedia * ) ) );
    mediaDialog->exec();
}

void BilboEditor::sltSetMedia( BilboMedia *media )
{
    QTextCharFormat f;
    f.setAnchor( true );
            f.setAnchorHref( media->remoteUrl().url() );
    editor->textCursor().insertText( media->name(), f );

    editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltSetImageProperties( const int index, const int width,
                    const int height, const QString title, const QString link,
                    const QString Alt_text )
{
    this->editor->setFocus( Qt::OtherFocusReason );
    QString path = lstMediaFiles->item( index )->toolTip();

    QTextCharFormat f;
    QTextCursor cursor;
    QTextBlock block = this->editor->document()->firstBlock();
    QTextBlock::iterator i;
    do {
        for ( i = block.begin(); !( i.atEnd() ); ++i ) {
            kDebug() << "start iterating";
            f = i.fragment().charFormat();
            if ( f.isImageFormat() ) {
                kDebug() << "is image format";
                QTextImageFormat imgFormat = f.toImageFormat();
                if ( imgFormat.name() == path ) {
                    kDebug() << "image exists";
                    imgFormat.setName(path);
                    if ( width != 0 ) {
                        imgFormat.setWidth( width );
                    }
                    if ( height != 0 ) {
                        imgFormat.setHeight( height );
                    }
                    if ( !title.isEmpty() ) {
                        imgFormat.setProperty( BilboTextFormat::ImageTitle, QVariant( title ) );
                    }
                    if ( !Alt_text.isEmpty() ) {
                        imgFormat.setProperty( BilboTextFormat::ImageAlternateText, QVariant( Alt_text ) );
                    }
                    if ( !link.isEmpty() ) {
                        imgFormat.setAnchor( true );
                        imgFormat.setAnchorHref( link );
                    }
                    cursor = this->editor->textCursor();
                    cursor.setPosition( i.fragment().position() );
                    cursor.movePosition( QTextCursor::NextCharacter,
                                         QTextCursor::KeepAnchor, i.fragment().length() );
                    if ( cursor.hasSelection() ) {
                        kDebug() << " mine hasSelection";
                        cursor.mergeCharFormat( imgFormat );
                    }
                }
            }
        }
        block = block.next();
    } while ( block.isValid() );
    this->editor->setFocus( Qt::OtherFocusReason );
}

void BilboEditor::sltRemoveMedia( const int index )
{
    this->editor->setFocus( Qt::OtherFocusReason );
    QString path = lstMediaFiles->item( index )->toolTip();
    delete lstMediaFiles->item( index );

    kDebug() << path;

    int count = mMediaList->remove( path );
    kDebug() << count;

    QTextCharFormat f;
    QTextCursor cursor;
    QTextBlock block = this->editor->document()->firstBlock();
    QTextBlock::iterator i;
    do {
        i = block.begin();
        while ( !i.atEnd() ) {
            kDebug() << "start iterating";
            f = i.fragment().charFormat();
            if ( ( f.isImageFormat() && f.toImageFormat().name() == path ) ||
                  ( f.isAnchor() && f.anchorHref() == path ) )
            {
                kDebug() << "found";

                cursor = this->editor->textCursor();
                cursor.setPosition( i.fragment().position() );
                cursor.movePosition( QTextCursor::NextCharacter,
                                     QTextCursor::KeepAnchor, i.fragment().length() );
                ++i;
                if (i.atEnd()) {
                    cursor.removeSelectedText();
                    kDebug() << "removed";
                    break;
                } else {
                    cursor.removeSelectedText();
                    kDebug() << "removed";
                    i = block.begin();
                }
            }
            else {
                ++i;
            }
        }
        kDebug() << "to go next";
        block = block.next();
        kDebug() << "at next";
        
    } while ( block.isValid() );
}

void BilboEditor::sltMediaTypeFound( BilboMedia * media )
{
    QListWidgetItem *item;
    QString url = media->remoteUrl().url();
    
//     AddMediaDialog *dialog;
//     dialog = qobject_cast<AddMediaDialog*>( sender() );
//     if ( dialog ) {
//         delete dialog;
//     }
    if ( mMediaList->contains( url ) ) {
        //media is already added.
        delete media;
    } else {
        mMediaList->insert( url, media );

        if ( media->mimeType().contains( "image" ) ) {
            item = new QListWidgetItem( media->icon(), media->name(), lstMediaFiles, MediaListWidget::ImageType );
        } else {
            item = new QListWidgetItem( media->icon(), media->name(), lstMediaFiles, MediaListWidget::OtherType );
        }
//         item->setData( Qt::UserRole, QVariant( url ) );
        item->setToolTip( url );
        item->setSizeHint( lstMediaFiles->gridSize() );
    }
}

void BilboEditor::sltAddOrderedList()
{
//  if (editor->textCursor().currentList() == 0) {
    editor->textCursor().createList( QTextListFormat::ListDecimal );
//  } else {
//   QTextListFormat lf = editor->textCursor().currentList()->format();
// //   QTextBlockFormat bf = editor->textCursor().block().blockFormat();
// //   bf.setIndent(lf.indent() - 1);
// //   editor->textCursor().mergeBlockFormat(bf);
//   editor->textCursor().currentList()->remove(editor->textCursor().block());
//  }
}

void BilboEditor::sltAddUnorderedList()
{
//  if (editor->textCursor().currentList() == 0) {
    editor->textCursor().createList( QTextListFormat::ListDisc );
//  } else {
//   QTextListFormat lf = editor->textCursor().currentList()->format();
// //   QTextBlockFormat bf = editor->textCursor().block().blockFormat();
// //   bf.setIndent(lf.indent() - 1);
// //   editor->textCursor().mergeBlockFormat(bf);
//   editor->textCursor().currentList()->remove(editor->textCursor().block());
//  }
}

void BilboEditor::sltAddPostSplitter()
{
    QTextBlockFormat f = editor->textCursor().blockFormat();
    QTextBlockFormat f1 = f;

    f.setProperty( BilboTextFormat::IsHtmlTagSign, true );
    f.setProperty( QTextFormat::BlockTrailingHorizontalRulerWidth, 
             QTextLength( QTextLength::PercentageLength, 80 ) );
    if ( editor->textCursor().block().text().isEmpty() ) {
        editor->textCursor().mergeBlockFormat( f );
    } else {
        editor->textCursor().insertBlock( f );
    }
    editor->textCursor().insertBlock( f1 );
}

void BilboEditor::sltSyncToolbar()
{
    if ( this->editor->textCursor().charFormat() != lastCharFormat ) {
        lastCharFormat = this->editor->textCursor().charFormat();
        
        if ( lastCharFormat.fontWeight() == QFont::Bold ) {
            this->actBold->setChecked( true );
        } else {
            this->actBold->setChecked( false );
        }
        this->actItalic->setChecked( lastCharFormat.fontItalic() );
        this->actUnderline->setChecked( lastCharFormat.fontUnderline() );
        this->actStrikeout->setChecked( lastCharFormat.fontStrikeOut() );
        if ( lastCharFormat.fontFamily() == QString::fromLatin1( "Courier New,courier" ) ) {
            this->actCode->setChecked( true );
        } else {
            this->actCode->setChecked( false );
        }
    }
    if ( this->editor->textCursor().blockFormat() != lastBlockFormat ) {
        lastBlockFormat = this->editor->textCursor().blockFormat();
        
        if ( lastBlockFormat.layoutDirection() == Qt::RightToLeft ) {
            this->actRightToLeft->setChecked( true );
        } else {
            this->actRightToLeft->setChecked( false );
        }
    }
}

void BilboEditor::sltSyncEditors( int index )
{
    kDebug();
    QTextDocument *doc = editor->document();

    htmlExporter* htmlExp = new htmlExporter();
    htmlExp->setDefaultCharFormat( this->defaultCharFormat );
    htmlExp->setDefaultBlockFormat( this->defaultBlockFormat );

    if ( index == 0 ) {
        doc->clear();
        BilboTextHtmlImporter( doc, htmlEditor->toPlainText() ).import();
        editor->setTextCursor( QTextCursor( doc ) );

    } else if ( index == 1 ) {
        htmlEditor->setPlainText( htmlExp->toHtml( doc ) );

    } else {
        if ( prev_index == 1 ) {
            doc->clear();
            BilboTextHtmlImporter( doc, htmlEditor->toPlainText() ).import();
        } else {
            htmlEditor->setPlainText( htmlExp->toHtml( doc ) );
        }
        QString baseU = "http://bilbo.ospdev.net";

        if ( __currentBlogId > -1 ) {
            baseU = DBMan::self()->getBlogInfo( __currentBlogId ).blogUrl();
        }
        preview->setHtml( htmlEditor->toPlainText(), QUrl( baseU ) );

        this->preview->setHtml( StyleGetter::styledHtml( __currentBlogId, 
                         currentPostTitle,
                         this->htmlEditor->toPlainText() ), QUrl( baseU ) );
    }

    prev_index = index;
    delete htmlExp;
}

QString BilboEditor::htmlToRichtext( const QString& html )
{
    QString richText = html;

    richText.remove( QChar( '\n' ) );

    richText.replace( QRegExp( "<del>(.*)</del>" ), "<s>\\1</s>" );

    QString h;
    h = "<html><head></head><body><p>" + richText + "</p></body></html>";
    return h;
}

QString BilboEditor::htmlContent()
{
    kDebug();

    if ( this->currentIndex() == 0 ) {

        htmlExporter* htmlExp = new htmlExporter();
        htmlExp->setDefaultCharFormat( this->defaultCharFormat );
        htmlExp->setDefaultBlockFormat( this->defaultBlockFormat );

        htmlEditor->setPlainText( htmlExp->toHtml( editor->document() ) );
        kDebug() << "setting Plain text done";

        delete htmlExp;
    }

    QString htmlContent = htmlEditor->toPlainText();
    return htmlContent;
}

void BilboEditor::setHtmlContent( const QString & content )
{
    QTextDocument *doc = editor->document();
    doc->clear();

    if ( !content.isEmpty() ) {
        BilboTextHtmlImporter( doc, content ).import();
    }
    this->editor->setTextCursor( QTextCursor( doc ) );
    this->htmlEditor->setPlainText( content );
}

void BilboEditor::setMediaList( QMap <QString, BilboMedia*> * list )
{
    mMediaList = list;
    editor->setMediaList( list );
}

void BilboEditor::setLayoutDirection( Qt::LayoutDirection direction )
{
    QTextBlockFormat f = editor->textCursor().blockFormat();
    f.setLayoutDirection( direction );
    editor->textCursor().mergeBlockFormat( f );

    if ( direction == Qt::LeftToRight ) {
        this->actRightToLeft->setChecked( false );
    } else {
        this->actRightToLeft->setChecked( true );
    }
}

void BilboEditor::setCurrentTitle( const QString& title)
{
    if ( title.isEmpty() ) {
        currentPostTitle = i18n( "Post Title" );
    } else {
        currentPostTitle = title;
    }
}

bool BilboEditor::updateMediaPaths()
{
    int startIndex = 0;
    int endIndex;
    QString path;
    QString htmlContent;
    bool changed = false;

    if ( this->currentIndex() == 0 ) {
        htmlExporter* htmlExp = new htmlExporter();
        htmlContent = htmlExp->toHtml( this->editor->document() );
    } else {
        htmlContent = this->htmlEditor->toPlainText();
    }

    startIndex = htmlContent.indexOf( QRegExp( "<([^<>]*)\"file://" ), startIndex );
    while ( startIndex != -1 ) {
        startIndex = htmlContent.indexOf( "file://", startIndex );
        endIndex = htmlContent.indexOf( '\"', startIndex );
        path = htmlContent.mid( startIndex, ( endIndex - startIndex ) );
        kDebug() << path << "is found";

        if ( mMediaList->contains( path ) ) {
//    if (mMediaList->value(path)->isUploaded()) {
            BilboMedia *media = mMediaList->value( path );

            htmlContent.replace( startIndex, ( endIndex - startIndex ),
                                 media->remoteUrl().url() );
            changed = true;

            mMediaList->remove( path );
            mMediaList->insert( media->remoteUrl().url(), media );

            QList < QListWidgetItem* > list;
            list = lstMediaFiles->findItems( media->name(), ( Qt::MatchFixedString | 
                Qt::MatchCaseSensitive ) );
            if ( list.isEmpty() ) {
                kDebug() << "media isn't inserted in list widget";
            } else {
                for ( int i = 0; i < list.size(); i++ ) {
                    if ( list.at( i )->toolTip() == path ) {
                        list.at( i )->setToolTip( media->remoteUrl().url() );
                        break;
                    }
                }
            }
        }
        startIndex = htmlContent.indexOf( QRegExp( "<([^<>]*)\"file://" ), endIndex );
    }
    kDebug() << htmlContent << "update result";
    if ( changed ) {
        kDebug() << "change is true";
        if ( this->currentIndex() == 0 ) {
            QTextDocument *doc = editor->document();
            doc->clear();
            BilboTextHtmlImporter( doc, htmlContent ).import();
        } else {
            this->htmlEditor->setPlainText( htmlContent );
        }
    }
    return true;
}

void BilboEditor::sltGetBlogStyle()
{
    int blogid = __currentBlogId;
    if ( blogid < 0 ) {
        KMessageBox::information( this,
               i18n( "Please select a blog from the toolbox, then try again." ), 
               i18n( "Select a blog" ) );
    }

    Q_EMIT sigShowStatusMessage( i18n( "Fetching blog style from the web..." ), true );
    Q_EMIT sigBusy( true );

    StyleGetter *styleGetter = new StyleGetter( __currentBlogId, this );
    connect( styleGetter, SIGNAL( sigStyleFetched() ), this, SLOT( sltSetPostPreview() ) );
}

void BilboEditor::sltSetPostPreview()
{
    Q_EMIT sigShowStatusMessage( i18n( "The requested blog style fetched." ), false );
    Q_EMIT sigBusy( false );

    if ( this->currentIndex() == 2 ) {
        Q_EMIT sigShowStatusMessage( i18n( "Setting blog style..." ), true );
        this->preview->setHtml( StyleGetter::styledHtml( __currentBlogId, 
                         currentPostTitle,
                         this->htmlEditor->toPlainText() ) );
        Q_EMIT sigShowStatusMessage( i18n( "The requested blog style set." ), false );
    }
    if ( qobject_cast< StyleGetter* >( sender() ) ) {
        sender()->deleteLater();
    }
}

#include "bilboeditor.moc"
