#include "pch.h"
#include "CppUnitTest.h"

#include "../GameDirectory/DatabaseMaster.h"


using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace DatabaseTesting
{
	TEST_CLASS(DatabaseTests)
	{
	public:

		TEST_METHOD(test1)
		{
			DatabaseMaster dbmaster{};

			dbmaster.AppInit();
			ofstream outfile("TestingFile.dat", std::ios::out);
			Assert::IsTrue(	dbmaster.GetEntryDatabase()->GetEntryCount() == 0);
		}
	};
}
