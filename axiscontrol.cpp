#include "axiscontrol.h"
#include "bakkesmod\wrappers\includes.h"
#include "bakkesmod\wrappers\arraywrapper.h"
#include "bakkesmod\wrappers\playercontrollerwrapper.h"
#include <iomanip>   // For setprecision
#include <sstream>   // For stringstream
//#include "HelperFunctions.h"
//#include <bakkesmod\wrappers\firstpersonplugin.h>
#include <Windows.h>
#include <iostream>
#include <xinput.h>
#include <stack>
#include <set>
#include <cmath>
using namespace std;
#pragma comment(lib, "Xinput9_1_0.lib")

BAKKESMOD_PLUGIN(axiscontrol, "control game variables with triggers and joysticks", "1.3", PLUGINTYPE_FREEPLAY)
float savedSteer = 0;

float LT = 0;
float RT = 0;
float LSUp= 0;
float LSDown = 0;
float LSLeft = 0;
float LSRight = 0;
float RSUp = 0;
float RSDown = 0;
float RSLeft = 0;
float RSRight= 0;
float LSUD= 0;
float RSUD= 0;
float LSLR = 0;
float RSLR = 0;
float bothTriggers= 0;
float numResult = 0;
float leftCircle = 0;
float rightCircle = 0;
float leftMagnitude = 0;
float rightMagnitude = 0;
bool buttonX = false;
bool buttonY = false;
std::map<std::string, float> previousVariables = {
		{"lt", LT},
		{"rt", RT},
		{"lsup", LSUp},
		{"lsdown", LSDown},
		{"lsleft", LSLeft},
		{"lsright", LSRight},
		{"rsup", RSUp},
		{"rsdown", RSDown},
		{"rsleft", RSLeft},
		{"rsright", RSRight},
		{"lsud", LSUD},
		{"rsud", RSUD},
		{"lslr", LSLR},
		{"rslr", RSLR},
		{"bothtriggers", bothTriggers},
{"leftcircle", leftCircle },
{"rightcircle", rightCircle },
	{"leftmagnitude",leftMagnitude},
	{"rightmagnitude",rightMagnitude}
};
std::map<std::string, float> variablesUsed;
//std::string execCommand1 = "";
ControllerInput controllerInput;
std::string savedComand1 = "";
bool commandHasChanged = false;





void GetControllerInput(std::shared_ptr<CVarManagerWrapper> cvarManager, std::shared_ptr<bool> forceUpdate) {
	XINPUT_STATE state;
	for (DWORD i = 0; i < 4; i++) {
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState(i, &state) == ERROR_SUCCESS) {
			float currentLT = state.Gamepad.bLeftTrigger / 255.0f;

			// Now it can see forceUpdate
			if (*forceUpdate || currentLT > 0.01f) {
				LT = currentLT;
				return;
			}
		}
	}
}
//int main() {
//	while (true) {
		
	//	Sleep(1);  // Wait 100ms between checks (adjust as needed)
	//}
//	return 0;
//}




void axiscontrol::onLoad()
{
	//cvarManager->log("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");

	cvarManager->registerCvar("axiscontrol_Spectator", "1", "work while spectating").bindTo(workWhileSpectating);
	cvarManager->registerCvar("on_Always", "0", "make mod active at all times").bindTo(onAlways);
	forceUpdate = make_shared<bool>(true);
	cvarManager->registerCvar("axiscontrol_ForceUpdate", "1", "Update values even at 0 or 255").bindTo(forceUpdate);

	command1 = std::make_shared<std::string>("");
	cvarManager->registerCvar("execCommand", "", "command to execute")
		.bindTo(command1);

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [&](std::string eventName) {isInGoalReplay = true; });
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [&](std::string eventName) {isInGoalReplay = false; });

	gameWrapper->RegisterDrawable(bind(&axiscontrol::Render, this, std::placeholders::_1));

	


	
	/*
	
	*/
	gameWrapper->HookEventWithCaller<CarWrapper>("Function TAGame.Car_TA.SetVehicleInput",
		[this](CarWrapper caller, void* params, std::string eventName) {
			ControllerInput* input = reinterpret_cast<ControllerInput*>(params);
			if (buttonX != buttonY) {
				if (buttonX) {
					input->Roll = -1;
				}
				else {
					input->Roll = 1;
				}
			}
			//input->Roll = -1;
			//if (savedSteer != input->Yaw) {
			//	savedSteer = input->Yaw;
		//	}
		// 
			//cvarManager->log("" + std::to_string(input->Steer));
			// 
			//input->Yaw = input->Steer;
			// 
			//input->Throttle = 100.0f;
			//if (cvarManager->getCvar("sv_soccar_gamespeed").getFloatValue() != (.999f + .999f * savedSteer + .001f)) {
			//	cvarManager->getCvar("sv_soccar_gamespeed").setValue(.999f + .999f * savedSteer + .001f);
			//}
		});
	
}
// Function to get the precedence of an operator
int getPrecedence(char op) {
	switch (op) {
	case '+':
	case '-':
		return 1;
	case '*':
	case '/':
		return 2;
	case '^':
		return 3;
	default:
		return 0;
	}
}

