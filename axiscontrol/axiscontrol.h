#pragma once
#pragma comment(lib, "PluginSDK.lib")
#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/plugin/pluginwindow.h"
#include <chrono>

class axiscontrol : public BakkesMod::Plugin::BakkesModPlugin
{
private:
	bool isInGoalReplay = false;
	std::chrono::steady_clock::time_point timeStartedBuffer;
	std::shared_ptr<bool>  workWhileSpectating;
	std::shared_ptr<int> logMode;
	std::string lastExecutedCommand = "";
	




public:
	//std::shared_ptr<bool>  onAlways;
	std::shared_ptr<std::string> command1;
	void onLoad() override;
	void onUnload() override;
	void executeCommands();
	ServerWrapper GetCurrentGameState();
	void Render(CanvasWrapper canvas);
	void GetControllerInput(std::shared_ptr<CVarManagerWrapper> cvarManager);
	std::shared_ptr<bool> forceExecuteCommand;
	std::shared_ptr<std::string> execCommands[9]; // Array for the 9 command strings
	std::string lastExecutedCommands[9];         // Array to track changes for each
	std::shared_ptr<bool> showDebugInputs;
	std::shared_ptr<int> manualControllerIndex; 
};
