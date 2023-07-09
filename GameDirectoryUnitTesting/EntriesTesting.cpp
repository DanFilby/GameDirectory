#include "pch.h"
#include "CppUnitTest.h"

#include "TestingCommon.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EntryTesting
{
	/// <summary>
	/// Checks entry's properites against params
	/// </summary>
	static inline void CheckEntryProperties(Entry& entry, ENTRYID id, EntryType type, uint16_t year, string name) {
		Assert::AreEqual((ENTRYID)id, entry.Id());
		Assert::AreEqual((uint16_t)year, entry.Year());
		Assert::AreEqual((string)name, entry.Name());
		Assert::IsTrue(type == entry.Type());
	}

	/// <summary>
	/// check entry summary against params
	/// </summary>
	static inline void CheckEntrySummaryProperties(EntryInfo_Short& entrySum, ENTRYID id, EntryType type, uint16_t year, string name) {
		Assert::AreEqual((ENTRYID)id, entrySum.id);
		Assert::AreEqual((uint16_t)year, entrySum.year);
		Assert::AreEqual((string)name, (string)entrySum.name);
		Assert::IsTrue(type == entrySum.type);
	}


	TEST_CLASS(BaseEntry_Tests)
	{	
	public:		
		TEST_METHOD(Properties_Base)
		{
			Entry entry = Entry(25, ET_Base, 2020, "Dan Filby");
			CheckEntryProperties(entry, 25, ET_Base, 2020, "Dan Filby");		
		}
		TEST_METHOD(Properties_Game)
		{
			Entry entry = Entry(50, ET_Game, 2023, "Penguin Village");
			CheckEntryProperties(entry, 50, ET_Game, 2023, "Penguin Village");
		}
		TEST_METHOD(Properties_Studio)
		{
			Entry entry = Entry(256, ET_Studio, 1996, "Valve");
			CheckEntryProperties(entry, 256, ET_Studio, 1996, "Valve");
		}
		TEST_METHOD(Comparison_DifferentGames)
		{
			Entry penguinVillageEntry = Entry(50, ET_Game, 2023, "Penguin Village");
			Entry clubPenguinEntry = Entry(51, ET_Game, 2005, "Club Penguin");

			//should evalulate as false as no properties match
			Assert::IsFalse(penguinVillageEntry == clubPenguinEntry);
		}
		TEST_METHOD(Comparison_MatchingIds)
		{
			Entry penguinVillageEntry = Entry(50, ET_Game, 2023, "Penguin Village");
			Entry clubPenguinEntry = Entry(50, ET_Game, 2005, "Club Penguin");
			
			//should evalulate as true because id's match
			Assert::IsTrue(penguinVillageEntry == clubPenguinEntry);
		}
		TEST_METHOD(Comparison_MatchingGames)
		{
			Entry penguinVillageEntry = Entry(50, ET_Game, 2023, "Penguin Village");
			Entry penguinVillageDuplicateEntry = Entry(60, ET_Game, 2023, "Penguin Village");

			//should evalulate as true because properties match
			Assert::IsTrue(penguinVillageEntry == penguinVillageDuplicateEntry);
		}
		TEST_METHOD(Comparison_MatchingEntries)
		{
			Entry penguinVillageEntry = Entry(50, ET_Game, 2023, "Penguin Village");
			Entry duplicateEntry = Entry(50, ET_Game, 2023, "Penguin Village");

			//should evalulate as true because id's and properties match
			Assert::IsTrue(penguinVillageEntry == duplicateEntry);
		}
		TEST_METHOD(Copy)
		{
			Entry penguinVillageEntry = Entry(50, ET_Game, 2023, "Penguin Village");
			Entry duplicateEntry = penguinVillageEntry;
			Entry duplicateEntry2 = Entry(penguinVillageEntry);

			CheckEntryProperties(duplicateEntry, 50, ET_Game, 2023, "Penguin Village");
			CheckEntryProperties(duplicateEntry2, 50, ET_Game, 2023, "Penguin Village");
		}
		TEST_METHOD(Summary_Base)
		{
			Entry entry = Entry(25, ET_Base, 2020, "Dan Filby");
			EntryInfo_Short summary = entry.GetSummary();

			CheckEntrySummaryProperties(summary, 25, ET_Base, 2020, "Dan Filby");
		}
		TEST_METHOD(Summary_Game)
		{
			Entry entry = Entry(50, ET_Game, 2023, "Penguin Village");
			EntryInfo_Short summary = entry.GetSummary();

			CheckEntrySummaryProperties(summary, 50, ET_Game, 2023, "Penguin Village");
		}
		TEST_METHOD(Summary_Studio)
		{
			Entry entry = Entry(256, ET_Studio, 1996, "Valve");
			EntryInfo_Short summary = entry.GetSummary();

			CheckEntrySummaryProperties(summary, 256, ET_Studio, 1996, "Valve");
		}
	};

	TEST_CLASS(EntrySummary_Tests) {
		TEST_METHOD(Init) {

		}



		TEST_METHOD(Comaprison) {

		}

	};

	TEST_CLASS(EntriesSerialization_Tests)
	{
	public:
		TEST_METHOD(EntrySummary_Serialization) {
			//get entry summary two ways
			auto entry = make_shared<Entry>(21, ET_Base, 2002, "Daniel");
			EntryInfo_Short summary = entry->GetSummary();

			//serialize into binary data
			unique_ptr<char[]> binDat1 = entry->GetRawData_Short();
			unique_ptr<char[]> binDat2 = summary.ToBinary();

			//reserialize into summaries
			EntryInfo_Short summaryRe1(binDat1.get());
			EntryInfo_Short summaryRe2(binDat2.get());

			//check resiralized summaries against original data
			CheckEntrySummaryProperties(summaryRe1, 21, ET_Base, 2002, "Daniel");
			CheckEntrySummaryProperties(summaryRe2, 21, ET_Base, 2002, "Daniel");
			Assert::IsTrue(summaryRe1 == summaryRe2);
		}
		TEST_METHOD(GameEntrySummary_Serialization) {
			//check differnt methods of getting a summary
			auto gameEntry = make_shared<GameEntry>(50, 2023, "Penguin Village");
			auto entry = dynamic_pointer_cast<Entry>(gameEntry);
			EntryInfo_Short summary = gameEntry->GetSummary();

			//turn summaries into binary data
			unique_ptr<char[]> binDat1 = gameEntry->GetRawData_Short();
			unique_ptr<char[]> binDat2 = entry->GetRawData_Short();
			unique_ptr<char[]> binDat3 = summary.ToBinary();

			//reserialise into summary struct
			EntryInfo_Short summaryRe1(binDat1.get());
			EntryInfo_Short summaryRe2(binDat2.get());
			EntryInfo_Short summaryRe3(binDat3.get());

			//check resiralised data against original input
			CheckEntrySummaryProperties(summaryRe1, 50, ET_Game, 2023, "Penguin Village");
			CheckEntrySummaryProperties(summaryRe2, 50, ET_Game, 2023, "Penguin Village");
			CheckEntrySummaryProperties(summaryRe3, 50, ET_Game, 2023, "Penguin Village");
			Assert::IsTrue(summaryRe3 == summaryRe1 && summaryRe3 == summaryRe2);
		}

	};

}
