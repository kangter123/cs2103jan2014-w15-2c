#include <string>
#include <iostream>
#include <QMessageBox>
#include <QtGui\qfontdatabase.h>
#include "Constants.h"
#include "Exceptions.h"
#include "CommandFactory.h"
#include "Commands.h"
#include "Tasuke.h"

// Constructor for the Tasuke singleton.
Tasuke::Tasuke() {
	storage = new Storage();
	storage->loadFile();
	showTaskWindow();
	updateTaskWindow(storage->getTasks());
}

// Destructor for the Tasuke singleton.
Tasuke::~Tasuke() {
	if (storage != nullptr) {
		delete storage;
	}
}

void Tasuke::loadFonts(){
	QFontDatabase fontDatabase; 
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Book.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Book_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Light.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Light_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Bold.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/Quicksand_Bold_Oblique.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/PrintClearly.otf");
	fontDatabase.addApplicationFont(":/Fonts/fonts/PrintBold.otf");
}

void Tasuke::initialize(){
	loadFonts();
	taskWindow.contextMenuOperations();
}

// Static method that returns the sole instance of Tasuke.
Tasuke& Tasuke::instance() {
	static Tasuke *instance = 0;
	
	if(instance == 0){
		instance = new Tasuke();
		instance->initialize();
		return *instance;
	} else {
		return *instance;
	}
}

void Tasuke::setStorage(Storage* _storage) {
	delete storage;
	storage = _storage;
}

InputWindow& Tasuke::getInputWindow(){
	return inputWindow;
}

AboutWindow& Tasuke::getAboutWindow(){
	return aboutWindow;
}

// This function exposes the Storage instance for editing.
Storage& Tasuke::getStorage() {
	if (storage == nullptr) {
		throw ExceptionNullPtr();
	}
	return *storage;
}

void Tasuke::showInputWindow() {
	inputWindow.showAndCenter();
}

void Tasuke::showTaskWindow() {
	taskWindow.showAndMoveToSide();
}

void Tasuke::showAboutWindow(){
	aboutWindow.showAndCenter();
}

void Tasuke::hideTaskWindow() {
	taskWindow.hide();
}

void Tasuke::showMessage(QString message) {
	taskWindow.showMessage(message);
}

void Tasuke::updateTaskWindow(QList<Task> tasks) {
	taskWindow.showTasks(tasks);
}

// This function runs a command in a string
void Tasuke::runCommand(std::string commandString) {
	try {
		std::shared_ptr<ICommand> command = CommandFactory::interpret(commandString);
		if (command == nullptr) {
			return;
		}
		command->run();
		commandUndoHistory.push_back(command);
		commandRedoHistory.clear();
	} catch (ExceptionBadCommand exception) {
		showMessage("Error parsing command");
	}
}

void Tasuke::undoCommand() {
	if (commandUndoHistory.size() == 0) {
		showMessage("Nothing to undo");
		return;
	}

	std::shared_ptr<ICommand> command = commandUndoHistory.back();
	commandUndoHistory.pop_back();
	command->undo();
	commandRedoHistory.push_back(command);
}

void Tasuke::redoCommand() {
	if (commandRedoHistory.size() == 0) {
		// Nothing to redo
		return;
	}

	std::shared_ptr<ICommand> command = commandRedoHistory.back();
	commandRedoHistory.pop_back();
	command->run();
	commandUndoHistory.push_back(command);
}