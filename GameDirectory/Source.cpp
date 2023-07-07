#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"
#include "EntryBuilders.h"
#include "DatabaseMaster.h"

shared_ptr<DatabaseMaster> appDbManager;

void DataBase() {
     shared_ptr<EntryDatabase>dataBase = appDbManager->GetEntryDatabase();
     shared_ptr<GenreListDataBase> gdatabase = appDbManager->GetGenreDatabase();
     shared_ptr<TagListDataBase> tagDB = appDbManager->GetTagDatabase();


     EntryInfo_Short info{ 1, ET_Game, 2023, "Penguin Village" };
     EntryInfo_Short info2{ 2, ET_Game, 2022, "Back to hell" };
     EntryInfo_Short info23{ 3, ET_Studio, 2023, "Shady Seals" };

     //dataBase.mActiveEntries.clear();
     //dataBase.mActiveEntries.push_back(info);dataBase.mActiveEntries.push_back(info2);dataBase.mActiveEntries.push_back(info23);

     tagDB->AddTag("Fast Paced");
     tagDB->AddTag("Open World");
     tagDB->AddTag("Zombies");
     tagDB->AddTag("Favourites");

     dataBase->RemoveDuplicates();
     dataBase->PrintActiveEntries();

     gdatabase->PrintGenreList();
     //gdatabase.UpdateGenreListFile();

     tagDB->PrintTagList();
     //tagDB.UpdateTagListFile();

     GameGenres genres(gdatabase.get());
     genres.AddGenre(gdatabase->GetKey("Strategy"));
     genres.AddGenre(gdatabase->GetKey("Racing"));
     genres.AddGenre(gdatabase->GetKey("Casual"));

     unique_ptr<char[]> binGenre = genres.ToBinary();

     GameGenres genresRe = GameGenres(gdatabase.get(), binGenre.get());

     cout << "Genre 1: " << genresRe.GetGenres()[0] << "\n";
     cout << "Genre 2: " << genresRe.GetGenres()[1] << "\n";
     cout << "Genre 3: " << genresRe.GetGenres()[2] << "\n\n";

     GameTags tags(tagDB.get());
     tags.AddTag(tagDB->GetKey("Zombies"));
     tags.AddTag(tagDB->GetKey("Favourites"));
     tags.PrintTags();

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

    auto ge = make_shared<GameEntry>();
    auto entry = dynamic_pointer_cast<Entry>(ge);

    cout << "game entry name: " << ge->Name() << "\n";
    cout << "game entry id: " << entry->Id() << "\n";
    cout << "game entry type: " << ge->Type() << "\n\n";

    string gameName = "Penguin village";

    EntryInfo_Short info = ge.get()->GetSummary();

    //EntryInfo_Short info{ 662, T_Game, gameName };

    cout << "info name: " << info.name << "\n";
    cout << "info id: " << info.id << "\n";
    cout << "info type: " << info.type << "\n";
    cout << "info year: " << info.year << "\n\n";

    unique_ptr<char[]> binDat = ge.get()->GetRawData_Short();

    EntryInfo_Short info2(binDat.get());

    cout << "name: " << info2.name << "\n";
    cout << "id: " << info2.id << "\n";
    cout << "type: " << info2.type << "\n";
    cout << "year: " << info.year << "\n";

    GameRatings ratings(8.0f,9.0f,5.5f,3.1f);

    unique_ptr<char[]> ratingsBinDat = ratings.ToBinary();

    GameRatings ratings2(ratingsBinDat.get());

    ratings2.DisplayAllRatings();




    AppClose();
}

//TODO: Move all testing to an actual unit test set up