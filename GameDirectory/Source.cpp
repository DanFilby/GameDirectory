#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"
#include "EntryBuilders.h"
#include "DatabaseMaster.h"

//TODO: finish game entry and then builder, description 

shared_ptr<DatabaseMaster> appDbManager;

void DataBase() {
     shared_ptr<EntryDatabase>dataBase = appDbManager->GetEntryDatabase();
     shared_ptr<GenreListDataBase> gdatabase = appDbManager->GetGenreDatabase();
     shared_ptr<TagListDataBase> tagDB = appDbManager->GetTagDatabase();

     dataBase->UpdateEntriesFile();

     /*tagDB->AddTag("Fast Paced");
     tagDB->AddTag("Open World");
     tagDB->AddTag("Zombies");
     tagDB->AddTag("Favourites");*/

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
     genresRe.PrintGenres();

     GameTags tags(tagDB.get());
     tags.AddTag(tagDB->GetKey("Zombies"));
     tags.AddTag(tagDB->GetKey("Favourites"));

     unique_ptr<char[]> binTags = tags.ToBinary();

     GameTags tagsRe = GameTags(tagDB.get(), binTags.get());
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

    ratings2.DisplayAllRatings();

    AppClose();
}

//TODO: Move all testing to an actual unit test set up