//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "qtgui/ConsoleWidget.h"
#include "SteerLib.h"
#include "core/SteerSimOptions.h"

using namespace std;
using namespace Util;
using namespace SteerSimQt;


streamsize ConsoleWidget::qtOutStreamBuf::xsputn ( const char * s, streamsize n ) 
{
	_textEdit->setTextCursor(*_textCursor); 
	_textEdit->insertPlainText(QString::fromAscii(s,n));
	*_textCursor = _textEdit->textCursor();

	return n;
}

int ConsoleWidget::qtOutStreamBuf::overflow ( int c ) 
{

	_textEdit->setTextCursor(*_textCursor);
	_textEdit->insertPlainText(QString(c));
	*_textCursor = _textEdit->textCursor();

	return 1;
}

streamsize ConsoleWidget::qtErrStreamBuf::xsputn ( const char * s, streamsize n ) 
{
	_textEdit->setTextCursor(*_textCursor);

	_textEdit->setTextColor(Qt::red);
	_textEdit->insertPlainText(QString::fromAscii(s,n));
	_textEdit->setTextColor(Qt::black);

	*_textCursor = _textEdit->textCursor();

	return n;
}

int ConsoleWidget::qtErrStreamBuf::overflow ( int c ) 
{
	_textEdit->setTextCursor(*_textCursor);

	_textEdit->setTextColor(Qt::red);
	_textEdit->insertPlainText(QString(c));
	_textEdit->setTextColor(Qt::black);

	*_textCursor = _textEdit->textCursor();

	return 1;
}

streamsize ConsoleWidget::qtLogStreamBuf::xsputn ( const char * s, streamsize n ) 
{
	_textEdit->setTextCursor(*_textCursor);

	_textEdit->setTextColor(Qt::blue);
	_textEdit->insertPlainText(QString::fromAscii(s,n));
	_textEdit->setTextColor(Qt::black);

	*_textCursor = _textEdit->textCursor();

	return n;
}

int ConsoleWidget::qtLogStreamBuf::overflow ( int c ) 
{
	_textEdit->setTextCursor(*_textCursor);

	_textEdit->setTextColor(Qt::blue);
	_textEdit->insertPlainText(QString(c));
	_textEdit->setTextColor(Qt::black);

	*_textCursor = _textEdit->textCursor();

	return 1;
}


ConsoleWidget * ConsoleWidget::getInstance()
{
	static ConsoleWidget * consoleSingletonInstance = new ConsoleWidget();
	return consoleSingletonInstance;
}

ConsoleWidget::ConsoleWidget()
{
	_storedSliderValue = 0;
	_scrollBarAtBottom = true;


	_lineEdit = new QLineEdit;
	_textEdit = new QTextEdit;

	_textCursor = _textEdit->textCursor();

	_qtcoutstreamBuf.init(_textEdit, &_textCursor);
	_qtcerrstreamBuf.init(_textEdit, &_textCursor);
	_qtclogstreamBuf.init(_textEdit, &_textCursor);

	if(gSteerSimConfig.defaultCoutRedirectionFilename() == "")
		cout.rdbuf(&_qtcoutstreamBuf);
	else
		_textEdit->append(QString::fromStdString("std::cout is being redirected to " + gSteerSimConfig.defaultCoutRedirectionFilename() + "\n" ));

	if(gSteerSimConfig.defaultCerrRedirectionFilename() == "")
		cerr.rdbuf(&_qtcerrstreamBuf);
	else
		_textEdit->append(QString::fromStdString("std::cerr is being redirected to " + gSteerSimConfig.defaultCerrRedirectionFilename() + "\n" ));

	if(gSteerSimConfig.defaultClogRedirectionFilename() == "")
		clog.rdbuf(&_qtclogstreamBuf);
	else
		_textEdit->append(QString::fromStdString("std::log is being redirected to " + gSteerSimConfig.defaultClogRedirectionFilename() + "\n" ));


	connect( _lineEdit, SIGNAL(returnPressed()), this, SLOT(echoCmdInput()) );
	connect( _textEdit, SIGNAL(textChanged()), this, SLOT(formatScrollBar()) );
	// note: the signal sliderMoved only emitted when the slider is pressed down and moved, not when the scroll bar is moving
	// to fix, add mousescroll event slot and call analyzeSliderChange
	connect( _textEdit->QAbstractScrollArea::verticalScrollBar(), SIGNAL(actionTriggered(int)), this, SLOT(analyzeSliderChange(int)) );

	_init();
}

ConsoleWidget::~ConsoleWidget()
{
	if (_lineEdit != NULL)
		delete _lineEdit;
	if (_textEdit != NULL)
		delete _textEdit;
}

void ConsoleWidget::_init()
{
	QFont preferredFont( "lucida console" );
    preferredFont.setStyleHint( QFont::TypeWriter );

	_textEdit->setReadOnly(true);
	_textEdit->setFont( preferredFont );
	_textEdit->setWordWrapMode(QTextOption::NoWrap);

	QVBoxLayout * layout = new QVBoxLayout;
	layout->addWidget(_textEdit);
	layout->addWidget(_lineEdit);

	setLayout(layout);
}

void ConsoleWidget::echoCmdInput()
{
	if(_lineEdit->cursorPosition() == 0)
		return;

	_textEdit->setTextCursor(_textCursor); 

	cout << _lineEdit->text().toStdString() << endl;

	_textCursor = _textEdit->textCursor();

	_lineEdit->clear();
}

void ConsoleWidget::formatScrollBar()
{
	if(_scrollBarAtBottom == true)
		_textEdit->QAbstractScrollArea::verticalScrollBar()->setSliderPosition(_textEdit->QAbstractScrollArea::verticalScrollBar()->maximum());
	else
		_textEdit->QAbstractScrollArea::verticalScrollBar()->setSliderPosition(_storedSliderValue);
}

void ConsoleWidget::analyzeSliderChange(int value)
{
	_storedSliderValue = _textEdit->QAbstractScrollArea::verticalScrollBar()->sliderPosition();

	if(_textEdit->QAbstractScrollArea::verticalScrollBar()->sliderPosition() == _textEdit->QAbstractScrollArea::verticalScrollBar()->maximum())
		_scrollBarAtBottom = true;
	else
		_scrollBarAtBottom = false;
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
