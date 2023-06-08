#include <iostream>

#include "Common.h"
#include "EntryDatabase.h"

int main()
{
    std::cout << "Hello World!\n";

    auto ge = make_shared<GameEntry>(); 
    auto entry = dynamic_pointer_cast<Entry>(ge);

    std::cout << "game entry name: " << ge->Name() << "\n";
    std::cout << "game entry id: " << entry->Id() << "\n";
    std::cout << "game entry rating: " << ge->Rating();
}

