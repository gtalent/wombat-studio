#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QModelIndex>
#include <QMainWindow>
#include <QPoint>
#include <QString>

#include "editorcore/misc.hpp"
#include "editorcore/editorwidget.hpp"
#include "editorcore/editorprofile.hpp"
#include "editorcore/modeliomanager.hpp"

namespace Ui {
class MainWindow;
}

class MainWindow: public QMainWindow, public wombat::editor::EditorWidgetListener, public wombat::editor::DebugLogger {
	Q_OBJECT
	public:
		Ui::MainWindow *ui;
	private:
		QString m_projectPath;
		QMap<QString, wombat::editor::EditorWidget*> m_openTabs;
		wombat::editor::ModelIoManager m_models;
		wombat::editor::EditorProfile *m_profile;

	public:
		explicit MainWindow(wombat::editor::EditorProfile *profile = 0, QWidget *parent = 0);
		~MainWindow();

		void openProject(QString);
		virtual void logDebug(QString msg);

	private:
		wombat::editor::EditorWidget *currentTab();
		int readSettings(QString path);
		int writeSettings(QString path);

	public slots:
		void newMenu();
		void openProject();
		void openFile(QModelIndex);
		void openFile(QString);
		void saveFile();
		void undo();
		void redo();
		void changeTab();
		void closeTab(int index);
		void closeTab(wombat::editor::EditorWidget *tab);
		void filePaneContextMenu(const QPoint &p);

		void fileSaved();
		void fileChanged();
};

#endif // MAINWINDOW_HPP