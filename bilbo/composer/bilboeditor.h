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

#ifndef BILBOEDITOR_H
#define BILBOEDITOR_H

#include <QTextFormat>

#include <ktabwidget.h>

class QTextCharFormat;
// class QProgressBar;
// class QWebView;
// class QPlainTextEdit;
namespace KTextEditor
{ 
    class View;
}

class KAction;
class KToolBar;
class KListWidget;
// class KPushButton;
class KSelectAction;
// class KStatusBar;

class MultiLineTextEdit;
class AddEditLink;
class BilboMedia;
class BilboPost;
class BilboBrowser;


/**
* Class BilboEditor represents the editor part of BilboBlogger
* @author Mehrdad Momeny <mehrdad.momeny@gmail.com>
* @author Golnaz Nilieh <g382nilieh@gmail.com>
*/

class BilboEditor : public KTabWidget
{
    Q_OBJECT
public:

    /**
     * @brief BilboEditor constructor.
     * Creates a new instance of BilboEditor, and calls createUi() function to 
     * initialize its widgets.
     */
    BilboEditor( QWidget *parent = 0 );

    /**
     * @brief BilboEditor destructor.
     */
    ~BilboEditor();

    /**
     * @brief Returns the editor current text in html format
     * Synchronizes HtmlEditor and editor tabs, by sending content of the current one to another.
     * then copies the content of HtmlEditor into the variable mHtmlContent, and returns it.
     * @return an String which contains html text
     */
//   const QString& htmlContent();
    QString htmlContent();

    /**
     * Sets the given string as the HtmlEditor and VisualEditor content.
     * @param content
     */
    void setHtmlContent( const QString &content );

//     QMap <QString, BilboMedia*> * mediaList();

    /**
     * Makes BilboEditor to use @param list as its list of media files.
     * then BilboEditor can add media files to it, or remove them.
     */
    void setMediaList( QMap <QString, BilboMedia*> *list );
//   void setMediaList(QMap <QString, BilboMedia*> & list);

    /**
     * Changes the layout direction of the editor, to the given direction.
     * @param direction is the new layout direction.
     */
    void setLayoutDirection( Qt::LayoutDirection direction );

    /**
     * Parses the content of htmlEditor tab, to replace media local paths with remote
     * addresses. It's used after uploading media files, for updating html content of the
     * post before being published.
     * @return true if successfull, and false otherwise.
     */
    bool updateMediaPaths();

    /**
     * Stores the given title, to be shown as the post title in preview tab.
     * @param title is the post title.
     */
    void setCurrentTitle( const QString& title );

Q_SIGNALS:
    /**
     * This signal is emitted when the content of VisualEditor or HtmlEditor changes.
     */
    void textChanged();

    /**
     * To show a message on statusBar
     * @param message Message to be shown
     * @param isPermanent If it's true the message will not have a timeout!
     *  so it will be shown until next message arrived
     */
    void sigShowStatusMessage( const QString& message, bool isPermanent );

    /**
     * This signal is emitted for operations in background, like request of some
     * data from the web.
     * @param isBusy if it's true, the operation is in progress. otherwise, it
     * is finished.
     */
    void sigBusy( bool isBusy );

protected Q_SLOTS:
    void slotSettingsChanged();

    void sltToggleSpellChecking();

    void sltSyncSpellCheckingButton( bool check );

    void sltSetTextBold( bool bold );

    /*!
    Changes the current text font into Courier fontfamily, to represent code style.
    or if text font already be in code style, uses the fontfamily it had before.
     */
    void sltToggleCode();

    /**
     * Changes the current paragraph format into Heading type or simple pargraph
     * type, according to the given parameter.
     * 
     * @param index is the index of the selected format type in the related
     * combobox in toolbar.
     */
    void sltChangeFormatType( const QString& text );
    /*!
    Increments font size by one degree.
    there are five predefined values for font size: "small", "medium", "large",
     * "x-large", "xx-large". Bilboeditor uses "medium" as default font size.
     */
    void sltFontSizeIncrease();

    /*!
    Decrements font size by one degree.
    there are five predefined values for font size: "small", "medium", "large",
     * "x-large", "xx-large". Bilboeditor uses "medium" as default font size.
     */
    void sltFontSizeDecrease();

    /*!
    Changes Alignment of current paragraph into Right Alignment.
     */
    void sltAlignRight();

    /*!
    Changes Alignment of current paragraph into Left Alignment.
     */
    void sltAlignLeft();

