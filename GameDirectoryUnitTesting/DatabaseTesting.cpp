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
			shared_ptr<EntryDatabase> originalDatabase = dbmaster.GetEntryDatabase();
			EntryDatabase testDatabase = EntryDatabase(*originalDatabase.get());

			Entry testEntry = Entry(399, ET_Game, 2004, "Mario");
			Entry testEntry2 = Entry(398, ET_Game, 2005, "Super-Mario");
			Entry testEntry3 = Entry(397, ET_Game, 2006, "Super-MarioBros");

			testDatabase.AddEntry(testEntry);
			testDatabase.AddEntry(testEntry2);
			testDatabase.AddEntry(testEntry3);

			//reset changes to file
			originalDatabase->UpdateEntriesFile();

			Assert::AreEqual(originalDatabase->GetEntryCount() + 3, testDatabase.GetEntryCount());
		}
		TEST_METHOD(RemovingEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> originalDatabase = dbmaster.GetEntryDatabase();

			EntryDatabase testDatabase = EntryDatabase(*originalDatabase.get());
			Entry testEntry = Entry(396, ET_Game, 2004, "Mario");
			Entry testEntry2 = Entry(395, ET_Game, 2005, "Super-Mario");

			//try to add and remove entries
			testDatabase.AddEntry(testEntry);
			testDatabase.AddEntry(testEntry2);
			testDatabase.RemoveEntry(testEntry);
			testDatabase.RemoveEntry(testEntry2);
			
			//reset changes to file
			originalDatabase->UpdateEntriesFile();

			Assert::AreEqual(originalDatabase->GetEntryCount(), testDatabase.GetEntryCount());
		}		
		TEST_METHOD(WritingEntries) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> originalDatabase = dbmaster.GetEntryDatabase();

			EntryDatabase testDatabase = EntryDatabase(*originalDatabase.get());
			Entry testEntry = Entry(394, ET_Game, 2004, "Mario");
			testDatabase.AddEntry(testEntry);

			EntryDatabase latestDatabase{};

			CheckEntrySummaryProperties(latestDatabase.GetEntrySummary(394), 394, ET_Game, 2004, "Mario");
			Assert::AreEqual(originalDatabase->GetEntryCount() + 1, testDatabase.GetEntryCount());

			originalDatabase->UpdateEntriesFile();
		}
		TEST_METHOD(AddingDuplicates) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<EntryDatabase> originalDatabase = dbmaster.GetEntryDatabase();

			EntryDatabase testDatabase = EntryDatabase(*originalDatabase.get());
			Entry testEntry = Entry(393, ET_Game, 2004, "Mario");

			//try to add duplicate entries
			testDatabase.AddEntry(testEntry);
			testDatabase.AddEntry(testEntry);
			testDatabase.AddEntry(testEntry);

			//reset changes to file
			originalDatabase->UpdateEntriesFile();

			//only one entry should be added
			Assert::AreEqual(originalDatabase->GetEntryCount() + 1, testDatabase.GetEntryCount());
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

	TEST_CLASS(GenreDatabaseTests) {
	public:
		TEST_METHOD(Startup) 
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();
			Assert::IsTrue(genreDatabase->GetAllGenres().size() > 0);
		}
		TEST_METHOD(AddingGenres)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();

			string testGenre = "Test-Genre";
			genreDatabase->AddGenre(testGenre);

			Assert::IsTrue(genreDatabase->GenreExsists(testGenre));

			genreDatabase->RemoveGenre(testGenre);
		}
		TEST_METHOD(RemovingGenres)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();

			string testGenre = "Test-Genre";
			genreDatabase->AddGenre(testGenre);
			Assert::IsTrue(genreDatabase->GenreExsists(testGenre));

			genreDatabase->RemoveGenre(testGenre);
			Assert::IsFalse(genreDatabase->GenreExsists(testGenre));
		}
		TEST_METHOD(WritingGenreFile)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();

			string testGenre = "Test-Genre";
			genreDatabase->AddGenre(testGenre);
			genreDatabase->UpdateGenreListFile();

			DatabaseMaster dbmaster2{}; dbmaster2.AppInit();
			genreDatabase = dbmaster2.GetGenreDatabase();

			Assert::IsTrue(genreDatabase->GenreExsists(testGenre));

			genreDatabase->RemoveGenre(testGenre);
			genreDatabase->UpdateGenreListFile();
		}
		TEST_METHOD(GettingGenres) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();

			string testGenre = "Test-Genre";
			genreDatabase->AddGenre(testGenre);

			uint8_t genreKey = genreDatabase->GetKey(testGenre);
			Assert::AreEqual(testGenre, genreDatabase->GetGenre(genreKey));

			genreDatabase->RemoveGenre(testGenre);
		}
		TEST_METHOD(GettingInlavidGenres) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();

			uint8_t genreKey = genreDatabase->GetKey("testinginvalidgenrenametoolongfordatbaseandshouldntexsist");
			Assert::AreEqual(string(), genreDatabase->GetGenre(genreKey));
			Assert::AreEqual((uint8_t)0, genreKey);
		}
		TEST_METHOD(AddingDuplicateGenres) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> genreDatabase = dbmaster.GetGenreDatabase();
			int genreStartCount = genreDatabase->GetAllGenres().size();

			string testGenre = "Test-Genre";
			genreDatabase->AddGenre(testGenre);
			genreDatabase->AddGenre(testGenre);
			genreDatabase->AddGenre(testGenre);

			Assert::AreEqual(genreStartCount + 1, (int)genreDatabase->GetAllGenres().size());

			genreDatabase->RemoveGenre(testGenre);
		}
		TEST_METHOD(MaxGenreCount) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<GenreListDataBase> originalGenreDatabase = dbmaster.GetGenreDatabase();

			//use a test database to not make permanent changes
			GenreListDataBase testDataBase = GenreListDataBase(*originalGenreDatabase.get());
			int genreCount = testDataBase.GetAllGenres().size();

			for (size_t i = 0; i < GenreListDataBase::MAXCOUNT_GENRE * 2; i++)
			{
				//generate a new string each iteration
				char testGenre[6] = { 'a', 'a', ('a' + (i % 26)) ,('a' + ((i / 26)) % 26), ('a' + ((i / (26 * 26)) % 26)), '\0' };
				testDataBase.AddGenre(string(testGenre));
			}

			Assert::AreEqual((int)GenreListDataBase::MAXCOUNT_GENRE, (int)testDataBase.GetAllGenres().size());

			originalGenreDatabase->UpdateGenreListFile();
		}
		
	};

	TEST_CLASS(TagDatabaseTests) {
	public:
		TEST_METHOD(Startup)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();
			Assert::IsTrue(tagDatabase->GetAllTags().size() > 0);
		}
		TEST_METHOD(AddingTags)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();

			string testTag = "Test-Tag";
			tagDatabase->AddTag(testTag);

			Assert::IsTrue(tagDatabase->TagExsists(testTag));

			tagDatabase->RemoveTag(testTag);
		}
		TEST_METHOD(RemovingTags)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();

			string testTag = "Test-Tag";
			tagDatabase->AddTag(testTag);
			Assert::IsTrue(tagDatabase->TagExsists(testTag));

			tagDatabase->RemoveTag(testTag);
			Assert::IsFalse(tagDatabase->TagExsists(testTag));
		}
		TEST_METHOD(WritingTagFile)
		{
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();

			string testTag = "Test-Tag";
			tagDatabase->AddTag(testTag);
			tagDatabase->UpdateTagListFile();

			DatabaseMaster dbmaster2{}; dbmaster2.AppInit();
			tagDatabase = dbmaster2.GetTagDatabase();

			Assert::IsTrue(tagDatabase->TagExsists(testTag));

			tagDatabase->RemoveTag(testTag);
			tagDatabase->UpdateTagListFile();
		}
		TEST_METHOD(GettingTags) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();

			string testTag = "Test-Tag";
			tagDatabase->AddTag(testTag);

			uint8_t tagKey = tagDatabase->GetKey(testTag);
			Assert::AreEqual(testTag, tagDatabase->GetTag(tagKey));

			tagDatabase->RemoveTag(testTag);
		}
		TEST_METHOD(GettingInlavidTags) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();

			uint8_t tagKey = tagDatabase->GetKey("testinginvalidtagnametoolongfordatbaseandshouldntexsist");
			Assert::AreEqual(string(), tagDatabase->GetTag(tagKey));
			Assert::AreEqual((uint8_t)0, tagKey);
		}
		TEST_METHOD(AddingDuplicateTags) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> tagDatabase = dbmaster.GetTagDatabase();
			int tagStartCount = tagDatabase->GetAllTags().size();

			string testTag = "Test-Tag";
			tagDatabase->AddTag(testTag);
			tagDatabase->AddTag(testTag);
			tagDatabase->AddTag(testTag);

			Assert::AreEqual(tagStartCount + 1, (int)tagDatabase->GetAllTags().size());

			tagDatabase->RemoveTag(testTag);
		}
		TEST_METHOD(MaxTagCount) {
			DatabaseMaster dbmaster{}; dbmaster.AppInit();
			shared_ptr<TagListDataBase> originalTagDatabase = dbmaster.GetTagDatabase();

			//use a test database to not make permanent changes
			TagListDataBase testDataBase = TagListDataBase(*originalTagDatabase.get());

			for (size_t i = 0; i < TagListDataBase::MAXCOUNT_TAGS * 2; i++)
			{
				//generate a new string each iteration
				char testTag[6] = { 'a', 'a', ('a' + (i % 26)) ,('a' + ((i / 26)) % 26), ('a' + ((i / (26 * 26)) % 26)), '\0' };
				testDataBase.AddTag(string(testTag));
			}

			Assert::AreEqual((int)TagListDataBase::MAXCOUNT_TAGS, (int)testDataBase.GetAllTags().size());

			originalTagDatabase->UpdateTagListFile();
		}

	};
}
