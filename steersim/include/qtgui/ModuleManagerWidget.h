//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifndef __MODULE_MANAGER_WIDGET_H__
#define __MODULE_MANAGER_WIDGET_H__

/// @file ModuleManagerWidget.h
/// @brief Declares the ModuleLoaderWidget class, which is the Qt GUI component for managing modules.

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include <QtGui/QtGui>
#include <QtCore/QStringList>
#include <QtGui/QMainWindow>
#include <QtGui/QTextCursor>
#include <QtGui/QApplication>
#include "SteerLib.h"

namespace SteerSimQt {

	class ModuleLoaderWidget : public QWidget
	{
		Q_OBJECT

	public:
		ModuleLoaderWidget(SteerLib::EngineInterface * engine);
		void init();

	protected slots:
		void newItemSelected(QListWidgetItem * item);

	protected:
		QListWidget * _moduleDisplay;
		QStringList _modules;

		QLabel * _metaName;
		QLabel * _metaDllBool;
		QLabel * _metaConflicts;
		QLabel * _metaDependencies;
		QLabel * _metaModulesDependOnThis;
		QLabel * _metaIsLoaded;
		QLabel * _metaIsInitialized;

		//QButton * _loadModuleButton;

		SteerLib::EngineInterface * _engine;

	};

} // namespace SteerSimQt;

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI

#endif