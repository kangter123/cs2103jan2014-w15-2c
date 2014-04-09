#ifndef INPUTWINDOW_H
#define INPUTWINDOW_H

#include <QWidget>
#include <glog/logging.h>
#include <QApplication>
#include <QDesktopWidget>
#include <QKeyEvent>
#include <QFocusEvent>
#include <QPropertyAnimation>
#include "HotKeyThread.h"
#include "ui_InputWindow.h"
#include "InputHighlighter.h"
#include "TooltipWidget.h"
#include "Constants.h"

//@author A0100189

class InputWindow : public QWidget {
	Q_OBJECT
	Q_PROPERTY(qreal opacity READ getOpacity WRITE setOpacity)

public:
	InputWindow(QWidget *parent = 0);
	~InputWindow();

	void showTooltipMessage(InputStatus status, QString message = "");
	void hideTooltip();
	void doErrorAnimation();

public slots:
	void showAndCenter();	
	void showAndAdd();
	void closeAndClear();

protected:
	bool eventFilter(QObject *object, QEvent *event);
	void hideEvent(QHideEvent *event);

signals:
	void inputChanged(QString text);

private slots:
	void handleReturnPressed();
	void handleLineEditChanged();

private:
	
	// ====================================================
	//	Attributes
	// ====================================================
	Ui::InputWindow ui;
    InputHighlighter *highlighter;
	TooltipWidget *tooltipWidget;
	QPropertyAnimation animation;
	QPropertyAnimation errorAnimation;
	qreal wOpacity;

	
	// ====================================================
	//	Functions
	// ====================================================
	void setOpacity(qreal value);
	qreal getOpacity() const;
	void initUI();
	void initWidgets();
	void initConnect();
	void initAnimation();
	void initErrorAnimation();
};

#endif // INPUTWINDOW_H