    /*!
    Switches Layout Direction of current paragraph between RightToLeft and LeftToRight Directions.
     */
    void sltChangeLayoutDirection();

    /*!
    Opens a dialog to set link address. if cursor has selection and it is a link
     * itself, the link address is shown in the dialog to edit.
     */
    void sltAddEditLink();

    /*!
    Sets the link address given in the link dialog as AnchorHref of the current text.
     * if link title is set in the link dialog, current text will change into that.
     */
    void sltSetLink( const QString& address, const QString& target, const QString& title );

    /*!
    Removes link from current text by assigning false to the Anchor property of text format.
     */
    void sltRemoveLink();

    /*!
    Changes the forground color of current text.
     */
    void sltSelectColor();

    /*!
    Changes all properties of current text format into default, except Anchor,
     * AnchorName and AnchorHref properties.
    \sa defaultCharFormat
     */
    void sltRemoveFormatting();

    /**
     * Creates an instance of AddImageDialog class,and opens it, to select an image.
     */
    void sltAddImage();

    void sltSetImage( BilboMedia *media, const int width, const int height, 
               const QString title, const QString link, const QString Alt_text );
    
    void sltReloadImage( const KUrl imagePath );

    void sltSetImageProperties( const int index, const int width, const int height,
                       const QString title, const QString link, const QString Alt_text );
    
    /**
     * Creates an instance of AddMediaDialog class,and opens it, to select a media file.
     */
//     void sltAddMedia();
    
    /**
     * Puts the given media object in the current cursor position, of the editor.
     * @param media is a BilboMedia object, which contains media path, mimetype and other needed information about it.
     */
//     void sltSetMedia( BilboMedia *media );
    
    void sltRemoveMedia( const int index );
    
    void sltMediaTypeFound( BilboMedia *media );

    void sltAddOrderedList();

    void sltAddUnorderedList();

    void sltToggleBlockQuote();

    void sltAddPostSplitter();

    void sltSyncToolbar();

    /*!
    Sets the content of the current tab  as other tabs' contents, to apply recent
     * changes. this function executes each time the user switches between tabs.
     */
    void sltSyncEditors( int index );

//     void sltGetBlogStyle();
// 
    void sltSetPostPreview();

private:

    /*!
    Creates actions of Bilbo editor, and assigns each one to a button, then adds each
     * button to barVisual, on the editor tab.
     */
    void createActions();

    /*!
    Creates Widget of BilboEditor.
    then assigns default charachter format of the editor tab to defaultCharFormat
     * variable, to be used in remove formatting operation. then calls createActions
     * function.
    \sa sltRemoveFormatting(), createActions()
     */
    void createUi();

    /*!
    Prepares the html code to be used by editor->setHtml() function.
     */
//     QString htmlToRichtext( const QString& html );

    QWidget *tabVisual;
    QWidget *tabHtml;
    QWidget *tabPreview;

    MultiLineTextEdit *editor;
//     QPlainTextEdit *htmlEditor;
    KTextEditor::View *htmlEditor;
//     QWebView *preview;
//     KHTMLPart *browserPart;
    BilboBrowser *preview;

    KToolBar *barVisual;

    KListWidget *lstMediaFiles;
//     KPushButton *btnGetStyle;

    KAction *actBold;
    KAction *actItalic;
    KAction *actUnderline;
    KAction *actStrikeout;
    KAction *actCode;
    KAction *actFontIncrease;
    KAction *actFontDecrease;
    KAction *actNewParagraph;
    KAction *actAlignRight;
    KAction *actAlignLeft;
    KAction *actAlignCenter;
    KAction *actJustify;
    KAction *actRightToLeft;
    KAction *actAddLink;
    KAction *actRemoveLink;
    KAction *actRemoveFormatting;
    KAction *actColorSelect;
    KAction *actAddImage;
//     KAction *actAddMedia;
    KAction *actOrderedList;
    KAction *actUnorderedList;
    KAction *actBlockQuote;
    KAction *actSplitPost;
    KAction *actCheckSpelling;
    KSelectAction *actFormatType;

//     QProgressBar *previewProgress;
//     KStatusBar *previewStatus;

//   QString *mHtmlContent;

    AddEditLink *linkDialog;
    
    QTextCharFormat defaultCharFormat;
    QTextBlockFormat defaultBlockFormat;
    QTextCharFormat lastCharFormat;
    QTextBlockFormat lastBlockFormat;
    
    QMap <QString, BilboMedia*> *mMediaList;

    QString currentPostTitle;
    int prev_index;
    QColor codeBackground;
};

#endif
