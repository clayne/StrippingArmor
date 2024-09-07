#include "StrippingArmor.h"

namespace StrippingArmor
{
	void MainLoop()
	{
		Config::ReadIni();
		bool isext = false;
		while (true) {
			Sleep(Config::GetTimePerFrame());
			if (!Utility::InGameScene())
				continue;

			if (atOnce) {
				Debug("atOnce Start");
				atOnce = false;
				ResetParameter();
				Config::EsmNotLoadCheck();
				Debug("atOnce finish");
			}

			if (Events::NeedReset) {
				ResetParameter();
				continue;
			}

			StateSelector();
		}
	}

	void ResetParameter()
	{
		Debug("in ResetParameter: start");

		Events::NeedReset = false;
		StrippingArmorCommon::WaitCount = 0;
		
		StrippingArmorLoot::ResetParameter();
		StateMachine::ClearListForPickpocketTarget();
		StateMachine::ClearListForForKeytap();

		Debug("in ResetParameter: end");
	}

	void StateSelector()
	{
		Debug("in StateSelector: start");

		if (Utility::IsMenuOpen("DialogueMenu")) {
			StrippingArmorByKey::State_Dialogue();
		} else if (Utility::IsMenuOpen("PickpocketMenu")) {
			StrippingArmorPickpocket::State_Pickpocket();
		} else {
			StrippingArmorLoot::State_Loot();
			StrippingArmorByKey::State_StrippingByKey();
			StrippingArmorByKey::State_ToggleByKey();
		}

		if (!Utility::IsMenuOpen("PickpocketMenu") && !StateMachine::PickpocketMap.empty()) 
			StrippingArmorPickpocket::HouseKeepPickpocket();

		if (!Utility::IsMenuOpen("DialogueMenu")) 
			StrippingArmorByKey::HouseKeepByKey();

		Debug("in StateSelector: end");
	}
}
