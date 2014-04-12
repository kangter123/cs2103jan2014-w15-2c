#include <QDataStream>
#include <cassert>
#include "Constants.h"
#include "Exceptions.h"
#include "Task.h"

//@author A0096863M
Task::Task() {
	id = -1;
	done = false;
}

Task::Task(QString _description) {
	id = -1;
	done = false;
	this->description = _description;
}

Task::~Task() {

}

void Task::setDescription(QString _description) {
	description = _description;
}

QString Task::getDescription() const {
	return description;
}

// Adds a single _tag QString to the list of tags that
// this task has. This method will throw an ExceptionTooManyTags
// if the incoming tag causes the number of tags to exceed
// MAXIMUM_TAGS.
void Task::addTag(QString _tag) {
	assert(!_tag.isEmpty());

	if (tags.size() >= MAXIMUM_TAGS) {
		throw ExceptionTooManyTags();
	}
	
	if (!tags.contains(_tag)) {
		tags.insert(_tag);
	}
}

bool Task::removeTag(QString _tag) {
	return tags.remove(_tag);
}

QList<QString> Task::getTags() const {
	return tags.toList();
}

QSet<QString> Task::getTagsSet() const {
	return tags;
}

// Sets the begin date and time for this task.
// It is the responsibility of the caller of this method
// to pass in a QDateTime object that is complete and valid,
// as this method makes no assumptions about the date and time.
void Task::setBegin(QDateTime _begin) {
	begin = _begin;
}

// Changes only the date portion of the begin QDateTime field.
// If the resulting begin QDateTime object is invalid because of
// missing time field, this method will attempt to validate
// by appending the time as 0000 hours.
void Task::setBeginDate(QDate _beginDate) {
	begin.setDate(_beginDate);

	if (!begin.isValid() || !begin.time().isValid()) {
		begin.setTime(QTime(0, 0, 0));
	}
}

// Changes only the time portion of the begin QDateTime field.
// If the resulting begin QDateTime object is invalid because of
// missing date field, this method will attempt to validate
// by prepending the date as the current day.
void Task::setBeginTime(QTime _beginTime) {
	begin.setTime(_beginTime);

	if (!begin.isValid() || !begin.date().isValid()) {
		begin.setDate(QDate::currentDate());
	}
}

QDateTime Task::getBegin() const {
	return begin;
}

// Sets the end date and time for this task.
// It is the responsibility of the caller of this method
// to pass in a QDateTime object that is complete and valid,
// as this method makes no assumptions about the date and time.
void Task::setEnd(QDateTime _end) {
	end = _end;
}

// Changes only the date portion of the end QDateTime field.
// If the resulting end QDateTime object is invalid because of
// missing time field, this method will attempt to validate
// by appending the time as 2359 hours.
void Task::setEndDate(QDate _endDate) {
	end.setDate(_endDate);

	if (!end.isValid() || !end.time().isValid()) {
		end.setTime(QTime(23, 59, 59));
	}
}

// Changes only the time portion of the end QDateTime field.
// If the resulting end QDateTime object is invalid because of
// missing date field, this method will attempt to validate
// by prepending the date as the current day.
void Task::setEndTime(QTime _endTime) {
	end.setTime(_endTime);

	if (!end.isValid() || !end.date().isValid()) {
		end.setDate(QDate::currentDate());
	}
}

QDateTime Task::getEnd() const {
	return end;
}

