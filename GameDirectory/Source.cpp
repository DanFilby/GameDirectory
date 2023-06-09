#include <iostream>
using std::cout;

#include "Common.h"
#include "EntryDatabase.h"

int main()
{
    std::cout << "Hello World!\n";

    auto ge = make_shared<GameEntry>();
    auto entry = dynamic_pointer_cast<Entry>(ge);

    cout << "game entry name: " << ge->Name() << "\n";
    cout << "game entry id: " << entry->Id() << "\n";
    cout << "game entry rating: " << ge->Rating();

    string gameName = "Penguin village";

    EntryInfo_Short info{ 662, T_Game, gameName };

    cout << "\n\nsize of entry struct " << sizeof(EntryInfo_Short) << "\n";
    cout << "name: " << info.name << "\n";
    cout << "id: " << info.id << "\n";
    cout << "type: " << info.type << "\n";

    char * binDat = info.ToBinary();

    EntryInfo_Short info2(binDat);

    cout << "name: " << info2.name << "\n";
    cout << "id: " << info2.id << "\n";
    cout << "type: " << info2.type << "\n";

    cout << "\n";
}

