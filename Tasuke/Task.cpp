#include <QDataStream>
#include <string>
#include "Task.h"

Task::Task() {

}

Task::~Task() {

}

void Task::setDescription(std::string& _description) {
	description = QString(_description.c_str());
}

std::string Task::getDescription() const {
	return description.toStdString();
}

void Task::addTag(std::string& tag) {
	tags.push_back(QString(tag.c_str()));
}

void Task::removeTag(std::string& tag) {
	tags.removeOne(QString(tag.c_str()));
}

void Task::setBegin(QDateTime& _begin) {
	begin = _begin;
}

QDateTime Task::getBegin() const {
	return begin;
}

void Task::setEnd(QDateTime& _end) {
	end = _end;
}

QDateTime Task::getEnd() const {
	return end;
}

QDataStream& operator<<(QDataStream& out, const Task& task) {
	out << task.description;
	out << task.tags.size();
	for (int i=0; i<task.tags.size(); i++) {
		out << task.tags[i];
	}
	out << task.begin;
	out << task.end;

	return out;
}

QDataStream& operator>>(QDataStream& in, Task& task) {
	in >> task.description;
	int numTags = 0;
	in >> numTags;
	for (int i=0; i<numTags; i++) {
		QString tag;
		in >> tag;
		task.tags.push_back(tag);
	}
	in >> task.begin;
	in >> task.end;

	return in;
}