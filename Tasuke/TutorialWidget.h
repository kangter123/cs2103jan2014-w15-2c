#ifndef TUTORIALWIDGET_H
#define TUTORIALWIDGET_H

#include <QWidget>
#include "ui_TutorialWidget.h"


class TutorialWidget : public QWidget {
	Q_OBJECT

public:
	TutorialWidget(QWidget *parent = 0);
	~TutorialWidget();

public slots:
	void backToTasuke();

protected:

private slots:


private:
	Ui::TutorialWidget ui;

};

#endif // TUTORIALWIDGET_H
