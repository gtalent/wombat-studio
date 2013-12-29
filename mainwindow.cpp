#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QMenu>
#include <QString>

#include "editorcore/misc.hpp"
#include "mainwindow.hpp"
#include "editorcore/editortab.hpp"
#include "newmenu.hpp"
#include "savevariables.hpp"
#include "spritesheeteditor.hpp"
#include "importmenu.hpp"
#include "newproject.hpp"
#include "newspritesheet.hpp"
#include "ui_mainwindow.h"

QString defaultPaths[] = {
	"Animations",
	"Creatures",
	"Creatures/Classes",
	"Creatures/Moves",
	"Creatures/Types",
	"Instances",
	"Instances/Creatures",
	"Instances/People",
	"Items",
	"Misc",
	"Misc/SaveVariables.json",
	"People",
	"People/Classes",
	"Resources",
	"Resources/SpriteSheets",
	"Worlds",
	"Worlds/Sprites",
	"Worlds/TileClasses",
	"Worlds/Worlds",
	"Worlds/Zones"
};

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	// for some reason this can't be done QtDesigner
	connect(ui->fileList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(filePaneContextMenu(QPoint)));

	ui->actionAnimationEditor->setEnabled(false);
	ui->dockAnimationEditor->setVisible(false);

	ui->dockDebug->setVisible(false);

	readSettings("editor_settings.json");
}

MainWindow::~MainWindow() {
	writeSettings("editor_settings.json");

	if (ui->fileList->model())
		delete ui->fileList->model();
	delete ui;
}

int MainWindow::readSettings(QString path) {
	int retval = 0;

	models::EditorSettings settings;
	retval = settings.readJsonFile(path);

	openProject(settings.openProject);

	for (auto i : settings.openFiles) {
		openFile(i);
	}
	ui->tabWidget->setCurrentIndex(settings.openTab);

	return retval;
}

int MainWindow::writeSettings(QString path) {
	models::EditorSettings settings;

	settings.openProject = m_projectPath;
	for (int i = 0; i < ui->tabWidget->count(); i++) {
		auto t = dynamic_cast<EditorTab*>(ui->tabWidget->widget(i));
		if (t) {
			settings.openFiles.push_back(t->path());
		}
	}
	settings.openTab = ui->tabWidget->currentIndex();

	settings.writeJsonFile(path, models::cyborgbear::Readable);

	return 0;
}

void MainWindow::newMenu() {
	NewMenu menu(this, "");
	if (!menu.exec()) {
		QString nw = menu.newWhat();
		if (nw == "Project") {
			NewProject np(this);
			if (np.exec() == 0) {
				QString p = np.path();
				if (p != "") {
					openProject(p);
				}
			}
		} else if (nw == "Sprite Sheet") {
			NewSpriteSheet np(m_projectPath, this);
			if (np.exec() == 0) {
				openFile(np.path());
			}
		}
	}
}

void MainWindow::openProject() {
	QString p = QFileDialog::getExistingDirectory(this, tr("Open Project Directory..."), QDir::homePath());
	if (p != "") {
		openProject(p);
	}
}

void MainWindow::openProject(QString path) {
	if (!path.endsWith("/"))
		path += "/";
	m_projectPath = path;
	setWindowTitle("Wombat Editor - " + path.mid(0, path.length() - 1));

	if (ui->fileList->model())
		delete ui->fileList->model();

	QFileSystemModel *model = new QFileSystemModel();
	m_fsModel = model;
	ui->fileList->setModel(model);
	ui->fileList->setHeaderHidden(true);
	for (int i = 1; i < 4; i++) {
		ui->fileList->hideColumn(i);
	}
	QStringList l;
	l << "*.json";
	model->setNameFilters(l);
	model->setNameFilterDisables(false);
	ui->fileList->setRootIndex(model->setRootPath(path));
	ui->actionImport->setEnabled(true);
}

void MainWindow::openFile(QModelIndex index) {
	QString path = ((QFileSystemModel*) ui->fileList->model())->fileInfo(index).canonicalFilePath();
	openFile(path);
}

