//
// Copyright (c) 2009-2014 Shawn Singh, Glen Berseth, Mubbasir Kapadia, Petros Faloutsos, Glenn Reinman
// See license.txt for complete license.
//

#ifdef ENABLE_GUI
#ifdef ENABLE_QT

#include "SteerLib.h"
#include "qtgui/ModuleManagerWidget.h"

using namespace SteerLib;
using namespace SteerSimQt;

ModuleLoaderWidget::ModuleLoaderWidget(EngineInterface * engine)
{
	_engine = engine;
}

void ModuleLoaderWidget::init()
{
	const std::vector<SteerLib::ModuleInterface*> & _allModules = _engine->getAllModules();

	for(std::vector<SteerLib::ModuleInterface*>::const_iterator i = _allModules.begin(); i != _allModules.end(); i++)
	{
		_modules << QString::fromStdString(_engine->getModuleMetaInfo((*i))->moduleName);
	}

	_moduleDisplay = new QListWidget;
	_moduleDisplay->addItems(_modules);
	_moduleDisplay->setMovement(QListView::Snap);
	_moduleDisplay->setDragDropMode(QAbstractItemView::InternalMove);

	_metaName = new QLabel();
	_metaName->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaDllBool = new QLabel();
	_metaDllBool->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaConflicts = new QLabel();
	_metaConflicts->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaDependencies = new QLabel();
	_metaDependencies->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaModulesDependOnThis = new QLabel();
	_metaModulesDependOnThis->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaIsLoaded = new QLabel();
	_metaIsLoaded->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	_metaIsInitialized = new QLabel();
	_metaIsInitialized->setFrameStyle(QFrame::Sunken | QFrame::Panel);

	QGridLayout * _layout = new QGridLayout;

	_layout->addWidget(_moduleDisplay,0,0,1,2);
	_layout->addWidget(new QLabel("Module Name:"),1,0);
	_layout->addWidget(_metaName,1,1);
	_layout->addWidget(new QLabel("Dynamic:"),2,0);
	_layout->addWidget(_metaDllBool,2,1);
	_layout->addWidget(new QLabel("Conflicts:"),3,0);
	_layout->addWidget(_metaConflicts,3,1);
	_layout->addWidget(new QLabel("Dependencies:"),4,0);
	_layout->addWidget(_metaDependencies,4,1);
	_layout->addWidget(new QLabel("Modules using this:"),5,0);
	_layout->addWidget(_metaModulesDependOnThis,5,1);
	_layout->addWidget(new QLabel("Loaded:"),6,0);
	_layout->addWidget(_metaIsLoaded,6,1);
	_layout->addWidget(new QLabel("Initialized:"),7,0);
	_layout->addWidget(_metaIsInitialized,7,1);
	

	setLayout(_layout);

	connect(_moduleDisplay, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(newItemSelected(QListWidgetItem *)));
}

void ModuleLoaderWidget::newItemSelected(QListWidgetItem * item)
{
	std::string _conflicts;
	std::string _dependencies;
	std::string _modulesDependentOnThis;

	SteerLib::ModuleMetaInformation * _metaInfo = _engine->getModuleMetaInfo(item->text().toStdString());
	_metaName->setText(QString::fromStdString(_metaInfo->moduleName));
	_metaDllBool->setText((_metaInfo->dll == NULL) ? "No" : "Yes");

	for(std::set<std::string>::const_iterator i = _metaInfo->conflicts.begin(); i != _metaInfo->conflicts.end(); i++)
	{	
		_conflicts += (*i);
		_conflicts += "; ";
	}

	_metaConflicts->setText(QString::fromStdString(_conflicts));

	for(std::set<ModuleMetaInformation*>::const_iterator i = _metaInfo->dependencies.begin(); i != _metaInfo->dependencies.end(); i++)
	{
		_dependencies += (*i)->moduleName;
		_dependencies += "; ";
	}

	_metaDependencies->setText(QString::fromStdString(_dependencies));

	for(std::set<ModuleMetaInformation*>::const_iterator i = _metaInfo->modulesDependentOnThis.begin(); i != _metaInfo->modulesDependentOnThis.end(); i++)
	{
		_modulesDependentOnThis += (*i)->moduleName;
		_modulesDependentOnThis += "; ";
	}

	_metaModulesDependOnThis->setText(QString::fromStdString(_modulesDependentOnThis));

	_metaIsLoaded->setText(QString::fromStdString((_metaInfo->isLoaded ? "Yes" : "No")));
	_metaIsInitialized->setText(QString::fromStdString((_metaInfo->isInitialized ? "Yes" : "No")));
}

#endif // ifdef ENABLE_QT
#endif // ifdef ENABLE_GUI