// Returns the QString representation of a countdown from now to the end of a task.
// This method also returns a countup from the end of an overdue task to now.
// This method does NOT guarantee accuracy of dates. This method assumes that
// there are 12 identical months in a year, and there are exactly 4 weeks every month,
// and each month has exactly 30 days.
QString Task::getTimeDifferenceString() const {
	qint64 now = QDateTime::currentDateTime().toMSecsSinceEpoch();
	qint64 end = getEnd().toMSecsSinceEpoch();
	qint64 delta = end - now;
	bool isNegative = delta < 0;

	// If date is in the past, set it to positive.
	if (isNegative) {
		delta *= -1;
	}

	delta /= MSECS_IN_SECOND;					// Get rid of milliseconds.

	int seconds = delta % SECONDS_IN_MINUTE;	// Obtain number of seconds.
	delta /= SECONDS_IN_MINUTE;					// Get rid of seconds.

	int minutes = delta % MINUTES_IN_HOUR;		// Obtain number of minutes.
	delta /= MINUTES_IN_HOUR;					// Get rid of minutes.

	int hours = delta % HOURS_IN_DAY;			// Obtain number of hours.
	delta /= HOURS_IN_DAY;						// Get rid of hours.

	int days = delta % DAYS_IN_WEEK;			// Obtain number of days.
	delta /= DAYS_IN_WEEK;						// Get rid of days.

	int weeks = delta % WEEKS_IN_MONTH;			// Obtain number of weeks.
	delta /= WEEKS_IN_MONTH;					// Get rid of weeks.

	int months = delta % MONTHS_IN_YEAR;		// Obtain number of months.
	delta /= MONTHS_IN_YEAR;					// Get rid of months.

	int years = delta;							// Obtain number of years.

	QString result = "";
	if (years > 0) {
		if (years == 1) {
			result.append("1 year");
		} else { // Plural
			QString yr = QString("%1 years").arg(years);
			result.append(yr);
		}
		result.append(", ");
	}

	if (months > 0) {
		if (months == 1) {
			result.append("1 month");
		} else { // Plural
			QString mth = QString("%1 months").arg(months);
			result.append(mth);
		}
		result.append(", ");
	}

	if (weeks > 0) {
		if (weeks == 1) {
			result.append("1 week");
		} else { // Plural
			QString wks = QString("%1 weeks").arg(weeks);
			result.append(wks);
		}
		result.append(", ");
	}

	if (days > 0) {
		if (days == 1) {
			result.append("1 day");
		} else { //  Plural
			QString dys = QString("%1 days").arg(days);
			result.append(dys);
		}
		result.append(", ");
	}

	if (hours > 0) {
		if (hours == 1) {
			result.append("1 hour");
		} else { // Plural
			QString hrs = QString("%1 hours").arg(hours);
			result.append(hrs);
		}
		result.append(", ");
	}

	if (minutes > 0) {
		if (minutes == 1) {
			result.append("1 minute");
		} else { // Plural
			QString min = QString("%1 minutes").arg(minutes);
			result.append(min);
		}
	}

	// Finally, add closing statement.
	if (isNegative) {
		result.append(" ago.");
	} else {
		result.append(" from now.");
	}

	return result;
}

// Directly writes a boolean value to the 'done' field of a task.
void Task::setDone(bool _done) {
	done = _done;
}

void Task::markDone() {
	done = true;
}

void Task::markUndone() {
	done = false;
}

bool Task::isDone() const {
	return done;
}

void Task::setId(int _id) {
	id = _id;
}

int Task::getId() const {
	return id;
}

