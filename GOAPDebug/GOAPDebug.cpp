#include "pch.h"
#include "CppUnitTest.h"
#include "G:/Documents/Unreal Projects/DissProject/Source/DissProject/Characters/GOAPEnemy.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace GOAPDebug
{
	TEST_CLASS(GOAPDebug)
	{
	public:
		
		TEST_METHOD(TestMethod1)
		{
			AGOAPEnemy Enemy = new AGOAPEnemy;
			auto Plan = Enemy.GetPlan();
		}
	};
}
