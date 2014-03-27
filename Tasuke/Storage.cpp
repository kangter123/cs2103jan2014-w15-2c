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

void Storage::sortByEndDate() {
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getEnd() < t2.getEnd();
	});
}

void Storage::sortByBeginDate() {
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getBegin() < t2.getBegin();
	});
}

void Storage::sortByDescription() {
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.getDescription().toLower() < t2.getDescription().toLower();
	});
}

void Storage::sortByDone() {
	qStableSort(tasks.begin(), tasks.end(), [](const Task& t1, const Task& t2) {
		return t1.isDone() < t2.isDone();
	});
}

void Storage::clearAllDone() {
	foreach (const Task& task, tasks) {
		if (task.isDone()) {
			tasks.removeOne(task);
		}
	}
}

void Storage::clearAllTasks() {
	tasks.clear();
}


// Searches all descriptions of all tasks in memory for specified keyword(s).
// Returns a list of all tasks that contain the keyword in its description.
// Searches by whole keyword only. Case sensitivity optional.
QList<Task&> Storage::searchByDesc(QString keyword, Qt::CaseSensitivity caseSensitivity) {
	QList<Task&> results;

	int taskCount = tasks.size();

	for (int i=0; i<taskCount; i++) {
		Task& task = tasks[i];
		QStringRef description(&task.getDescription());
		if (description.contains(keyword, caseSensitivity)) {
			results.push_back(task);
		}
	}

	return results;
}

// Searches all tags in all tasks in memory for specified tag.
// Returns a list of all tasks that contain that tag.
// Searches by whole tag only. Case sensitive.
QList<Task&> Storage::searchByTag(QString tag) {
	QList<Task&> results;

	int taskCount = tasks.size();

	for (int i=0; i<taskCount; i++) {
		Task& task = tasks[i];
		QList<QString> tags = task.getTags();
		int tagCount = tags.size();

		for (int j=0; j<tagCount; j++) {
			if (tags[j] == tag) {
				results.push_back(task);
			}
		}
	}

	return results;
}