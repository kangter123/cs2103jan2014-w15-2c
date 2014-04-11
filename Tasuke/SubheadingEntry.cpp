#include "Tasuke.h"
#include "SubheadingEntry.h"

//@author A0100189

SubheadingEntry::SubheadingEntry(const QString& content, QWidget *parent): QWidget(parent) {
	initUI();
	changeText(content);
}

SubheadingEntry::~SubheadingEntry() {

}

void SubheadingEntry::changeText(const QString& text) {
	ui.content->setText(text);
}

void SubheadingEntry::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
}