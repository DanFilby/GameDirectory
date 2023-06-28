#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"

shared_ptr<DatabaseManager> appDbManager;

void DataBase() {
     EntryDatabase dataBase{};
     GenreListDataBase gdatabase{};
     TagListDataBase tagDB{};

     EntryInfo_Short info{ 1, ET_Game, 2023, "Penguin Village" };
     EntryInfo_Short info2{ 2, ET_Game, 2022, "Back to hell" };
     EntryInfo_Short info23{ 3, ET_Studio, 2023, "Shady Seals" };

     //dataBase.mActiveEntries.clear();
     //dataBase.mActiveEntries.push_back(info);dataBase.mActiveEntries.push_back(info2);dataBase.mActiveEntries.push_back(info23);

     /*tagDB.AddTag("Fast Paced");
     tagDB.AddTag("Open World");
     tagDB.AddTag("Zombies");
     tagDB.AddTag("Favourites");*/


     dataBase.UpdateEntriesFile();
     dataBase.PrintActiveEntries();

     gdatabase.PrintGenreList();
     //gdatabase.UpdateGenreListFile();

     tagDB.PrintTagList();
     //tagDB.UpdateTagListFile();
}

void AppStart() {
    //inits directories for entire app
    appDbManager = shared_ptr<DatabaseManager>();
    appDbManager->AppInit();
}

void AppClose() {
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

    EntryInfo_Short info = ge.get()->GetInfo_Short();

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

    cout << "\n";

    AppClose();
}

