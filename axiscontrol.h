#pragma once
#pragma comment(lib, "PluginSDK.lib")
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include <chrono>

class axiscontrol : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	bool isInGoalReplay = false;
	bool wasSuperSonic = false;
	std::chrono::steady_clock::time_point timeStartedBuffer;
	std::shared_ptr<bool>  workWhileSpectating;


public:
	//std::shared_ptr<bool>  onAlways;
	std::shared_ptr<std::string> command1;
	std::shared_ptr<bool> forceUpdate;
	void onLoad() override;
	void onUnload() override;
	void executeCommands();
	ServerWrapper GetCurrentGameState();
	void Render(CanvasWrapper canvas);
};
