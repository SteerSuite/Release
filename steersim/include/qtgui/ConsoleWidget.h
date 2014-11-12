//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __CONSOLE_WIDGET_H__
#define __CONSOLE_WIDGET_H__

/// @file ConsoleWidget.h
/// @brief Declares the ConsoleWidget class, which is a console that redirects C++ standard output streams (cout, cerr, and clog).

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>
#include "SteerLib.h"

namespace SteerSimQt {

	class ConsoleWidget : public QWidget
	{
		Q_OBJECT

	public:
		static ConsoleWidget * getInstance();

	protected slots:
		void echoCmdInput();
		void formatScrollBar();
		void analyzeSliderChange(int action);

	protected:
		ConsoleWidget();
		~ConsoleWidget();

		void _init();

		QLineEdit * _lineEdit;
		QTextEdit * _textEdit;

		QTextCursor _textCursor;

		QScrollBar * _vScrollBar;

		bool _scrollBarAtBottom;
		int _storedSliderValue;

	private:
		class qtOutStreamBuf : public std::streambuf
		{
		public:
			qtOutStreamBuf() {}
			~qtOutStreamBuf() {if(_textEdit != NULL) delete _textEdit;}
			void init(QTextEdit * _newTextEdit, QTextCursor * _newTextCursor) {_textEdit = _newTextEdit; _textCursor = _newTextCursor;}
		protected:
			std::streamsize xsputn ( const char * s, std::streamsize n );
			int overflow ( int c = EOF );

			QTextEdit * _textEdit;
			QTextCursor * _textCursor;
		};

		class qtErrStreamBuf : public std::streambuf
		{
		public:
			qtErrStreamBuf() {}
			~qtErrStreamBuf() {if(_textEdit != NULL) delete _textEdit;}
			void init(QTextEdit * _newTextEdit, QTextCursor * _newTextCursor) {_textEdit = _newTextEdit; _textCursor = _newTextCursor;}
		protected:
			std::streamsize xsputn ( const char * s, std::streamsize n );
			int overflow ( int c = EOF );

			QTextEdit * _textEdit;
			QTextCursor * _textCursor;
		};

		class qtLogStreamBuf : public std::streambuf
		{
		public:
			qtLogStreamBuf() {}
			~qtLogStreamBuf() {if(_textEdit != NULL) delete _textEdit;}
			void init(QTextEdit * _newTextEdit, QTextCursor * _newTextCursor) {_textEdit = _newTextEdit; _textCursor = _newTextCursor;}
		protected:
			std::streamsize xsputn ( const char * s, std::streamsize n );
			int overflow ( int c = EOF );

			QTextEdit * _textEdit;
			QTextCursor * _textCursor;
		};

		qtOutStreamBuf _qtcoutstreamBuf;
		qtErrStreamBuf _qtcerrstreamBuf;
		qtLogStreamBuf _qtclogstreamBuf;
	}; 

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif
