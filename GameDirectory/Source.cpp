#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"

void DataBase() {

    EntryDatabase dataBase{};

}


int main()
{
    DataBase();

    std::cout << "Hello World!\n";

    auto ge = make_shared<GameEntry>();
    auto entry = dynamic_pointer_cast<Entry>(ge);

    cout << "game entry name: " << ge->Name() << "\n";
    cout << "game entry id: " << entry->Id() << "\n";
    cout << "game entry rating: " << ge->Rating() << "\n";
    cout << "game entry type: " << ge->Type() << "\n\n";

    string gameName = "Penguin village";

    EntryInfo_Short info = ge.get()->GetInfo_Short();

    //EntryInfo_Short info{ 662, T_Game, gameName };

    cout << "info name: " << info.name << "\n";
    cout << "info id: " << info.id << "\n";
    cout << "info type: " << info.type << "\n\n";

    unique_ptr<char[]> binDat = ge.get()->GetRawData_Short();

    EntryInfo_Short info2(binDat.get());

    cout << "name: " << info2.name << "\n";
    cout << "id: " << info2.id << "\n";
    cout << "type: " << info2.type << "\n";

    cout << "\n";

}

