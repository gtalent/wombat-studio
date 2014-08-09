#ifndef EDITORCORE_EDITORWIDGET_HPP
#define EDITORCORE_EDITORWIDGET_HPP

#include <QWidget>
#include <QUndoStack>

#include "modeliomanager.hpp"
#include "editorwidgetparams.hpp"

namespace editor {

class EditorWidget: public QWidget {
	Q_OBJECT
	friend class EditorProfile;
	private:
		ModelIoManager *m_models;

		QUndoStack *m_undoStack;

		int m_lastCommand;
		int m_lastSavedCommand;

		QString m_title;
		QString m_path;

	public:
		EditorWidget(EditorWidgetParams);
		~EditorWidget();
		bool currentStateSaved();
		QString title();
		void title(QString);
		void undo();
		void redo();
		bool canUndo();
		bool canRedo();
		QString path();
		virtual void closeWidget();
		virtual int saveFile() = 0;

	signals:
		void fileSaved();
		void fileChanged();

	protected:
		void modelIoManager(ModelIoManager*);
		ModelIoManager *modelIoManager();
		void notifyFileChange(QUndoCommand *c = 0);
		void notifyFileSave();
};

}

#endif
