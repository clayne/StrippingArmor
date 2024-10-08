#include "Utility.h"

namespace Utility
{
	void ExecuteCommandString(std::string command, std::string displayPrefix)
	{
		ExecuteCommand(0, command.data());
		Info(displayPrefix + command);
	}

	void ExecuteCommandStringOnFormID(int formID, std::string subCommand)
	{
		std::string command = fmt::format("{}.{}", num2hex(formID), subCommand);
		if (formID == 0x0) {
			//ERROR(fmt::format("ERROR: invalid command:{}", command));
			return;
		}
		ExecuteCommandString(command);
	}

	void Notification(std::string message)
	{
		bool NotificationOn = false;
		bool InfoOn = false;
		//NotificationOn = true;
		InfoOn = true;
		if (NotificationOn)
			RE::DebugNotification(message.c_str());
		if (InfoOn)
			logger::info("{}", message);
	}

	void Notification(std::string message, bool flg)
	{
		bool NotificationOn = flg;
		bool InfoOn = false;
		//NotificationOn = true;
		InfoOn = true;
		if (NotificationOn)
			RE::DebugNotification(message.c_str());
		if (InfoOn)
			logger::info("{}", message);
	}

	void SetLogLevel(int level)
	{
		int before = level;
		LogLevel = level;
		Notification(fmt::format("SetLogLevel: {} => {}", before, LogLevel));
	}

	void Error(std::string message)
	{
		if (LogLevel <= 2)
			Notification(message);
	}

	void Info(std::string message)
	{
		if (LogLevel <= 1)
			Notification(message);
	}

	void Debug(std::string message)
	{
		if (LogLevel <= 0)
			Notification(message);
	}

	void Notify(std::string message)
	{
		Notification(message, true);
	}

	bool InGameScene(bool withOutMenu)
	{
		auto* player = RE::PlayerCharacter::GetSingleton();
		if (player == nullptr)
			return false;

		if (!RE::UI::GetSingleton()->IsMenuOpen("HUDMenu"))
			return false;

		if (RE::UI::GetSingleton()->IsMenuOpen("EndGameCreditsMenu"))
			return false;

		if (RE::UI::GetSingleton()->IsMenuOpen("MainMenu"))
			return false;
		if (withOutMenu)
			return true;

		if (RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu"))
			return true;
		if (RE::UI::GetSingleton()->IsMenuOpen("Console"))
			return false;
		if (IsMenuOthersOpen())
			return false;
		return true;
	}

	bool IsMenuOthersOpen()
	{
		if (IsMenuForSystemOpen())
			return true;
		if (IsMenuForCraftingOpen())
			return true;
		if (IsMenuForTerminalOpen())
			return true;
		if (IsMenuForTradeOpen())
			return true;
		return false;
	}

	bool IsMenuInGameOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu") || RE::UI::GetSingleton()->IsMenuOpen("WorkshopMenu") || RE::UI::GetSingleton()->IsMenuOpen("CursorMenu") || RE::UI::GetSingleton()->IsMenuOpen("MonocleMenu") || RE::UI::GetSingleton()->IsMenuOpen("PickpocketMenu"))
			return true;
		return false;
	}

