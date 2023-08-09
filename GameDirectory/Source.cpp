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

    gameBuilder.SetNameYear("Dan's Test Game 63", 2023);
    gameBuilder.SetShortDescription("Action adventure rpg as a penguin");
    gameBuilder.SetFullDescription("Explore a quaint iceberg village, full of interesting villagers. whilst building their small town and completing their quests. ");
    gameBuilder.SetRatings(ratings);
    gameBuilder.SetTags(tags);
    gameBuilder.SetGenres(genres);

    gameBuilder.AddTag("Dan's-Game", true);

    EntryRelations rel1(vector<ENTRYID>{125, 250, 375}, Relation_toStudios);
    EntryRelations rel2(vector<ENTRYID>{50, 100, 150}, Relation_toProducers);

    gameBuilder.SetDevStudio(rel1);
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

    dataBase->AddGameEntry(gameEntry);

    GameEntry ge2 = dataBase->GetGameEntry(gameEntry->Id());

    ge2.PrintInfo();

    dataBase->RemoveGameEntry(gameEntry->Id());

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

    SimpleDate danBday(2002, 1);
    std::cout << "Age: " << danBday.GetAge() << " Date: " << danBday.GetDateAsString() << "\n\n";

    Person dan("Daniel Filby", danBday);
    dan.Print();

    Person p1("Natela");
    Person p2("Martin");
    Person p3("Alex");

    StudioExecutives execs(dan, vector<Person>{p1,p2,p3});
    execs.PrintAllExecs();

    EntryInfo_Short studioinfo(ET_Studio, 2023, "Shady Seals");

    StudioEntry se= StudioEntry();

    se.SetBaseInfo(studioinfo);
    se.mExecutives = execs;
    se.SetDescription("A small studio based in falmouth. consists of five elite members");

    se.PrintInfo();

    auto bindaaat = se.GetBinaryData();

    StudioEntry se2(studioinfo, bindaaat);
    se2.PrintInfo();
    AppClose();
}