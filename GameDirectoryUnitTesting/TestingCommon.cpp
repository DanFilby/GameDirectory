#include "pch.h"
#include "TestingCommon.h"

Entry CreateStandardEntry()
{
    Entry e(100, ET_Game, 2023, "Penguin Village");

    return e;
}

void CheckEntryProperties(Entry& entry, ENTRYID id, EntryType type, uint16_t year, string name)
{
	Assert::AreEqual((ENTRYID)id, entry.Id());
	Assert::AreEqual((uint16_t)year, entry.Year());
	Assert::AreEqual((string)name, entry.Name());
	Assert::IsTrue(type == entry.Type());	
}

void CheckEntrySummaryProperties(EntryInfo_Short& entrySum, ENTRYID id, EntryType type, uint16_t year, string name)
{
	Assert::AreEqual((ENTRYID)id, entrySum.id);
	Assert::AreEqual((uint16_t)year, entrySum.year);
	Assert::AreEqual((string)name, (string)entrySum.name);
	Assert::IsTrue(type == entrySum.type);
}
