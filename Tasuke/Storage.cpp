#include <QSettings>
#include <QVariant>
#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"

// Constructor for Storage.
Storage::Storage() {
	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");

	loadFile();	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	QSettings settings("Tasuke","Tasuke");
	int size = settings.beginReadArray("tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task task = settings.value("task").value<Task>();
		tasks.push_back(task);
	}
	settings.endArray();
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	QSettings settings("Tasuke","Tasuke");
	settings.beginWriteArray("tasks");
	for (int i=0; i<tasks.length(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("task", QVariant::fromValue<Task>(tasks[i]));
	}
	settings.endArray();
}

void Storage::addTask(Task& task) {
	tasks.push_back(task);
}

Task& Storage::getTask(int i) {
	throw tasks[i];
}

void Storage::removeTask(int i) {
	tasks.removeAt(i);
}