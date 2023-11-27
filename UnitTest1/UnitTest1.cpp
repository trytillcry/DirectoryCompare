#include "CppUnitTest.h"
#include "../funct.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest1
{
	TEST_CLASS(UnitTest1)
	{
	public:
		TEST_METHOD(TestMethod1)
		{
			string teststring = "C:/OEM/AcerLogs/|DriverInstallation.log|4000";
			Datei dat = strtofile(teststring);
			Assert::AreEqual(dat.getsize(), UINT64(4000));
			Assert::AreEqual(dat.getpath(), string("C:/OEM/AcerLogs/"));
			Assert::AreEqual(dat.getname(), string("DriverInstallation.log"));
		}


		TEST_METHOD(TestMethod2)
		{
			string teststring = "C:/|DriverInstallation.log|4000";
			Datei dat = strtofile(teststring);
			Assert::AreEqual(dat.getsize(), UINT64(4000));
			Assert::AreEqual(dat.getpath(), string("C:/"));
			Assert::AreEqual(dat.getname(), string("DriverInstallation.log"));
		}

	};
}