// Returns TRUE if task has neither a valid begin date/time, 
// nor a valid end date/time.
// Returns FALSE for all other cases.
bool Task::isFloating() const {
	if (!begin.isValid() && !end.isValid()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if there is no end date/time for this task,
// or it is not valid.
// Returns FALSE if end date/time for this task is later
// than current date/time.
// Returns TRUE if end date/time for this task is earlier
// than current date/time.
bool Task::isOverdue() const {
	if (end.isNull() || !end.isValid()) {
		return false;
	}
	if (end < QDateTime::currentDateTime()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if there is no begin date/time for this task,
// or it is not valid.
// Returns FALSE if start date/time for this task is later
// than current date/time.
// Returns FALSE if task is already overdue.
// Returns TRUE if start date/time for this task is earlier
// than current date.time.
bool Task::isOngoing() const {
	if (begin.isNull() || !begin.isValid()) {
		return false;
	}
	if (isOverdue()) {
		return false;
	}
	if (begin < QDateTime::currentDateTime()) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if task has no valid end date.
// Returns TRUE if task is already overdue.
// Returns TRUE if task has an end time within specified date.
bool Task::isDueOn(QDate _date) const {
	QDateTime dateStart(_date, QTime(0, 0, 0));
	QDateTime dateEnd(_date, QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		return true;
	}

	if ((getEnd() <= dateEnd) && (getEnd() >= dateStart)) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if this task has no valid end date.
// Returns FALSE if this task is already overdue.
// Returns FALSE if this task has a due date that is not the current day, 2359hrs.
// Returns TRUE if this task is already overdue but the due date is the current day.
// Returns TRUE if this task has a due date that is the current day, 2359hrs.
bool Task::isDueToday() const {
	QDateTime todayStart(QDateTime::currentDateTime().date(), QTime(0, 0, 0));
	QDateTime todayEnd(QDateTime::currentDateTime().date(), QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		if (getEnd() >= todayStart) {
			return true;
		} else {
			return false;
		}
	}

	if (getEnd() <= todayEnd) {
		return true;
	} else {
		return false;
	}
}

// Returns FALSE if this task has no valid end date.
// Returns FALSE if this task is already overdue.
// Returns FALSE if this task has a due date that is not the next day, 2359hrs.
// Returns TRUE if this task has a due date that is the next day, 2359hrs.
bool Task::isDueTomorrow() const {
	QDateTime tomorrowStart(QDateTime::currentDateTime().date().addDays(1), QTime(0, 0, 0));
	QDateTime tomorrowEnd(QDateTime::currentDateTime().date().addDays(1), QTime(23, 59, 59));

	if (!getEnd().isValid()) {
		return false;
	}

	if (isOverdue()) {
		if (getEnd() >= tomorrowStart) {
			return true;
		} else {
			return false;
		}
	}

	if (getEnd() <= tomorrowEnd) {
		return true;
	} else {
		return false;
	}
}

// An event is defined as a task that has a begin and end date/time.
// This method returns true if a task has both, and false if otherwise.
bool Task::isEvent() const {
	if (begin.isValid() && end.isValid()) {
		return true;
	} else {
		return false;
	}
}

// Returns true if this task is equal to the other task; otherwise returns false.
bool Task::operator==(Task const& other) const {
	bool isSameDescription = (description==other.getDescription());
	bool isSameTags = (tags==other.getTagsSet());
	bool isSameBegin = (begin==other.getBegin());
	bool isSameEnd = (end==other.getEnd());
	bool isSameDone = (done==other.isDone());

	return (isSameDescription && isSameTags && isSameBegin && isSameEnd && isSameDone);
}

// Returns true if this task is different to the other task; otherwise returns false;
bool Task::operator!=(Task const& other) const {
	bool isSameDescription = (description==other.getDescription());
	bool isSameTags = (tags==other.getTagsSet());
	bool isSameBegin = (begin==other.getBegin());
	bool isSameEnd = (end==other.getEnd());
	bool isSameDone = (done==other.isDone());

	return !(isSameDescription && isSameTags && isSameBegin && isSameEnd && isSameDone);
}

QDataStream& operator<<(QDataStream& out, const Task& task) {
	out << task.description;
	QSetIterator<QString> tags(task.tags);
	while (tags.hasNext()) {
		out << tags.next();
	}
	out << task.begin;
	out << task.end;
	out << task.done;

	return out;
}

QDataStream& operator>>(QDataStream& in, Task& task) {
	in >> task.description;
	int numTags = 0;
	in >> numTags;
	for (int i=0; i<numTags; i++) {
		QString tag;
		in >> tag;
		task.tags.insert(tag);
	}
	in >> task.begin;
	in >> task.end;
	in >> task.done;

	return in;
}