// Function to apply an operator to two numbers
double applyOperator(double num1, double num2, char op) {
	switch (op) {
	case '+':
		return num1 + num2;
	case '-':
		return num1 - num2;
	case '*':
		return num1 * num2;
	case '/':
		if (num2 == 0) {
			throw std::runtime_error("Division by zero");
		}
		return num1 / num2;
	case '^':
		return pow(num1, num2);
	default:
		throw std::runtime_error("Invalid operator");
	}
}

// Function to evaluate mathematical expressions
double evaluateExpression(const std::string& expression) {
	std::stack<double> numStack;
	std::stack<char> opStack;

	for (size_t i = 0; i < expression.length(); ++i) {
		char c = expression[i];

		// If the character is a digit or a decimal point, extract the number
		if (c == '.' || std::isdigit(c)) {
			size_t start = i;
			while (i < expression.length() && (expression[i] == '.' || std::isdigit(expression[i]))) {
				++i;
			}
			--i; // Decrement i to account for the increment at the end of the loop
			double num = std::stod(expression.substr(start, i - start + 1));
			numStack.push(num);
		}
		// If the character is a minus sign
		else if (c == '-') {
			// Check if the minus sign is a unary operator
			if (i == 0 || expression[i - 1] == '(' || expression[i - 1] == '+' || expression[i - 1] == '-' || expression[i - 1] == '*' || expression[i - 1] == '/' || expression[i - 1] == '^') {
				size_t start = i + 1;
				while (i + 1 < expression.length() && (expression[i + 1] == '.' || std::isdigit(expression[i + 1]))) {
					++i;
				}
				double num = -std::stod(expression.substr(start, i - start + 1));
				numStack.push(num);
			}
			// If the minus sign is a binary operator
			else {
				while (!opStack.empty() && opStack.top() != '(' && getPrecedence(opStack.top()) >= getPrecedence(c)) {
					double num2 = numStack.top();
					numStack.pop();
					double num1 = numStack.top();
					numStack.pop();
					char op = opStack.top();
					opStack.pop();
					double result = applyOperator(num1, num2, op);
					numStack.push(result);
				}
				opStack.push(c);
			}
		}
		// If the character is an operator
		else if (c == '+' || c == '*' || c == '/' || c == '^') {
			while (!opStack.empty() && opStack.top() != '(' && getPrecedence(opStack.top()) >= getPrecedence(c)) {
				double num2 = numStack.top();
				numStack.pop();
				double num1 = numStack.top();
				numStack.pop();
				char op = opStack.top();
				opStack.pop();
				double result = applyOperator(num1, num2, op);
				numStack.push(result);
			}
			opStack.push(c);
		}
		// If the character is an opening parenthesis
		else if (c == '(') {
			opStack.push(c);
		}
		// If the character is a closing parenthesis
		else if (c == ')') {
			while (opStack.top() != '(') {
				double num2 = numStack.top();
				numStack.pop();
				double num1 = numStack.top();
				numStack.pop();
				char op = opStack.top();
				opStack.pop();
				double result = applyOperator(num1, num2, op);
				numStack.push(result);
			}
			opStack.pop(); // Remove the opening parenthesis
		}
	}

	// Apply any remaining operators
	while (!opStack.empty()) {
		double num2 = numStack.top();
		numStack.pop();
		double num1 = numStack.top();
		numStack.pop();
		char op = opStack.top();
		opStack.pop();
		double result = applyOperator(num1, num2, op);
		numStack.push(result);
	}

	return numStack.top();
}
// Function to replace variable names with their values (case-insensitive)
std::string replaceVariables(const std::string& input) {
	size_t spacePos = input.find(' ');
	if (spacePos == std::string::npos) return "";

	std::string prefix = input.substr(0, spacePos + 1);
	std::string expression = input.substr(spacePos + 1);
	std::transform(expression.begin(), expression.end(), expression.begin(), ::tolower);

	// 1. Update all variables in the map with their current live values
	variablesUsed["lt"] = LT;
	variablesUsed["rt"] = RT;
	variablesUsed["lsup"] = LSUp;
	variablesUsed["lsdown"] = LSDown;
	variablesUsed["lsleft"] = LSLeft;
	variablesUsed["lsright"] = LSRight;
	variablesUsed["rsup"] = RSUp;
	variablesUsed["rsdown"] = RSDown;
	variablesUsed["rsleft"] = RSLeft;
	variablesUsed["rsright"] = RSRight;
	variablesUsed["lsud"] = LSUD;
	variablesUsed["rsud"] = RSUD;
	variablesUsed["lslr"] = LSLR;
	variablesUsed["rslr"] = RSLR;
	variablesUsed["bothtriggers"] = bothTriggers;

	// 2. Loop through the map and replace every abbreviation found in the command
	for (auto const& [name, value] : variablesUsed) {
		size_t pos = expression.find(name);
		while (pos != std::string::npos) {
			// Check if there's a digit immediately before the variable (e.g., "2lt")
			// and insert a '*' if needed to prevent math errors
			if (pos > 0 && std::isdigit(expression[pos - 1])) {
				expression.insert(pos, "*");
				pos++;
			}

			expression.replace(pos, name.length(), std::to_string(value));
			pos = expression.find(name, pos + std::to_string(value).length());
		}
	}

	// 3. Evaluate the final math expression
	try {
		double result = evaluateExpression(expression);

		std::stringstream ss;
		ss << std::fixed << std::setprecision(2) << result;
		return prefix + ss.str();
	}
	catch (...) {
		// Returns empty if the math is invalid (e.g. division by zero)
		return "";
	}
}



