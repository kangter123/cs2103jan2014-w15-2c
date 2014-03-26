#include <glog/logging.h>
#include <QSettings>
#include <QVariant>
#include <QJsonArray>
#include <QStandardPaths>
#include <iostream>
#include "Constants.h"
#include "Exceptions.h"
#include "Storage.h"
#include "Tasuke.h"

IStorage::IStorage() {
	
}

IStorage::~IStorage() {

}

void IStorage::addTask(Task& task) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Adding task " << task.getDescription().toStdString();

	tasks.push_back(task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void IStorage::addTask(Task& task, int pos) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Adding task " << task.getDescription().toStdString()
		<< " at position " << pos;

	tasks.insert(pos, task);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

Task& IStorage::getTask(int pos) {
	QMutexLocker lock(&mutex);
	return tasks[pos];
}

void IStorage::removeTask(int pos) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Removing task at position " << pos;

	tasks.removeAt(pos);
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

void IStorage::popTask() {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Popping task from back";

	tasks.pop_back();
	saveFile();
	Tasuke::instance().updateTaskWindow(tasks);
}

QList<Task> IStorage::getTasks() {
	QMutexLocker lock(&mutex);
	return tasks;
}

int IStorage::totalTasks() {
	QMutexLocker lock(&mutex);
	return tasks.size();
}

// Constructor for Storage.
Storage::Storage() {
	LOG(INFO) << "Storage instance created...";

	path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/tasuke.ini";

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << "Loading file...";

	QSettings settings(path, QSettings::IniFormat);
	int size = settings.beginReadArray("Tasks");
	for (int i=0; i<size; i++) {
		settings.setArrayIndex(i);
		Task task;

		task.setDescription(settings.value("Description").toString());
		uint beginTime = settings.value("BeginTimeUnix", 0).toUInt();
		if (beginTime != 0) {
			task.setBegin(QDateTime::fromTime_t(settings.value("BeginTimeUnix").toInt()));
		}
		uint endTime = settings.value("EndTimeUnix", 0).toUInt();
		if (endTime != 0) {
			task.setEnd(QDateTime::fromTime_t(settings.value("EndTimeUnix").toInt()));
		}
		

		int tagCount = settings.beginReadArray("Tags");
		for (int j=0; j<tagCount; j++) {
			settings.setArrayIndex(j);
			QString tag = settings.value("Tag").toString();
			task.addTag(tag);
		}
		settings.endArray();

		tasks.push_back(task);
	}
	settings.endArray();

	LOG(INFO) << "File loaded.";
}

// This function deserialize the data from memory and writes it to the text
// file. If the file cannot be written, an ExceptionNotOpen is thrown.
void Storage::saveFile() {
	LOG(INFO) << "Saving file...";

	QSettings settings(path, QSettings::IniFormat);
	settings.beginWriteArray("Tasks");
	for (int i=0; i<tasks.size(); i++) {
		settings.setArrayIndex(i);
		settings.setValue("Description", tasks[i].getDescription());
		settings.setValue("BeginTime", tasks[i].getBegin().toString());
		settings.setValue("EndTime", tasks[i].getEnd().toString());

		if (tasks[i].getBegin().isNull() || !tasks[i].getBegin().isValid()) {
			settings.setValue("BeginTimeUnix", "");
		} else {
			settings.setValue("BeginTimeUnix", tasks[i].getBegin().toTime_t());
		}

		if (tasks[i].getEnd().isNull() || !tasks[i].getEnd().isValid()) {
			settings.setValue("EndTimeUnix", "");
		} else {
			settings.setValue("EndTimeUnix", tasks[i].getEnd().toTime_t());
		}

		settings.setValue("Done", tasks[i].isDone());

		settings.beginWriteArray("Tags");
		QList<QString> tags = tasks[i].getTags();
		for (int j=0; j<tags.size(); j++) {
			settings.setArrayIndex(j);
			settings.setValue("Tag", tags[j]);
		}
		settings.endArray();
	}
	settings.endArray();

	LOG(INFO) << "File saved.";
}