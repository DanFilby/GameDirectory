#pragma once
//builder for video game has a studio step so can setup multiple studios

#include "DatabaseMaster.h"

class EntryBuilder {

public:
	EntryBuilder(shared_ptr<EntryDatabase> _entryDatabase);

	virtual void ClearBuild();

	void EditEntry(Entry& _entry);
	void EditEntry(ENTRYID _entryId);

	virtual bool BuildEntry(shared_ptr<Entry> & entry);
	virtual bool BuildAndSaveEntry(shared_ptr<Entry>& entry);

	void SetInfo(EntryInfo_Short info);
	void SetNameYear(string _name, uint16_t year);
	void SetName(string _name);
	void SetYear(uint16_t _year);
	void SetValidId();

protected:
	/// <summary>
	/// checks all required fields for an entry
	/// </summary>
	/// <returns> true if rfequired fields are valid </returns>
	virtual bool RequiredFieldsCheck();

	shared_ptr<EntryDatabase> mEntryDatabase;

	//temp entry which is built through wrapper functions
	Entry* mCurrentEntry;
};

class GameEntryBuilder : public EntryBuilder {

public:
	GameEntryBuilder(shared_ptr<DatabaseMaster> databases);

	void ClearBuild();

	void EditGameEntry(GameEntry& _gameEntry);
	void EditGameEntry(ENTRYID _gameEntryId);

	bool BuildGameEntry(shared_ptr<GameEntry>& gameEntry);
	bool BuildAndSaveGameEntry(shared_ptr<GameEntry>& gameEntry);

	void SetShortDescription(string _shortDescription);
	void SetFullDescription(string _fullDescription);

	void SetTags(GameTags _tags);
	void SetTags(vector<string> _tags);

	void SetGenres(GameGenres _genres);
	void SetGenres(vector<string> _genres);

	void SetRatings(GameRatings _ratings);

	void SetDevStudio(EntryRelations _studios);
	void SetDevProducers(EntryRelations _producers);

	void AddGenre(string _genre, bool addToDatabase = false);
	void AddTag(string _tag, bool addToDatabase = false);
	void AddDevStudio(ENTRYID _studioId);
	void AddDevProducer(ENTRYID _producerId);

private:
	bool RequiredFieldsCheck();

	shared_ptr<DatabaseMaster> mDatabases;

	GameEntry* mCurrentGameEntry;

};


//TODO: edit exsisting entry
//TODO: maybe change to error handling like open-gl instead of returning bool on every funtion