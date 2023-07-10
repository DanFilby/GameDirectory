#include "pch.h"
#include "CppUnitTest.h"

#include "../GameDirectory/DatabaseMaster.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DatabaseTesting
{
	TEST_CLASS(DatabaseTests)
	{
	public:
		TEST_METHOD(EntryDatabase_Startup)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> entryDatabase = dbmaster.GetEntryDatabase();

			Assert::IsTrue(entryDatabase->GetEntryCount() > 0);
		}
		TEST_METHOD(EntryDatabase_FileHeader)
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
		TEST_METHOD(EntryDatabase_FileHeaderSerialization)
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

	};
}
