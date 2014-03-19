#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QtGui\qbitmap.h>
#include "Tasuke.h"
#include "InputWindow.h"

InputWindow::InputWindow(QWidget* parent) : QWidget(parent) {
	LOG(INFO) << "InputWindow instance created";

	ui.setupUi(this);

	setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background:transparent;");
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);

	connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturnPressed()));
	connect(ui.lineEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));
}

InputWindow::~InputWindow() {
	LOG(INFO) << "InputWindow instance destroyed";
}

void InputWindow::showAndCenter() {
	LOG(INFO) << "Displaying input window";

	QPoint pos = QApplication::desktop()->screen()->rect().center() - rect().center();
	if(Tasuke::instance().getTaskWindow().isVisible()){ //if taskWindow is open
		pos.setY(Tasuke::instance().getTaskWindow().y() + Tasuke::instance().getTaskWindow().height() + 3); //set commandbox below taskWindow
		pos.setX(Tasuke::instance().getTaskWindow().x());
	} else {
		pos.setY(QApplication::desktop()->screen()->rect().height() / 4);
	}

	move(pos);

	show();
	raise();
	activateWindow();
}

//will be updated when "themes" is implemented.
void InputWindow::changeBorder(int themeNumber){
	QPixmap pxr(QString::fromUtf8("InputWindowMask.png"));
	QPixmap pxr2(QString::fromUtf8("InputWindowMask.png"));
	pxr.fill(Qt::red);
	pxr.setMask( pxr2.createMaskFromColor( Qt::transparent ) );


    ui.label_2->setPixmap(pxr);
}

//will be updated when "themes" is implemented.
void InputWindow::changeBG(int themeNumber){
	//ui.label_2->setPixmap(pxr);
}

void InputWindow::handleReturnPressed() {
	std::string command = ui.lineEdit->text().toUtf8().constData();

	if (command.empty()) {
		return;
	}

	Tasuke::instance().runCommand(command);
}

void InputWindow::handleEditingFinished() {
	hide();
	ui.lineEdit->clear();
}