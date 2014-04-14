//@author A0096863M
#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace StorageTests {
	QApplication *app;
	StorageStub *storage;

	// Simulate running the main() function
	// Sets up the logging facility and the Qt event loop
	TEST_MODULE_INITIALIZE(ModuleInitialize) {
		int argc = 1;
		char *argv[] = { "Tasuke.exe" };
		FLAGS_logtostderr = true;
		google::InitGoogleLogging(argv[0]);
		app = new QApplication(argc, argv);
		Tasuke::setGuiMode(false);
		Tasuke::instance();
		storage = nullptr;
	}

	// Cleans up what we set up
	TEST_MODULE_CLEANUP(ModuleCleanup) {
		if (storage == nullptr) {
			delete storage;
		}

		app->quit();
		delete app;
	}

	TEST_CLASS(StorageTests) {

	public:

		// Create a new storage object before running any method
		TEST_METHOD_INITIALIZE(init) {
			storage = new StorageStub();
			Tasuke::instance().setStorage(storage);
		}

		TEST_METHOD_CLEANUP(deinit) {
			delete storage;
			storage = nullptr;
		}
		
		/********** Tests for TASK class **********/

		// Should throw an exception because exceed maximum tag count.
		TEST_METHOD(TaskExceedMaximumTagCount) {
			Assert::ExpectException<ExceptionBadCommand>([] {
				Task task;
				for (int i=0; i<MAXIMUM_TAGS+1; i++) {
					task.addTag("tag" + QString::number(i));
				}
			});
		}

		// Should not throw an exception because within maximum tag count.
		TEST_METHOD(TaskWithinMaximumTagCount) {
			Task task;
			for (int i=0; i<MAXIMUM_TAGS; i++) {
				task.addTag("tag" + QString::number(i));
			}
		}

		// Task should NOT be overdue (due date is year 2100, never overdue).
		TEST_METHOD(TaskIsNotOverdue) {
			Task task;
			task.setEnd(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
			Assert::IsFalse(task.isOverdue());
		}

		// Task should be overdue (due date is year 2010, always overdue).
		TEST_METHOD(TaskIsOverdue) {
			Task task;
			task.setEnd(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsTrue(task.isOverdue());
		}

		// Task should never be ongoing (starts in year 2100, never ongoing).
		TEST_METHOD(TaskIsNotOngoing) {
			Task task;
			task.setBegin(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
			Assert::IsFalse(task.isOngoing());
		}

		// Task should always be ongoing (task started in year 2010, always ongoing).
		TEST_METHOD(TaskIsOngoing) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsTrue(task.isOngoing());
		}

		// Task should be ongoing and not overdue (task starts 2010 Jan, ends 2100 Dec).
		TEST_METHOD(TaskIsOngoingBecauseNotOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
			Assert::IsTrue(task.isOngoing());
			Assert::IsFalse(task.isOverdue());
		}

		// Task should not be ongoing, and is overdue (task starts 2010 Jan, ends 2010 Jan).
		TEST_METHOD(TaskIsNotOngoingBecauseOverdue) {
			Task task;
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			task.setEnd(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			Assert::IsFalse(task.isOngoing());
			Assert::IsTrue(task.isOverdue());
		}

		TEST_METHOD(TaskIsFloating) {
			Task task;
			Assert::IsTrue(task.isFloating());
			task.setBegin(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsFalse(task.isFloating());
			task.setEnd(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
			Assert::IsFalse(task.isFloating());
		}

		// Tasks that are already overdue should return false.
		TEST_METHOD(TaskIsDueToday) {
			Task dueToday;
			dueToday.setEnd(QDateTime(QDateTime::currentDateTime().date(), QTime(23, 59, 58)));
			Assert::IsTrue(dueToday.isDueToday());

			dueToday.setEnd(QDateTime(QDateTime::currentDateTime().date(), QTime(0, 0, 2)));
			Assert::IsTrue(dueToday.isDueToday());

			Task dueTomorrow;
			dueTomorrow.setEnd(QDateTime(QDateTime::currentDateTime().date().addDays(1), QTime(0, 0, 2)));
			Assert::IsFalse(dueTomorrow.isDueToday());

			Task alreadyOverdue;
			alreadyOverdue.setEnd(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 1)));
			Assert::IsFalse(alreadyOverdue.isDueToday());
		}

		TEST_METHOD(TaskIsDueTomorrow) {
			Task dueToday;
			dueToday.setEnd(QDateTime::currentDateTime());
			Assert::IsFalse(dueToday.isDueTomorrow());
			
			dueToday.setEnd(QDateTime(QDateTime::currentDateTime().date(), QTime(0, 0, 2)));
			Assert::IsFalse(dueToday.isDueTomorrow());

			Task dueTomorrow;
			dueTomorrow.setEnd(QDateTime(QDateTime::currentDateTime().date().addDays(1), QTime(0, 0, 2)));
			Assert::IsTrue(dueTomorrow.isDueTomorrow());
			
			Task alreadyOverdue;
			alreadyOverdue.setEnd(QDateTime(QDate(2010, 1, 1), QTime(1, 1, 2)));
			Assert::IsFalse(alreadyOverdue.isDueTomorrow());
		}

		TEST_METHOD(TaskIsAnEvent) {
			Task noBegin;
			noBegin.setEnd(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			Assert::IsFalse(noBegin.isEvent());

			Task noEnd;
			noEnd.setBegin(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			Assert::IsFalse(noEnd.isEvent());

			Task anEvent;
			anEvent.setBegin(QDateTime(QDate(2010, 1, 2), QTime(1, 1, 1)));
			anEvent.setEnd(QDateTime(QDate(2100, 12, 31), QTime(23, 59, 59)));
			Assert::IsTrue(anEvent.isEvent());
		}

		TEST_METHOD(TaskIsDueOnDate) {
			Task task;
			Assert::IsFalse(task.isDueOn(QDate::currentDate()));

			task.setEnd(QDateTime::currentDateTime());
			Assert::IsTrue(task.isDueOn(QDate::currentDate()));

			task.setEndDate(QDate(2010, 1, 1));
			Assert::IsTrue(task.isDueOn(QDate::currentDate()));

			task.setEndDate(QDate(2100, 12, 31));
			Assert::IsFalse(task.isDueOn(QDate::currentDate()));
		}

		/********** Tests for STORAGE class **********/
		
		TEST_METHOD(StorageSearchByDescription) {
			// Add the test cases
			Tasuke::instance().runCommand("add DESCRIPTION IN UPPERCASE");
			Tasuke::instance().runCommand("add description in lowercase");
			Tasuke::instance().runCommand("add DeScRiPtIoN iN iReGuLaR");
			Tasuke::instance().runCommand("add existing description");

			Assert::AreEqual(storage->searchByDescription("description").size(), 4);
			Assert::AreEqual(storage->searchByDescription("DESCRIPTION").size(), 4);
			Assert::AreEqual(storage->searchByDescription("description", Qt::CaseSensitive).size(), 2);
			Assert::AreEqual(storage->searchByDescription("DeScRiPtIoN", Qt::CaseSensitive).size(), 1);
			Assert::AreEqual(storage->searchByDescription("DeScRiPtIoN").size(), 4);
			Assert::AreEqual(storage->searchByDescription("nonexistent description").size(), 0);
		}
		
		TEST_METHOD(StorageSearchByTag) {
			// Add the test cases
			Tasuke::instance().runCommand("add task1 #tagcase #tag1 #tag3");
			Tasuke::instance().runCommand("add task2 #TAGCASE #tag1");
			Tasuke::instance().runCommand("add task3 #tag3 #tag2 #tag1");
			Tasuke::instance().runCommand("add task4 #tag2");
			Tasuke::instance().runCommand("add task5 #tag4 #tag4 #tag4");

			// Case sensitive tests
			Assert::AreEqual(storage->searchByTag("tagcase", Qt::CaseSensitive).size(), 1);
			Assert::AreEqual(storage->searchByTag("TAGCASE", Qt::CaseSensitive).size(), 1);

			Assert::AreEqual(storage->searchByTag("tag1", Qt::CaseSensitive).size(), 3);
			Assert::AreEqual(storage->searchByTag("tag3", Qt::CaseSensitive).size(), 2);
			Assert::AreEqual(storage->searchByTag("tag4").size(), 1);

			// Case insensitive tests
			Assert::AreEqual(storage->searchByTag("TAGCASE").size(), 2);
			Assert::AreEqual(storage->searchByTag("tagcase").size(), 2);
		}
		
		TEST_METHOD(StorageSortByDescription) {
			QList<Task> correct;

			Task task1("aaaa");
			correct.push_back(task1);
			Task task2("bbbb");
			correct.push_back(task2);
			Task task3("cccc");
			correct.push_back(task3);
			Task task4("xxxx");
			correct.push_back(task4);
			Task task5("zzzz");
			correct.push_back(task5);
			
			Tasuke::instance().runCommand("add zzzz");
			Tasuke::instance().runCommand("add xxxx");
			Tasuke::instance().runCommand("add aaaa");
			Tasuke::instance().runCommand("add cccc");
			Tasuke::instance().runCommand("add bbbb");

			storage->sortByDescription();

			Assert::IsTrue(storage->getTasks() == correct);
		}

		TEST_METHOD(StorageSortByEndDate) {
			QList<Task> correct;

			Task task1("task1"), task2("task2"), task3("task3"), task4("task4"), task5("task5");
			task1.setEnd(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 0)));
			task2.setEnd(QDateTime(QDate(2010, 1, 1), QTime(0, 0, 1)));
			task3.setEnd(QDateTime(QDate(2010, 1, 1), QTime(23, 59, 59)));
			task4.setEnd(QDateTime(QDate(2010, 1, 2), QTime(0, 0, 0)));
			task5.setEnd(QDateTime(QDate(2010, 1, 2), QTime(0, 0, 1)));

			correct.push_back(task1);
			correct.push_back(task2);
			correct.push_back(task3);
			correct.push_back(task4);
			correct.push_back(task5);

			storage->addTask(task5);
			storage->addTask(task4);
			storage->addTask(task3);
			storage->addTask(task2);
			storage->addTask(task1);
			storage->sortByEndDate();

			Assert::IsTrue(storage->getTasks() == correct);
		}
	};
}