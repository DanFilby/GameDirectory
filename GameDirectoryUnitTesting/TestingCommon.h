#pragma once

#include "pch.h"
#include "CppUnitTest.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../GameDirectory/Entries.h"
#include "../GameDirectory/EntryUtils.h"
#include "../GameDirectory/EntryBuilders.h"

//common functions used in each testing class

Entry CreateStandardEntry();

/// <summary>
/// Checks entry's properites against params
/// </summary>
void CheckEntryProperties(Entry& entry, ENTRYID id, EntryType type, uint16_t year, string name);

/// <summary>
/// check entry summary against params
/// </summary>
void CheckEntrySummaryProperties(EntryInfo_Short& entrySum, ENTRYID id, EntryType type, uint16_t year, string name);