void axiscontrol::onUnload() {}

ServerWrapper axiscontrol::GetCurrentGameState()
{
	if (gameWrapper->IsInReplay())
		return gameWrapper->GetGameEventAsReplay().memory_address;
	else if (gameWrapper->IsInOnlineGame())
		return gameWrapper->GetOnlineGame();
	else
		return gameWrapper->GetGameEventAsServer();
}


void axiscontrol::executeCommands() {
	// PROTECT: Never run in online games
	if (gameWrapper->IsInOnlineGame() || gameWrapper->IsInReplay()) return;

	CVarWrapper cmdCvar = cvarManager->getCvar("execCommand");
	if (!cmdCvar) return;

	std::string cmd = cmdCvar.getStringValue();
	if (cmd.empty()) return;

	std::string replaced = replaceVariables(cmd);

	if (!replaced.empty() && replaced != cmd) {
		cvarManager->executeCommand(replaced);

		static int logTimer = 0;
		if (logTimer++ % 120 == 0) {
			cvarManager->log("Executing: " + replaced);
		}
	}
}


void axiscontrol::Render(CanvasWrapper canvas)
{
	//if (gameWrapper->IsInFreeplay()) {
	//	int a = 1;
	//}
	bool freeplay = gameWrapper->IsInFreeplay();
	bool training = gameWrapper->IsInCustomTraining();
	bool online = gameWrapper->IsInOnlineGame();
	bool replay = gameWrapper->IsInReplay();

	if (!freeplay && !training && !online && !replay) return;//make sure its in a valid gamestate first
	if (online && isInGoalReplay) return;//return if camera is in goal replay
	CameraWrapper camera = gameWrapper->GetCamera();
	if (camera.IsNull()) return;
	if (!*workWhileSpectating)
	{
		if (replay || online)
			return;//return if user doesnt want it to work when spectating
	}
	else
	{
		if (online && !gameWrapper->GetLocalCar().IsNull())
			return;//only allow it in online if player is spectating (controllable car does not exist)
	}
	PriWrapper target = PriWrapper(reinterpret_cast<std::uintptr_t>(gameWrapper->GetCamera().GetViewTarget().PRI));
	if (target.IsNull()) return;
	CarWrapper car = target.GetCar();
	if (car.IsNull()) return;

	/*
	cvarManager->log("LT: " + std::to_string( LT ));
	cvarManager->log("RT: " + std::to_string( RT ));
	cvarManager->log("LSUp: " + std::to_string( LSUp ));
	cvarManager->log("LSDown: " + std::to_string( LSDown ));
	cvarManager->log("LSLeft: " + std::to_string( LSLeft ));
	cvarManager->log("LSRight: " + std::to_string( LSRight ));
	cvarManager->log("RSUp: " + std::to_string( RSUp ));
	cvarManager->log("RSDown: " + std::to_string( RSDown ));
	cvarManager->log("RSLeft: " + std::to_string( RSLeft ));
	cvarManager->log("RSRight: " + std::to_string( RSRight ));
	cvarManager->log("LSUD: " + std::to_string( LSUD ));
	cvarManager->log("RSUD: " + std::to_string( RSUD ));
	cvarManager->log("LSLR: " + std::to_string( LSLR ));
	cvarManager->log("RSLR: " + std::to_string( RSLR ));
	cvarManager->log("bothTriggers: " + std::to_string( bothTriggers ));
	
	cvarManager->log("buttonX: " + std::to_string(buttonX));
	cvarManager->log("buttonY: " + std::to_string(buttonY));
	*/
	

		//if (cvarManager->getCvar("sv_soccar_gamespeed").getFloatValue() != (1.0f + 1.0f * LSUD)) {
		//	cvarManager->getCvar("sv_soccar_gamespeed").setValue(1.0f + 1.0f * LSUD);
		//	cvarManager->log("" + std::to_string(LSUD));
		//}



	GetControllerInput(cvarManager, forceUpdate);
	executeCommands();

}
