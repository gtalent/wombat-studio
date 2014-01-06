#include <QDir>

#include "imageselectordialog.hpp"
#include "ui_imageselectordialog.h"

#include <iostream>

namespace wombat {
namespace editor {

ImageSelectorDialog::ImageSelectorDialog(QWidget *parent, QString): QDialog(parent), ui(new Ui::ImageSelectorDialog) {
	ui->setupUi(this);
}

ImageSelectorDialog::~ImageSelectorDialog() {
	delete ui;
}

}
}
