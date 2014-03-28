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

// Searches all descriptions of all tasks in memory for specified keyword(s).
// Returns a list of all tasks that contain the keyword in its description.
// Searches by any part of the description. Case insensitive is the default.
QList<Task> IStorage::searchByDescription(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by description keyword: " << keyword.toStdString();
	QList<Task> results;

	int taskCount = tasks.size();

	for (int i=0; i<taskCount; i++) {
		QStringRef description(&tasks[i].getDescription());
		if (description.contains(keyword, caseSensitivity)) {
			results.push_back(tasks[i]);
		}
	}

	return results;
}

// Searches all tags in all tasks in memory for specified tag.
// Returns a list of all tasks that contain that tag.
// Searches by whole tag only. Case sensitive.
QList<Task> IStorage::searchByTag(QString tag) {
	QMutexLocker lock(&mutex);
	LOG(INFO) << "Searching by tag: " << tag.toStdString();
	QList<Task> results;

	int taskCount = tasks.size();

	for (int i=0; i<taskCount; i++) {
		QList<QString> tags = tasks[i].getTags();
		int tagCount = tags.size();

		for (int j=0; j<tagCount; j++) {
			if (tags[j] == tag) {
				results.push_back(tasks[i]);
			}
		}
	}

	return results;
}

void IStorage::sortByEndDate() {
	LOG(INFO) << "Sorting by end date.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getEnd() < t2.getEnd();
	});
}

void IStorage::sortByBeginDate() {
	LOG(INFO) << "Sorting by begin date.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getBegin() < t2.getBegin();
	});
}

void IStorage::sortByDescription() {
	LOG(INFO) << "Sorting by description.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getDescription().toLower() < t2.getDescription().toLower();
	});
}

void IStorage::sortByDone() {
	LOG(INFO) << "Sorting by done status.";
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.isDone() < t2.isDone();
	});
}

void IStorage::clearAllDone() {
	LOG(INFO) << "Clearing all tasks marked as done.";
	foreach (const Task& task, tasks) {
		if (task.isDone()) {
			tasks.removeOne(task);
		}
	}
}

void IStorage::clearAllTasks() {
	LOG(INFO) << "Clearing all tasks with great justice.";
	tasks.clear();
}

// Constructor for Storage.
Storage::Storage() {
	LOG(INFO) << "Storage instance created...";

	//path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/tasuke.ini";

	qRegisterMetaType<Task>("Task");
	qRegisterMetaTypeStreamOperators<Task>("Task");	
}

// This function loads the contents of the text file and serialize it into
// the memory. If there is no such file, this function does nothing.
void Storage::loadFile() {
	LOG(INFO) << "Loading file...";

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");

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
		
		task.setDone(settings.value("Done").toBool());

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

	sortByDescription();
	sortByEndDate();
	sortByDone();

	//QSettings settings(path, QSettings::IniFormat);
	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "Tasuke", "Tasuke");
	settings.clear();
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