	bool IsMenuForSystemOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("PauseMenu") || RE::UI::GetSingleton()->IsMenuOpen("Console") || RE::UI::GetSingleton()->IsMenuOpen("FaderMenu") || RE::UI::GetSingleton()->IsMenuOpen("LoadingMenu") || RE::UI::GetSingleton()->IsMenuOpen("MainMenu") || RE::UI::GetSingleton()->IsMenuOpen("GalaxyStarMapMenu"))
			return true;
		return false;
	}

	bool IsMenuForCraftingOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("ResearchMenu") || RE::UI::GetSingleton()->IsMenuOpen("ArmorCraftingMenu") || RE::UI::GetSingleton()->IsMenuOpen("FaderMenu") || RE::UI::GetSingleton()->IsMenuOpen("FoodCraftingMenu") || RE::UI::GetSingleton()->IsMenuOpen("DrugsCraftingMenu") || RE::UI::GetSingleton()->IsMenuOpen("IndustrialCraftingMenu") || RE::UI::GetSingleton()->IsMenuOpen("WeaponsCraftingMenu"))
			return true;
		return false;
	}

	bool IsMenuForTradeOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("ContainerMenu") || RE::UI::GetSingleton()->IsMenuOpen("BarterMenu") || RE::UI::GetSingleton()->IsMenuOpen("SpaceshipEditorMenu"))
			return true;
		return false;
	}

	bool IsMenuForTerminalOpen()
	{
		if (RE::UI::GetSingleton()->IsMenuOpen("MissionBoard") || RE::UI::GetSingleton()->IsMenuOpen("DataMenu") || RE::UI::GetSingleton()->IsMenuOpen("GenesisTerminalMenu"))
			return true;
		return false;
	}

	bool IsMenuOpen(std::string MenuName, bool DialogueOnly)
	{
		if (MenuName != "DialogueMenu" || !DialogueOnly)
			return RE::UI::GetSingleton()->IsMenuOpen(MenuName);

		//DialogueMenuRoute
		bool result = RE::UI::GetSingleton()->IsMenuOpen("DialogueMenu");
		if (IsMenuOthersOpen())
			result = false;
		return result;
	}

	std::vector<int> DecomposeSlot(uint32_t value)
	{
		std::vector<int> powers;

		while (value > 0) {
			int logVal = static_cast<int>(std::log2(value));
			powers.push_back(logVal);
			value -= static_cast<unsigned int>(std::pow(2, logVal));
		}

		return powers;
	}

	bool HasSlot(uint32_t value, int slot)
	{
		return value & (1 << slot);
	}

	RE::BGSKeyword* GetKeywordFromID(int formID, int modIndex)
	{
		int fullFormID = modIndex * (1 << 24) + formID;

		auto form = RE::TESForm::LookupByID<RE::BGSKeyword>(fullFormID);
		if (!form) {
			Error(format("can't find id:{}", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		return form;
	}

	int GetModIndex(int formID, std::string name)
	{
		std::string myName = ToLower(name);

		for (int i = 0; i < 256; i++) {
			int  num = formID + (1 << 24) * i;
			auto numstr = Utility::num2hex(num);
			auto form = RE::TESForm::LookupByID(num);
			if (!form)
				continue;
			std::string editorID = ToLower(std::string(form->GetFormEditorID()));
			if (editorID == myName) {
				Info(format("GetModIndex: myModIndex:{}, myName:{}", i, myName));
				return i;
			}
		}
		return 0;
	}

	std::string ToLower(std::string str)
	{
		std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) {
			return std::tolower(c);
		});
		return str;
	}

	bool HasKeyword(RE::TESBoundObject* item, std::string keyword)
	{
		auto armor = BoundObjectToArmor(item);
		return HasKeyword(armor, keyword);
	}

	bool HasKeyword(RE::TESObjectARMO* armor, std::string keyword)
	{
		return armor->ContainsKeywordString(keyword);
	}

	bool HasKeyword(RE::TESObjectWEAP* weapon, std::string keyword)
	{
		return weapon->ContainsKeywordString(keyword);
	}

	bool HasKeyword(RE::TESObjectMISC* item, std::string keyword)
	{
		return item->ContainsKeywordString(keyword);
	}

	bool HasKeyword(RE::TESObjectREFR* member, std::string keyword)
	{
		return HasKeyword(member, GetKeywordFromString(keyword));
	}

	bool HasKeyword(RE::TESObjectREFR* member, RE::BGSKeyword* keyword)
	{
		return member->HasKeyword(keyword);
	}

	bool HasKeyword(RE::TESBoundObject* item, RE::BGSKeyword* keyword)
	{
		auto armor = BoundObjectToArmor(item);
		return HasKeyword(armor, keyword);
	}

	RE::BGSKeyword* GetKeywordFromString(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID<RE::BGSKeyword>(editorID.c_str());
		if (!form) {
			Error(fmt::format("keyword({}) was not found.", editorID));
			return nullptr;
		}
		return form;
	}

	RE::TESObjectMISC* GetMiscFromID(int formID, int modIndex)
	{
		int  fullFormID = modIndex * (1 << 24) + formID;
		auto form = RE::TESForm::LookupByID<RE::TESObjectMISC>(fullFormID);
		if (!form) {
			Error(fmt::format("formID:{} was not found", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		return form;
		//return static_cast<RE::TESObjectMISC*>(form);
	}

	RE::TESObjectMISC* GetMiscFromString(std::string editorID)
	{
		auto tmp = RE::TESForm::LookupByEditorID(editorID.c_str());
		if (!tmp) {
			Error(fmt::format("tmp({}) was not found.", editorID));
		}
		auto tmp2 = static_cast<RE::TESObjectMISC*>(tmp);
		if (!tmp2) {
			Error(fmt::format("tmp2({}) was not found.", editorID));
		}
		auto form = RE::TESForm::LookupByEditorID<RE::TESObjectMISC>(editorID.c_str());
		if (!form) {
			Error(fmt::format("misc({}) was not found.", editorID));
			return nullptr;
		}
		if (!form && tmp2)
			return tmp2;
		return form;
	}

	RE::TESObjectWEAP* GetWeaponFromString(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID<RE::TESObjectWEAP>(editorID.c_str());
		if (!form) {
			Error(fmt::format("weapon({}) was not found.", editorID));
			return nullptr;
		}
		return form;
	}

	RE::TESBoundObject* GetWeaponFromID(int formID, int modIndex)
	{
		int fullFormID = modIndex * (1 << 24) + formID;
		//Info(format("GetArmorFromID: TEST: idDec:{}, idHex:{}", fullFormID, Utility::num2hex(fullFormID)));
		auto form = RE::TESForm::LookupByID(fullFormID);
		if (!form) {
			Error(format("ERROR: can't find id:{}", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		auto form2 = static_cast<RE::TESBoundObject*>(form);
		if (!form2) {
			Error(format("ERROR: can't cast id:{}, {}", Utility::num2hex(fullFormID), form2->GetFormEditorID()));
			return nullptr;
		}
		return form2;
	}

	RE::TESBoundObject* GetArmorFromStringAsBoundObject(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID(editorID.c_str());
		if (!form) {
			Error(format("armor({}) was not found.", editorID));
			return nullptr;
		}
		auto armor = static_cast<RE::TESBoundObject*>(form);
		return armor;
	}

	RE::TESObjectREFR* GetPlayer()
	{
		auto playerForm = RE::TESForm::LookupByID(0x14);
		if (playerForm == nullptr) {
			Error(fmt::format("Can't get player form."));
			return nullptr;
		}

		auto player = static_cast<RE::TESObjectREFR*>(playerForm);
		if (player == nullptr) {
			Error(fmt::format("Can't get player ref."));
			return nullptr;
		}
		return player;
	}

	RE::Actor* GetPlayerActor()
	{
		auto playerForm = RE::TESForm::LookupByID(0x14);
		if (playerForm == nullptr) {
			Error(fmt::format("Can't get player form."));
			return nullptr;
		}

		auto player = static_cast<RE::Actor*>(playerForm);
		if (player == nullptr) {
			Error(fmt::format("Can't get player actor ref."));
			return nullptr;
		}
		return player;
	}

	RE::Actor* Object2Actor(RE::TESObjectREFR* member)
	{
		if (!member)
			return nullptr;
		auto form = RE::TESForm::LookupByID(member->formID);
		if (!form) {
			Error(format("can't find id:{}", Utility::num2hex(member->formID)));
			return nullptr;
		}
		auto form2 = static_cast<RE::Actor*>(form);
		if (!form2) {
			Error(format("can't cast id:{}, {}", Utility::num2hex(member->formID), form2->GetFormEditorID()));
			return nullptr;
		}
		return form2;
	}

	bool IsCommandedActor(RE::TESObjectREFR* member)
	{
		auto actor = Object2Actor(member);
		if (!actor)
			return false;
		return actor->boolFlags.any(RE::Actor::BOOL_FLAGS::kIsCommandedActor);
	}

	bool IsBleeding(RE::TESObjectREFR* member)
	{
		auto actor = Object2Actor(member);
		if (!actor)
			return false;
		return actor->boolFlags.any(RE::Actor::BOOL_FLAGS::kInBleedoutAnimation);
	}

	bool IsTalking(RE::TESObjectREFR* member)
	{
		if (!member)
			return false;
		auto actor = Object2Actor(member);
		if (actor) {
			auto pointer = actor->dialogueItemTarget;
			auto pkg = actor->currentProcess->currentPackage.package;
			if (pointer != 0) {
				Debug(fmt::format("test: {}({}): pointer={}", member->GetFormEditorID(), Utility::num2hex(member->formID), pointer));
			} else {
				Debug(fmt::format("test: {}({}): pointer is zero", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			}
			if (pkg) {
				Debug(fmt::format("test: {}({}): pkg={}({})", member->GetFormEditorID(), Utility::num2hex(member->formID), pkg->GetFormEditorID(), pkg->formID));
			} else {
				Debug(fmt::format("test: {}({}): pkg is nullptr", member->GetFormEditorID(), Utility::num2hex(member->formID)));
			}
		}

		return member->IsTalking();
	}

	RE::TESObjectARMO* GetArmorFromString(std::string editorID)
	{
		auto form = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(editorID.c_str());
		if (!form) {
			Error(format("armor({}) was not found.", editorID));
			return nullptr;
		}
		return form;
	}

	RE::TESBoundObject* GetArmorFromID(int formID, int modIndex)
	{
		int  fullFormID = modIndex * (1 << 24) + formID;
		//Info(format("GetArmorFromID: TEST: idDec:{}, idHex:{}", fullFormID, Utility::num2hex(fullFormID)));
		auto form = RE::TESForm::LookupByID(fullFormID);
		if (!form) {
			Error(format("can't find id:{}", Utility::num2hex(fullFormID)));
			return nullptr;
		}
		auto form2 = static_cast<RE::TESBoundObject*>(form);
		if (!form2) {
			Error(format("can't cast id:{}, {}", Utility::num2hex(fullFormID), form2->GetFormEditorID()));
			return nullptr;
		}
		return form2;
	}

	std::string GetFormIDsFromVector(std::vector<int> list, std::string separator, bool quoteOn, bool hexOn)
	{
		std::string result = "";
		std::string quote = quoteOn ? "\"" : "";
		for (int formID : list) {
			std::string sFormID = (hexOn) ? num2hex(formID) : fmt::format("{}", formID);
			result = result + separator + quote + sFormID + quote;
		}
		return result;
	}

	std::string num2hex(uint32_t formID, bool prefixOn, bool paddingOn)
	{
		std::string result = paddingOn ? fmt::format("{:08x}", formID) : fmt::format("{:x}", formID);
		result = prefixOn ? "0x" + result : result;
		return result;
	}

	uint32_t hex2num(std::string formID)
	{
		int intValue;
		try {
			intValue = std::stoi(formID, 0, 16);  // �x�[�X16�i16�i���j�ŕϊ�
			return intValue;
		} catch (const std::invalid_argument& ia) {
			return 0;
		} catch (const std::out_of_range& oor) {
			return 0;
		}
		return 0;
	}

	std::unordered_map<RE::TESBoundObject*, int> CollectEquipItems(RE::TESObjectREFR* actor, std::string itemType)
	{
		//Info("CollectInventoryItems Start:");
		ItemType = itemType;
		ItemForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			//Info(fmt::format("item: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
			bool isTarget = false;
			isTarget = (ItemType == "ALL") ? true : isTarget;
			isTarget = (ItemType == "ARMOR" && item.object->IsArmor()) ? true : isTarget;
			isTarget = (ItemType == "WEAPON" && item.object->IsWeapon()) ? true : isTarget;
			isTarget = (ItemType == "BOUNDOBJECT" && item.object->IsBoundObject()) ? true : isTarget;
			if (isTarget)
				ItemForScanner[item.object] += GetEquipmentStackCount(item);
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		//Info("CollectInventoryItems Finish:");
		return ItemForScanner;
	}

	std::unordered_map<RE::TESObjectARMO*, int> CollectEquipArmors(RE::TESObjectREFR* actor)
	{
		ArmorForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				auto armor = BoundObjectToArmor(item.object);
				ArmorForScanner[armor] += GetEquipmentStackCount(item);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		return ArmorForScanner;
	}

	std::vector<RE::TESObjectARMO*> CollectEquipArmorsWithoutCount(RE::TESObjectREFR* actor)
	{
		ArmorVector.clear();
		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				auto armor = BoundObjectToArmor(item.object);
				ArmorVector.push_back(armor);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachEquippedItem(scanner);
		return ArmorVector;
	}

	std::unordered_map<RE::TESBoundObject*, int> CollectInventoryItems(RE::TESObjectREFR* actor, std::string itemType)
	{
		//Info("CollectInventoryItems Start:");
		ItemType = itemType;
		ItemForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			//Info(fmt::format("item: {}: {}", num2hex(item.object->formID), item.object->GetFormEditorID()));
			bool isTarget = false;
			isTarget = (ItemType == "ALL") ? true : isTarget;
			isTarget = (ItemType == "ARMOR" && item.object->IsArmor()) ? true : isTarget;
			isTarget = (ItemType == "WEAPON" && item.object->IsWeapon()) ? true : isTarget;
			isTarget = (ItemType == "BOUNDOBJECT" && item.object->IsBoundObject()) ? true : isTarget;
			isTarget = (ItemType == "MISC" && item.object->Is(RE::FormType::kMISC)) ? true : isTarget;
			if (isTarget)
				ItemForScanner[item.object] += GetEquipmentStackCount(item);
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		//Info("CollectInventoryItems Finish:");
		return ItemForScanner;
	}

	std::unordered_map<RE::TESObjectARMO*, int> CollectInventoryArmors(RE::TESObjectREFR* actor)
	{
		ArmorForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsArmor()) {
				auto armor = BoundObjectToArmor(item.object);
				ArmorForScanner[armor] += GetEquipmentStackCount(item);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		return ArmorForScanner;
	}

	std::unordered_map<RE::TESObjectWEAP*, int> CollectInventoryWeapons(RE::TESObjectREFR* actor)
	{
		WeaponForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			if (item.object->IsWeapon()) {
				auto weapon = BoundObjectToWeapon(item.object);
				WeaponForScanner[weapon] += GetEquipmentStackCount(item);
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		return WeaponForScanner;
	}

	std::unordered_map<RE::TESObjectMISC*, int> CollectInventoryMiscItems(RE::TESObjectREFR* actor)
	{
		MiscForScanner.clear();

		auto scanner = [](const RE::BGSInventoryItem& item) -> RE::BSContainer::ForEachResult {
			//Info(fmt::format("CollectInventoryMiscItems: item id:{}", num2hex(item.object->formID)));
			if (item.object->Is(RE::FormType::kMISC)) {
				auto misc = BoundObjectToMisc(item.object);
				if (!misc)
					Error(fmt::format("id:{} can't convert MISC.", num2hex(item.object->formID)));
				else {
					MiscForScanner[misc] += GetEquipmentStackCount(item);
					Info(fmt::format("->CollectInventoryMiscItems: item :{}", misc->GetFormEditorID()));
				}
			}
			return RE::BSContainer::ForEachResult::kContinue;
		};
		actor->ForEachInventoryItem(scanner);
		return MiscForScanner;
	}

	std::unordered_map<RE::TESObjectREFR*, bool> CollectRefsInCell(RE::TESObjectCELL* cell)
	{
		RefsForScanner.clear();
		if (cell == nullptr)
			return RefsForScanner;

		auto scanner = [](const RE::NiPointer<RE::TESObjectREFR>& ref) -> RE::BSContainer::ForEachResult {
			Debug("test: in loop");
			auto obj = ref.get();
			if (obj)
				Info(fmt::format("CollectRefsInCell: obj id:{}", num2hex(obj->formID)));
			if (IsValidNPC(obj))
				RefsForScanner[obj] = true;
			return RE::BSContainer::ForEachResult::kContinue;
		};
		cell->ForEachReference(scanner);
		return RefsForScanner;
	}

	template<typename T>
    T* GetMember(void* base, std::ptrdiff_t offset) {
        auto address = std::uintptr_t(base) + offset;
        auto reloc = REL::Relocation<T*>(address);
        return reloc.get();
    }

	std::unordered_map<RE::TESObjectREFR*, bool> CollectRefsInCellInRangeAlt(RE::TESObjectCELL* cell, RE::TESObjectREFR* actor, float radius)
	{
		uint32_t cellAddr = reinterpret_cast<uint32_t>(cell);
		auto references = GetMember<RE::BSTArray<RE::NiPointer<RE::TESObjectREFR>>>(cell, TESObjectCELL_references_offset);
		std::unordered_map<RE::TESObjectREFR*, bool> RefsForScanner;
        for (const auto& ref : *references) {
			if (!ref)
				continue;
			auto refr = ref.get();
			if (!refr->IsActor())
				continue;
			Debug(fmt::format("formID:{:x}, editorID:{}", refr->formID, refr->GetFormEditorID()));
        }
        for (const auto& ref : *references) {
			if (!ref)
				continue;
			auto refr = ref.get();
			if (!refr->IsActor())
				continue;
			Debug(fmt::format("formID:{:x}, editorID:{}", refr->formID, refr->GetFormEditorID()));
			if (actor->GetPosition().GetDistance(refr->GetPosition()) > radius)
				continue;
			RefsForScanner[refr] = true;
        }
		return RefsForScanner;
	}

	std::unordered_map<RE::TESObjectREFR*, bool> CollectRefsInCellInRange(RE::TESObjectCELL* cell, RE::TESObjectREFR* actor, float radius)
	{
		//RefsForScanner.clear();
		//if (cell == nullptr || actor == nullptr)
		//	return RefsForScanner;
		//auto origin = actor->GetPosition();

		//auto scanner = [](const RE::NiPointer<RE::TESObjectREFR>& ref) -> RE::BSContainer::ForEachResult {
		//	auto obj = ref.get();
		//	if(obj)
		//		Info(fmt::format("CollectRefsInCellInRange: obj id:{}", num2hex(obj->formID)));
		//	if (!IsWrongForm(obj, "CollectRefsInCellInRange")) {
		//		if (IsValidNPC(obj))
		//			RefsForScanner[obj] = true;
		//	}
		//	return RE::BSContainer::ForEachResult::kContinue;
		//};

		//Debug("before: CollectRefsInCellInRange");
		////cell->ForEachReferenceInRange(ConvertFromNP3ToNP3A(origin), radius, scanner);
		//cell->ForEachReference(scanner);
		//Debug("after : CollectRefsInCellInRange");
		//auto cworld = cell->cellWorldspace;
		//if (cworld == nullptr) {
		//	Debug("cworld is null");
		//} else {
		//	Debug(fmt::format("cworld:{}", cworld->GetFormEditorID()));
		//}
		//
		//auto np3 = ConvertFromNP3ToNP3A(origin);

		//Debug(fmt::format("origin: {}, {}, {}", np3.x, np3.y, np3.z));
		//auto refs = cell->references;
		//if (refs.empty()) {
		//	Debug("refs is empty");
		//} else {
		//	Debug("refs is NOT empty");

		//	for (auto ref : refs) {
		//		auto obj = ref.get();
		//		Debug(fmt::format("obj: formID:{}", obj->formID));
		//	}
		//}
		//
		//Debug(fmt::format("cell: cell->IsExterior:{}", cell->IsExterior()));
		////Debug(fmt::format("cell: cell->cellData:{}", cell->cellData.exterior->cellX));
		//auto ref = RE::PlayerCharacter::GetSingleton()->crosshairRef;
		//if (ref == nullptr) {
		//	Debug("crosshair is null");
		//} else {
		//	Debug(fmt::format("ref is {}({})", num2hex(ref->formID), ref->GetFormEditorID())); 
		//}
		//return RefsForScanner;
	}

	RE::NiPoint3A ConvertFromNP3ToNP3A(RE::NiPoint3 origin)
	{
		RE::NiPoint3A np3a;
		np3a.x = origin.x;
		np3a.y = origin.y;
		np3a.z = origin.z;

		return np3a;
	}

	bool IsWrongForm(RE::TESObjectREFR* member, std::string debugMsg)
	{
		if (!member) {
			Error(fmt::format("  ERROR!! {}: member is null", debugMsg));
			return true;
		}
		if (member->formID == 0x0) {
			Error(fmt::format("  ERROR!! {}: member is 0x0", debugMsg));
			return true;
		}
		return false;
	}

	bool IsValidNPC(RE::TESObjectREFR* member)
	{
		if (IsWrongForm(member, "IsValidNPC"))
			return false;
		if (!member->IsActor())
			return false;
		if (member->formID == 0x14) {
			//Info(fmt::format("DebugToSameCell: formID 0x14 is player. Skip"));
			return false;
		}
		if (!Utility::IsHumanRace(member)) {
			//Info(fmt::format("DebugToSameCell: formID:{} is NOT HumanRace. Skip", Utility::num2hex(obj->formID)));
			return false;
		}
		return true;
	}

	bool IsHumanRace(RE::TESObjectREFR* member)
	{
		if (!member)
			return false;
		//auto racename = member->GetVisualsRace()->GetFormEditorID();
		auto raceid = member->GetVisualsRace()->GetFormID();

		return raceid == 0x347d;
		//if (raceid != 0x347d) {
		//	Info(fmt::format("DebugToSameCell: formID:{} is NOT HumanRace({}). Skip", Utility::num2hex(obj->formID), racename));
		//	continue;
		//}
	}

	bool IsInSameCell(RE::TESObjectREFR* member1, RE::TESObjectREFR* member2)
	{
		if (!member1 || !member2)
			return false;
		return member1->parentCell == member2->parentCell;
	}

	RE::TESObjectARMO* BoundObjectToArmor(RE::TESBoundObject* item)
	{
		if (!item || !item->IsArmor())
			return nullptr;
		auto armor = RE::TESForm::LookupByEditorID<RE::TESObjectARMO>(item->GetFormEditorID());
		return armor;
	}

	RE::TESObjectWEAP* BoundObjectToWeapon(RE::TESBoundObject* item)
	{
		if (!item || !item->IsWeapon())
			return nullptr;
		auto weapon = RE::TESForm::LookupByEditorID<RE::TESObjectWEAP>(item->GetFormEditorID());
		return weapon;
	}

	RE::TESObjectMISC* BoundObjectToMisc(RE::TESBoundObject* item)
	{
		if (!item || !item->Is(RE::FormType::kMISC))
			return nullptr;
		auto misc = RE::TESForm::LookupByEditorID<RE::TESObjectMISC>(item->GetFormEditorID());
		return misc;
	}

	void AddItem(RE::TESObjectREFR* member, int itemFormID, int num) { AddItem(member->formID, itemFormID, num); }

	void AddItem(int actorFormID, int itemFormID, int num)
	{
		Utility::ExecuteCommandStringOnFormID(actorFormID, fmt::format("additem {} {}", num2hex(itemFormID), num));
	}


	void RemoveItem(RE::TESObjectREFR* member, int itemFormID, int num) { RemoveItem(member->formID, itemFormID, num); }

	void RemoveItem(int actorFormID, int itemFormID, int num)
	{
		ExecuteCommandStringOnFormID(actorFormID, fmt::format("RemoveItem {} {}", num2hex(itemFormID), num));
	}

	void UnequipItem(RE::TESObjectREFR* member, int itemFormID) { UnequipItem(member->formID, itemFormID); }

	void UnequipItem(int actorFormID, int itemFormID)
	{
		ExecuteCommandStringOnFormID(actorFormID, fmt::format("UnequipItem {}", num2hex(itemFormID)));
	}

	void EquipItem(RE::TESObjectREFR* member, int itemFormID) { EquipItem(member->formID, itemFormID); }

	void EquipItem(int actorFormID, int itemFormID)
	{
		ExecuteCommandStringOnFormID(actorFormID, fmt::format("EquipItem {}", num2hex(itemFormID)));
	}

	int GetItemCount(RE::TESObjectREFR* member, int itemFormID, std::string itemType) { GetItemCount(member->formID, itemFormID, itemType); }

	int GetItemCount(int actorFormID, int itemFormID, std::string itemType)
	{
		auto form = RE::TESForm::LookupByID(actorFormID);
		if (!form)
			return 0;
		auto actor = static_cast<RE::TESObjectREFR*>(form);
		if (!actor)
			return 0;
		auto items = CollectInventoryItems(actor, itemType);
		for (auto itr = items.begin(); itr != items.end(); ++itr) {
			//Info(fmt::format("formID:{}, name:{}, num:{}, targetFormID:{}", num2hex(itr->first->formID), itr->first->GetFormEditorID(), itr->second, num2hex(itemFormID)));
			if (itr->first->formID == itemFormID)
				return itr->second;
		}
		return 0;
	}

	int GetPerkLevel(RE::TESObjectREFR* member, int formID)
	{
		auto actor = static_cast<RE::Actor*>(member);
		if (!actor)
			return 0;
		for (auto itr = actor->perks->perkRanks->begin(); itr != actor->perks->perkRanks->end(); ++itr) {
			if (itr->perk->formID == formID)
				return itr->currentRank;
		}
		return 0;
	}

	int GetEquipmentStackCount(const RE::BGSInventoryItem& item)
	{
		int sum = 0;
		if (item.object->IsArmor() || item.object->IsWeapon()) {
			for (int i = 0; i < item.stacks.size(); i++) {
				sum += item.stacks[i].count;
			}
		}
		return sum;
	}

	std::string GetArmorType(RE::TESObjectARMO* armor)
	{
		std::string result = "ERROR";
		if (armor->ContainsKeywordString("ArmorTypeApparelHead"))
			result = "Hat";
		else if (armor->ContainsKeywordString("ArmorTypeApparelOrNakedBody"))
			result = "Cloth";
		else if (armor->ContainsKeywordString("ArmorTypeSpacesuitBody"))
			result = "Spacesuit";
		else if (armor->ContainsKeywordString("ArmorTypeSpacesuitHelmet"))
			result = "Helmet";
		else if (armor->ContainsKeywordString("ArmorTypeSpacesuitBackpack"))
			result = "Backpack";
		else
			result = "ERROR";
		//Info(fmt::format("    GetArmorType: item: {}: result:{}", armor->GetFormEditorID(), result));
		return result;
	}
}