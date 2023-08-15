#include "Common.h"
#include "EntryDatabase.h"
#include "EntryBuilders.h"
#include "DatabaseMaster.h"

using std::cout;

//TODO: studios
//TODO: Storing game entries
//TODO: entry's link struct storing linking ids ie games to studios 
// 
//each entry get it's own file, need store path with entry database, use folder tree
//1st: folders games, studios etc
//2nd: folders Dir00000-Dir65536, Dir00001, Dir00002, Dir00003, etc
//3rd: 255 entries in each folder Game001, Studio245, in their own folder

shared_ptr<DatabaseMaster> appDbManager;

void TestTempGameEntryReadWrite(shared_ptr<EntryDatabase>dataBase, shared_ptr<TagListDataBase> tagDB, shared_ptr<GenreListDataBase> gdatabase) {
    GameGenres genres(gdatabase);
    genres.AddGenre(gdatabase->GetKey("Strategy"));

    GameTags tags(tagDB);
    tags.AddTag(tagDB->GetKey("Favourites"));

    GameRatings ratings(8.0f, 9.0f, 5.5f, 3.1f);


    GameEntryBuilder gameBuilder(appDbManager);

    gameBuilder.SetNameYear("Dan's Test Game 78", 2023);
    gameBuilder.SetShortDescription("Action adventure rpg as a penguin");
    gameBuilder.SetFullDescription("Explore a quaint iceberg village, full of interesting villagers. whilst building their small town and completing their quests. ");
    gameBuilder.SetRatings(ratings);
    gameBuilder.SetTags(tags);
    gameBuilder.SetGenres(genres);

    gameBuilder.AddTag("Dan's-Game", true);

    EntryRelations rel1(vector<ENTRYID>{125, 250, 375}, Relation_toStudios);
    EntryRelations rel2(vector<ENTRYID>{50, 100, 150}, Relation_toProducers);

    gameBuilder.SetDevStudios(rel1);
    gameBuilder.SetDevProducers(rel2);

    shared_ptr<Entry> entry;
    if (gameBuilder.BuildEntry(entry)) {
        cout << "Successfully built entry:\n"
            << "Id: " << entry.get()->Id() << "\n"
            << "Name: " << entry.get()->Name() << "\n"
            << "Type: " << entry.get()->Type() << "\n"
            << "Year: " << entry.get()->Year() << "\n\n";
    }

    gameBuilder.SetValidId();
    shared_ptr<GameEntry> gameEntry;

    if (gameBuilder.BuildGameEntry(gameEntry)) {
        gameEntry->PrintInfo();
    }

    dataBase->AddEntry<GameEntry>(gameEntry);

    Entry_Editor<GameEntry> geEditor = dataBase->EditEntry<GameEntry>(gameEntry->Id());

    geEditor.entry->SetShortDescription("Changed this desciption");
    geEditor.entry->PrintInfo();
    geEditor.UpdateDatabase();

    shared_ptr<GameEntry> ge2 = dataBase->GetGameEntry(gameEntry->Id());
    ge2->PrintInfo();

    dataBase->RemoveEntry(gameEntry->Id());

    StudioEntryBuilder studioEntryBuilder(appDbManager);

    Person dan("Daniel Filby", SimpleDate(1970, 1));
    StudioExecutives execs(dan, vector<Person>{Person("Natela"), Person("Martin"), Person("Alex")});

    studioEntryBuilder.SetNameYear("FalmouthGroup11", 2023);
    studioEntryBuilder.SetDescription("A small studio based in falmouth. Consisting of five elite members");
    studioEntryBuilder.SetStudioExecutives(execs);
    studioEntryBuilder.SetNumGamesReleased(1);
    studioEntryBuilder.SetValidId();

    shared_ptr<StudioEntry> studioEntry;
    if (studioEntryBuilder.BuildStudioEntry(studioEntry)) {
        studioEntry->PrintInfo();
    }

    dataBase->AddEntry<StudioEntry>(studioEntry);

    Entry_Editor<StudioEntry> seEditor = dataBase->EditEntry<StudioEntry>(studioEntry->Id());

    seEditor.entry->mNumGamesReleased = 2;
    seEditor.UpdateDatabase();

    shared_ptr<StudioEntry> se2 = dataBase->GetStudioEntry(studioEntry->Id());
    se2->PrintInfo();

    dataBase->RemoveEntry<StudioEntry>(studioEntry);
}


void DataBase() {
     shared_ptr<EntryDatabase>dataBase = appDbManager->GetEntryDatabase();
     shared_ptr<GenreListDataBase> gdatabase = appDbManager->GetGenreDatabase();
     shared_ptr<TagListDataBase> tagDB = appDbManager->GetTagDatabase();
     dataBase->UpdateEntriesFile();

     dataBase->PrintActiveEntries();
     dataBase->PrintEntriesPaths();

     gdatabase->PrintGenreList();
     tagDB->PrintTagList();

     TestTempGameEntryReadWrite(dataBase, tagDB, gdatabase);         
     
}

void AppStart() {
    //inits directories for entire app
    appDbManager = make_shared<DatabaseMaster>();
    appDbManager->AppInit();
}

void AppClose() {
    cout << "\n";
    appDbManager->Close();
}


int main()
{
    AppStart();
    DataBase();

    GameRatings ratings(8.0f,9.0f,5.5f,3.1f);

    unique_ptr<char[]> ratingsBinDat = ratings.ToBinary();

    GameRatings ratings2(ratingsBinDat.get());

    //ratings2.DisplayAllRatings();

    IncomeStatement_FQ incomeStatement{ FQDate(2008,Q1), 100,100,100 };

    auto binDAtaa = incomeStatement.ToBinary();

    IncomeStatement_FQ incomeStatement2{ binDAtaa.get() };

    incomeStatement2.PrintSummary();

    AppClose();
}