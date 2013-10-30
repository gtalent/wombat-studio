#ifndef SPRITESHEETEDITOR_HPP
#define SPRITESHEETEDITOR_HPP

#include <QString>
#include <QMap>
#include <QGraphicsScene>

#include "models/models.hpp"

#include "editortab.hpp"

namespace Ui {
class SpriteSheetEditor;
}

class SpriteSheetEditor: public EditorTab {
	Q_OBJECT

	private:

		class Image {
			public:
				QPixmap pxMap;
				QImage img;
				models::Bounds srcBnds;
				int imgId;
				// The location of the image on the sprite sheet
				int x, y;
		};

		//COMMANDS
		class AddImageCommand: public QUndoCommand {
			private:
				QVector<Image> m_newImages;
				SpriteSheetEditor *m_parent;
				models::SpriteSheet m_before;
				models::SpriteSheet m_after;
			public:
				AddImageCommand(SpriteSheetEditor *parent, QVector<Image> imgs,
									 models::SpriteSheet before, models::SpriteSheet after);
				void undo();
				void redo();
		};

		Ui::SpriteSheetEditor *ui;
		models::SpriteSheet m_model;
		models::Point m_sheetIdx;
		QString m_projectDir;
		QGraphicsScene *m_scene;
		QMap<int, Image> m_imgs;

	public:
		explicit SpriteSheetEditor(QWidget *parent, QString projectDir, QString path);
		~SpriteSheetEditor();
		QImage buildImage(QImage*, models::Bounds);
		virtual bool saveFile();
		
	public slots:
		int addImages();

	private:
		int load(QString path);
		void draw();
		int newImageId();
		void recycleImageId(int imgId);
};

#endif // SPRITESHEETEDITOR_HPP
