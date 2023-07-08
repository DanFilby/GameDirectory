#include "pch.h"
#include "CppUnitTest.h"

#include "../GameDirectory/Entries.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace EntryTesting
{
	TEST_CLASS(EntryTests)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			Entry entry{};
			int t = entry.Id();
				

		}
	};
}
