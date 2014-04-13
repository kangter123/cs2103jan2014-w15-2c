#include <QSettings>
#include "Tasuke.h"
#include "Constants.h"
#include "TooltipWidget.h"

//@author A0100189m

TooltipWidget::TooltipWidget(QWidget *parent) : QWidget(parent), font("Consolas", 11), fm(font), 
												animation(this, "pos") {
	LOG(INFO) << "TooltipWidget instance created";

	initUI();
	handleReloadIcons();
	initAnimation();
}

TooltipWidget::~TooltipWidget() {
	LOG(INFO) << "TooltipWidget instance destroyed";
}

// Changes the text shown on the window then resizes it.
void TooltipWidget::setText(InputStatus status, QString content) {
	setIconOnLabel(status);

	// Only update text if provided
	if (!content.isEmpty()) {
		ui.text->setText(content);
		fitWidthToTextLength(content);
	}
}

// Shows the widget with animation
void TooltipWidget::showAndAlign() {
	LOG(INFO) << "Displaying tooltip widget";

	resetAnimation();
	show();
	animation.start();
}

// Initialize icon set from settings
void TooltipWidget::handleReloadIcons() {
	LOG(INFO) << "Initialising tooltip widget icons";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	IconSet iconSet = (IconSet)settings.value("Icon", (char)IconSet::NYANSUKE).toInt();

	switch (iconSet) {
		case IconSet::NYANSUKE:
			normalIcon = QPixmap(NYANSUKE_NORMAL);
			successIcon = QPixmap(NYANSUKE_SUCCESS);
			failureIcon = QPixmap(NYANSUKE_FAILURE);
			break;
		case IconSet::SYMBOLS:
			normalIcon = QPixmap(SYMBOL_NORMAL);
			successIcon = QPixmap(SYMBOL_SUCCESS);
			failureIcon = QPixmap(SYMBOL_FAILURE);
			break;
		case IconSet::SYMBOLS2:
			normalIcon = QPixmap(SYMBOL2_NORMAL);
			successIcon = QPixmap(SYMBOL2_SUCCESS);
			failureIcon = QPixmap(SYMBOL2_FAILURE);
			break;
		case IconSet::NICCAGE:
			normalIcon = QPixmap(NICCAGE_NORMAL);
			successIcon = QPixmap(NICCAGE_SUCCESS);
			failureIcon = QPixmap(NICCAGE_FAILURE);
			break;
		case IconSet::MEME:
			normalIcon = QPixmap(MEME_NORMAL);
			successIcon = QPixmap(MEME_SUCCESS);
			failureIcon = QPixmap(MEME_FAILURE);
			break;
		case IconSet::SHIBE:
			normalIcon = QPixmap(SHIBE_NORMAL);
			successIcon = QPixmap(SHIBE_SUCCESS);
			failureIcon = QPixmap(SHIBE_FAILURE);
			break;
		default:
			break;
	}
}

// Modifies width according to text width
void TooltipWidget::fitWidthToTextLength(QString text) {
	QTextDocument document;
	document.setDefaultFont(QFont("Consolas", 11));
	document.setHtml(text);
	ui.bg->resize(document.idealWidth() + WIDTH_DIFFERENCE, ui.bg->height()); 
}

// Sets the icon according to the status of the input
void TooltipWidget::setIconOnLabel(InputStatus status) {
	switch (status) {
		case InputStatus::SUCCESS:
			ui.icon->setPixmap(successIcon);
			break;
		case InputStatus::FAILURE:
			ui.icon->setPixmap(failureIcon);
			break;
		case InputStatus::NORMAL:
			ui.icon->setPixmap(normalIcon);
			break;
		default:
			break;
	}
}

// ===================================
// initialization
// ===================================

void TooltipWidget::initUI() {
	ui.setupUi(this);
	setAttribute(Qt::WA_TranslucentBackground);
	setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
}


// ===================================
// animation
// ===================================

void TooltipWidget::initAnimation() {
	animation.setDuration(500);
	animation.setEasingCurve(QEasingCurve::OutCubic);
}

void TooltipWidget::resetAnimation() {
	QPoint posBefore;
	posBefore.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height() - 20);
	posBefore.setX(Tasuke::instance().getInputWindow().x() + 8);

	QPoint posAfter;
	posAfter.setY(Tasuke::instance().getInputWindow().y() + Tasuke::instance().getInputWindow().height());
	posAfter.setX(Tasuke::instance().getInputWindow().x() + 8);

	animation.setStartValue(posBefore);
	animation.setEndValue(posAfter);
}