void MainWindow::openFile(QString path) {
	EditorTab *tab = m_openTabs[path];
	if (!tab) {
		QString tabName = "";
		QStringList list = path.split("/");
		tabName = list[list.size() - 1];

		if (m_projectPath + "Misc/SaveVariables.json" == path) {
			//open save variables tab
			tab = new SaveVariables(ui->tabWidget, path);
		} else if (path.startsWith(m_projectPath + "Resources/SpriteSheets/")) {
			tab = new SpriteSheetEditor(ui->tabWidget, m_projectPath, path);
		}

		if (tab) {
			tab->addListener(this);
			tab->title(tabName);
			m_openTabs[path] = tab;
			ui->tabWidget->addTab(tab, tab->title());

			ui->tabWidget->setCurrentWidget(tab);
		}
	} else {
		ui->tabWidget->setCurrentWidget(tab);
	}
}

void MainWindow::import() {
	ImportMenu(this, m_projectPath).run();
}

void MainWindow::undo() {
	EditorTab *tab = currentTab();
	if (tab) {
		tab->undo();
		ui->actionUndo->setEnabled(tab->canUndo());
		ui->actionRedo->setEnabled(tab->canRedo());
	}
}

void MainWindow::redo() {
	EditorTab *tab = currentTab();
	if (tab) {
		tab->redo();
		ui->actionUndo->setEnabled(tab->canUndo());
		ui->actionRedo->setEnabled(tab->canRedo());
	}
}

void MainWindow::closeTab(EditorTab *tab) {
	if (tab) {
		ui->actionUndo->setEnabled(false);
		ui->actionRedo->setEnabled(false);
		tab->close();
		m_openTabs[tab->path()] = 0;
		m_openTabs.erase(m_openTabs.find(tab->path()));
		delete tab;
	}
}

void MainWindow::closeTab(int index) {
	closeTab(dynamic_cast<EditorTab*>(ui->tabWidget->widget(index)));
}

void MainWindow::filePaneContextMenu(const QPoint &itemPt) {
	QModelIndex index = ui->fileList->currentIndex();
	QPoint p = ui->fileList->mapToGlobal(itemPt);
	QMenu m;

	QString path = ((QFileSystemModel*) ui->fileList->model())->fileInfo(index).canonicalFilePath();
	QString projectDir = m_projectPath;

	if (path != "") {
		bool fileDeletable = true;
		unsigned long size = sizeof(defaultPaths) / sizeof(QString);
		for (unsigned long i = 0; i < size; i++) {
			if (path == projectDir + defaultPaths[i]) {
				fileDeletable = false;
				break;
			}
		}
		if (fileDeletable) {
			m.addAction("&Delete");
			QAction *qaction = m.exec(p);

			if (qaction) {
				QString action = qaction->text();
				if (action == "&Delete") {
					QFile::remove(path);
				}
			}
			closeTab(m_openTabs[path]);
		}
	}
}

EditorTab *MainWindow::currentTab() {
	return dynamic_cast<EditorTab*>(ui->tabWidget->currentWidget());
}

void MainWindow::logDebug(QString msg) {
	ui->listDebugLog->addItem(msg);
	ui->listDebugLog->scrollToBottom();
}

void MainWindow::saveFile() {
	EditorTab *tab = currentTab();
	if (tab)
		tab->saveFile();
}

void MainWindow::fileSaved() {
	ui->actionSave->setEnabled(false);
	auto tab = currentTab();
	auto tabIdx = ui->tabWidget->indexOf(tab);
	ui->tabWidget->setTabText(tabIdx, tab->title());
}

void MainWindow::fileChanged() {
	EditorTab *tab = currentTab();
	if (tab) {
		ui->actionSave->setEnabled(true);
		ui->actionUndo->setEnabled(tab->canUndo());
		ui->actionRedo->setEnabled(tab->canRedo());

		auto tab = currentTab();
		auto tabIdx = ui->tabWidget->indexOf(tab);
		ui->tabWidget->setTabText(tabIdx, "* " + tab->title());
	}
}

void MainWindow::changeTab() {
	EditorTab *tab = currentTab();
	if (tab) {
		ui->actionSave->setEnabled(!tab->currentStateSaved());
		ui->actionUndo->setEnabled(tab->canUndo());
		ui->actionRedo->setEnabled(tab->canRedo());
	}
}
