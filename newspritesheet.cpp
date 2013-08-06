#include <QFile>
#include <iostream>
#include "newspritesheet.hpp"
#include "models/enginemodels.hpp"
#include "ui_newspritesheet.h"

NewSpriteSheet::NewSpriteSheet(QString projectPath, QWidget *parent): QDialog(parent), ui(new Ui::NewSpriteSheet) {
	ui->setupUi(this);
	m_projectPath = projectPath;
}

NewSpriteSheet::~NewSpriteSheet() {
	delete ui;
}

void NewSpriteSheet::accept() {
	if (!QFile::exists(ui->leName->text() + ".json")) {
		enginemodels::SpriteSheet model;

		QStringList ts = ui->cbTileSize->currentText().split("x");
		model.tileWidth = ts[0].toInt();
		model.tileHeight = ts[1].toInt();

		QStringList ss = ui->cbSheetSize->currentText().split("x");
		model.tilesWide = ss[0].toInt();
		model.tilesHigh = ss[1].toInt();

		std::cout << model.write() << std::endl;

		model.writeFile(m_projectPath.toStdString() + "/Resources/SpriteSheets/" + ui->leName->text().toStdString() + ".json");
		this->close();
		this->parentWidget()->activateWindow();
	}
}