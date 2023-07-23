#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"
#include "EntryBuilders.h"
#include "DatabaseMaster.h"

//TODO: studios
//TODO: Storing game entries
//TODO: entry's link struct storing linking ids ie games to studios 
// 
//each entry get it's own file, need store path with entry database, use folder tree
//1st: folders games, studios etc
//2nd: folders Dir00000-Dir65536, Dir00001, Dir00002, Dir00003, etc
//3rd: 255 entries in each folder Game001, Studio245, in their own folder

shared_ptr<DatabaseMaster> appDbManager;

void DataBase() {
     shared_ptr<EntryDatabase>dataBase = appDbManager->GetEntryDatabase();
     shared_ptr<GenreListDataBase> gdatabase = appDbManager->GetGenreDatabase();
     shared_ptr<TagListDataBase> tagDB = appDbManager->GetTagDatabase();
     dataBase->UpdateEntriesFile();

     dataBase->PrintActiveEntries();
     dataBase->PrintEntriesPaths();

     gdatabase->PrintGenreList();
     tagDB->PrintTagList();

     GameGenres genres(gdatabase);
     genres.AddGenre(gdatabase->GetKey("Strategy"));
     genres.AddGenre(gdatabase->GetKey("Racing"));
     genres.AddGenre(gdatabase->GetKey("Casual"));
     unique_ptr<char[]> binGenre = genres.ToBinary();

     GameGenres genresRe = GameGenres(gdatabase, binGenre.get());
     genresRe.PrintGenres();

     GameTags tags(tagDB);
     tags.AddTag(tagDB->GetKey("Zombies"));
     tags.AddTag(tagDB->GetKey("Favourites"));

     unique_ptr<char[]> binTags = tags.ToBinary();

     GameTags tagsRe = GameTags(tagDB, binTags.get());
     tagsRe.PrintTags();

     EntryBuilder entryBuilder( dataBase);

     entryBuilder.SetNameYear("Fifa22", 2021);

     shared_ptr<Entry> entry1;

     if (entryBuilder.BuildAndSaveEntry(entry1)) {
         cout << "Successfully built entry:\n"
             << "Id: " << entry1.get()->Id() << "\n"
             << "Name: " << entry1.get()->Name() << "\n"
             << "Type: " << entry1.get()->Type() << "\n"
             << "Year: " << entry1.get()->Year() << "\n\n";
     }
     GameRatings ratings(8.0f, 9.0f, 5.5f, 3.1f);

     GameEntryBuilder gameBuilder(appDbManager);

     gameBuilder.SetNameYear("Dan's Test Game 29", 2023);

     gameBuilder.SetShortDescription("Action adventure rpg as a penguin");
     gameBuilder.SetFullDescription("Explore a quaint iceberg village, full of interesting villagers. whilst building their small town and completing their quests. ");
     gameBuilder.SetRatings(ratings);
     gameBuilder.SetTags(tags);
     gameBuilder.SetGenres(genres);

     gameBuilder.AddTag("Dan's-Game", true);

     if (gameBuilder.BuildEntry(entry1)) {
         cout << "Successfully built entry:\n"
             << "Id: " << entry1.get()->Id() << "\n"
             << "Name: " << entry1.get()->Name() << "\n"
             << "Type: " << entry1.get()->Type() << "\n"
             << "Year: " << entry1.get()->Year() << "\n\n";
     }

     gameBuilder.SetValidId();
     shared_ptr<GameEntry> gameEntry1;

     if (gameBuilder.BuildGameEntry(gameEntry1)) {
         gameEntry1->PrintInfo();
     }

     dataBase->AddGameEntry(gameEntry1);

     GameEntry ge2 = dataBase->GetGameEntry(gameEntry1->Id());

     ge2.PrintInfo();

     GameFinances finances(1200000, 2300000, 200, 9000000);
     finances.PrintFinances();

     shared_ptr<char[]> binFinances = finances.ToBinary();

     GameFinances finances2(binFinances.get());
     finances2.PrintFinances();
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

    AppClose();
}