//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "qtgui/GLWidget.h"
#include "qtgui/GlobalEventFilter.h"

using namespace SteerSimQt;

GlobalEventFilter::GlobalEventFilter(QtEngineController * controller, QMainWindow * mainWindow, GLWidget * gl)
{
	_controller = controller;
	_mainWindow = mainWindow;
	_glWidget = gl;
}

bool GlobalEventFilter::eventFilter( QObject * obj, QEvent * qtevent)
{
	//
	// This code tries to capture global events.  The implementation feels
	// conceptually incorrect, but it works reliably, so we keep it this way.
	//
	// To capture global events (exactly once per event) from Qt, it seems like we should
	// be capturing all these events from the QApplication instance.  However, when we tried that, events
	// occurred multiple times.  If we try to catch events through the QMainWindow, not all events get
	// caught (in particular, the GLWidget events).  So, the working solution is to catch events from the
	// QMainWindow and GLWidget	together, until we understand the "right" qt way to do it.
	//
	/// @todo
	///   - check if these global event filtering problems are due to the way we are using Qt, causing multiple repeated events in the QApplication object?
	// 
	if ((obj == _mainWindow) || (obj == _glWidget)) {
		if (qtevent->type() == QEvent::KeyPress) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(qtevent);
			if (keyEvent->key() == Qt::Key_Control) {
				_glWidget->setControlKey(true);
			}
			else if (keyEvent->key() == Qt::Key_Escape) {
				_mainWindow->close();
			}
			return false;
		}
		else if (qtevent->type() == QEvent::KeyRelease) {
			QKeyEvent *keyEvent = static_cast<QKeyEvent*>(qtevent);
			if (keyEvent->key() == Qt::Key_Control) {
				_glWidget->setControlKey(false);
			}
			return false;
		}
		else if (qtevent->type() == QEvent::Close) {
			if (_controller->_engine->isSimulationRunning()) {
				bool alreadyPaused = _controller->_paused;
				if (!alreadyPaused) _controller->_pauseSimulation();

				QMessageBox askUserDialog;
				askUserDialog.setWindowTitle("Really exit?");
				askUserDialog.setText("A simulation is still running.");
				askUserDialog.setInformativeText("Are you sure you want to exit?");
				askUserDialog.addButton(QMessageBox::Cancel);
				QPushButton *exitButton = askUserDialog.addButton("Exit", QMessageBox::YesRole);
				askUserDialog.setDefaultButton(QMessageBox::Cancel);
				askUserDialog.exec();
				if (askUserDialog.clickedButton() == exitButton) {
					return false; // propagate the close() event so that everything closes.
				}
				else {
					if (!alreadyPaused) _controller->_unpauseSimulation();
					qtevent->ignore();
					return true;  // this event will be filtered out and not received by any other objects.
				}
			}
			else {
				return false;  // if no simulation is running, go ahead and close without asking the user.
			}
		}
		/*
		// this was a working example of how to add a popup menu using this event
		// filter, however, for now the popup menu is not implemented.
		else if (qtevent->type() == QEvent::MouseButtonPress) {
			QMouseEvent * mouseEvent = static_cast<QMouseEvent*>(qtevent);
			if (mouseEvent->button() == Qt::RightButton) {
				if (obj == _glWidget) {
					_popupMenu->popup(mouseEvent->globalPos());
					return true;
				}
				else {
					return false;
				}
			}
			else {
				return false;
			}
		}
		*/
		else {
			return false; // propagate any un-handled events.
		}
	}
	else {
		return QObject::eventFilter(obj, qtevent);
	}
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
