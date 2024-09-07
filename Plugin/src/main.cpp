#pragma once
#include "PCH.h"
#include "Utility.h"
#include "Config.h"
#include "Event.h"
#include "StrippingArmor.h"

// SFSE message listener, use this to do stuff at specific moments during runtime
void Listener(SFSE::MessagingInterface::Message* message) noexcept
{
	if (message->type == SFSE::MessagingInterface::kPostLoad) {
		RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForMenu::GetSingleton());
		//RE::UI::GetSingleton()->RegisterSink(Events::EventHandlerForContainer::GetSingleton());
	}
}

namespace Main
{
	static DWORD MainLoop(void* unused)
	{
		Info("Main Start");
		StrippingArmor::MainLoop();
		return 0;
	}
}

extern "C" DLLEXPORT constinit auto SFSEPlugin_Version = []() noexcept {
	SFSE::PluginVersionData data{};

	data.PluginVersion(Plugin::Version);
	data.PluginName(Plugin::NAME);
	data.AuthorName(Plugin::AUTHOR);

	// REL::ID usage instead of REL::Offset
	data.UsesAddressLibrary(true);
	data.UsesSigScanning(true);

	data.CompatibleVersions({ SFSE::RUNTIME_LATEST });

	return data;
}();

extern "C" DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)

//DLLEXPORT bool SFSEAPI SFSEPlugin_Load(const SFSE::LoadInterface* a_sfse)
{
#ifndef NDEBUG
	MessageBoxA(NULL, "Loaded. You can now attach the debugger or continue execution.", Plugin::NAME.data(), NULL);
#endif

	SFSE::Init(a_sfse, true);
	Info(fmt::format("{} v{} loaded", Plugin::NAME, Plugin::Version));
	Info("test");
	if (const auto messaging{ SFSE::GetMessagingInterface() }; !messaging->RegisterListener(Listener))
		return false;
	CreateThread(NULL, 0, Main::MainLoop, NULL, 0, NULL);

	return true;
}
