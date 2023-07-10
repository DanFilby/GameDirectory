#include "pch.h"
#include "CppUnitTest.h"

#include "../GameDirectory/DatabaseMaster.h"
#include "TestingCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DatabaseTesting
{
	TEST_CLASS(EntryDatabaseTests)
	{
	public:
		TEST_METHOD(Startup)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			Assert::IsTrue(entryDatabase->GetEntryCount() > 0);
		}
		TEST_METHOD(FileHeader)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();
			auto entryTypes = entryDatabase->GetEntryTypeCount();

			//check file header is being correctly converted to binary
			EntryFileHeader fileHeader = EntryFileHeader(entryTypes);

			Assert::AreEqual(fileHeader.baseEntries, (uint16_t)entryTypes[ET_Base]);
			Assert::AreEqual(fileHeader.gameEntries, (uint16_t)entryTypes[ET_Game]);
			Assert::AreEqual(fileHeader.studioEntries, (uint16_t)entryTypes[ET_Studio]);
			Assert::AreEqual((int)fileHeader.totalEntries, entryTypes[ET_Base] + entryTypes[ET_Game] + entryTypes[ET_Studio]);
		}
		TEST_METHOD(FileHeaderSerialization)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();
			auto entryTypes = entryDatabase->GetEntryTypeCount();

			//check file header is being correctly converted to binary
			EntryFileHeader fileHeader = EntryFileHeader(entryTypes);
			shared_ptr<char[]> headerBin = fileHeader.ToBinary();
			
			//reserialize header
			EntryFileHeader fileHeaderRe = EntryFileHeader(headerBin.get());

			Assert::AreEqual(fileHeader.baseEntries, fileHeaderRe.baseEntries);
			Assert::AreEqual(fileHeader.gameEntries, fileHeaderRe.gameEntries);
			Assert::AreEqual(fileHeader.studioEntries, fileHeaderRe.studioEntries);
			Assert::AreEqual(fileHeader.totalEntries, fileHeaderRe.totalEntries);
		}
		TEST_METHOD(AddingEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			int startCount = entryDatabase->GetEntryCount();

			Entry testEntry = Entry(234, ET_Game, 2004, "Mario");
			Entry testEntry2 = Entry(254, ET_Game, 2005, "Super-Mario");
			Entry testEntry3 = Entry(254, ET_Game, 2006, "Super-MarioBros");

			//try to add duplicate entries
			entryDatabase->AddEntry(testEntry);
			entryDatabase->AddEntry(testEntry2);
			entryDatabase->AddEntry(testEntry3);

			//two entries should be added
			Assert::AreEqual(startCount + 3, entryDatabase->GetEntryCount());

			//remove test entries
			entryDatabase->RemoveEntry(testEntry);
			entryDatabase->RemoveEntry(testEntry2);
			entryDatabase->RemoveEntry(testEntry3);
		}
		TEST_METHOD(RemovingEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			int startCount = entryDatabase->GetEntryCount();

			Entry testEntry = Entry(234, ET_Game, 2004, "Mario");
			Entry testEntry2 = Entry(254, ET_Game, 2005, "Super-Mario");

			//try to add duplicate entries
			entryDatabase->AddEntry(testEntry);
			entryDatabase->AddEntry(testEntry2);

			//remove test entries
			entryDatabase->RemoveEntry(testEntry);
			entryDatabase->RemoveEntry(testEntry2);
			Assert::AreEqual(startCount, entryDatabase->GetEntryCount());
		}
		TEST_METHOD(AddingDuplicates) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			int startCount = entryDatabase->GetEntryCount();

			Entry testEntry = Entry(234, ET_Game, 2004, "Mario");

			//try to add duplicate entries
			entryDatabase->AddEntry(testEntry);
			entryDatabase->AddEntry(testEntry);
			entryDatabase->AddEntry(testEntry);

			//only one entry should be added
			Assert::AreEqual(startCount + 1, entryDatabase->GetEntryCount());

			//remove test entry
			entryDatabase->RemoveEntry(testEntry);
			Assert::AreEqual(startCount, entryDatabase->GetEntryCount());
		}
		TEST_METHOD(AddingInvalidEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			int startCount = entryDatabase->GetEntryCount();

			//empty entry, and empty id entry
			Entry testEntry = Entry(0, ET_Base, 0, "");
			Entry testEntry2 = Entry(0, ET_Studio, 1066, "Hastings");

			//try to add duplicate entries
			entryDatabase->AddEntry(testEntry);
			entryDatabase->AddEntry(testEntry2);

			//neither entry should be added
			Assert::AreEqual(startCount, entryDatabase->GetEntryCount());
		}
		TEST_METHOD(GetingEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			ENTRYID testId = 200;
			entryDatabase->AddEntry(Entry(testId, ET_Studio, 2002, "Daniel"));

			auto entryInfoRecived = entryDatabase->GetEntrySummary(testId);		
			CheckEntrySummaryProperties(entryInfoRecived, testId, ET_Studio, 2002, "Daniel");

			entryDatabase->RemoveEntry(testId);
		}
		TEST_METHOD(GetingEntryIds) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			ENTRYID testId = 200;
			entryDatabase->AddEntry(Entry(testId, ET_Studio, 2002, "Daniel"));

			auto entryIdRecieved = entryDatabase->GetEntryId(ET_Studio, "Daniel", 2002);
			Assert::AreEqual(testId, entryIdRecieved);

			entryDatabase->RemoveEntry(testId);
		}
		TEST_METHOD(EntryExsists) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			ENTRYID testId = 200;
			entryDatabase->AddEntry(Entry(testId, ET_Studio, 2002, "Daniel"));

			Assert::IsTrue(entryDatabase->EntryExsists(testId));

			entryDatabase->RemoveEntry(testId);

			Assert::IsFalse(entryDatabase->EntryExsists(testId));
		}
		TEST_METHOD(EntryValidityCheck) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			Entry testEntryValid = Entry(201, ET_Studio, 2002, "Valid");

			Assert::IsTrue(entryDatabase->IsValidEntry(testEntryValid.GetSummary()));

			Entry testEntryInvalid = Entry(0, ET_Base, 0, "invlaid");

			Assert::IsFalse(entryDatabase->IsValidEntry(testEntryInvalid.GetSummary()));
		}
	};
}
