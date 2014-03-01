#include <QApplication>
#include <QDesktopWidget>
#include "Tasuke.h"
#include "InputWindow.h"

InputWindow::InputWindow(QWidget* parent) : QWidget(parent) {
	ui.setupUi(this);

	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
	
	connect(ui.lineEdit, SIGNAL(returnPressed()), this, SLOT(handleReturnPressed()));
	connect(ui.lineEdit, SIGNAL(editingFinished()), this, SLOT(handleEditingFinished()));

	hotKeyThread = new HotKeyThread(this);
	connect(hotKeyThread, SIGNAL(hotKeyPress(int)), this, SLOT(handleHotKeyPress(int)), Qt::QueuedConnection);
	hotKeyThread->start();
}

InputWindow::~InputWindow() {

}

void InputWindow::showAndCenter() {
	show();
	raise();
	activateWindow();

	move(QApplication::desktop()->screen()->rect().center() - rect().center());
}

void InputWindow::closeEvent(QCloseEvent* event) {
	hotKeyThread->stop();
}

void InputWindow::handleReturnPressed() {
	Tasuke::instance().runCommand(ui.lineEdit->text().toUtf8().constData());
}

void InputWindow::handleEditingFinished() {
	hide();
	ui.lineEdit->clear();
}

void InputWindow::handleHotKeyPress(int key) {
	showAndCenter();
}