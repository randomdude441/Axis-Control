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
int lockedControllerIndex = -1; // Initialized to -1 (No controller locked)
float LT = 0;
float RT = 0;
float LSUp = 0;
float LSDown = 0;
float LSLeft = 0;
float LSRight = 0;
float RSUp = 0;
float RSDown = 0;
float RSLeft = 0;
float RSRight = 0;
float LSUD = 0;
float RSUD = 0;
float LSLR = 0;
float RSLR = 0;
float bothTriggers = 0;
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
		{"bt", bothTriggers},
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




void axiscontrol::onLoad()
{

	for (int i = 0; i < 9; i++) {
		std::string cvarName = "aCommand" + std::to_string(i + 1);
		execCommands[i] = std::make_shared<std::string>("");
		cvarManager->registerCvar(cvarName, "", "Command " + std::to_string(i + 1) + " to execute").bindTo(execCommands[i]);

		// FORCE SYNC
		// Manually reach into the engine and pull the text immediately.
		CVarWrapper cv = cvarManager->getCvar(cvarName);
		if (cv) {
			*execCommands[i] = cv.getStringValue();
		}
		lastExecutedCommands[i] = "";
	}
	// Default manual index to -1 (Auto)
	manualControllerIndex = std::make_shared<int>(-1);
	cvarManager->registerCvar("axiscontrol_ControllerIndex", "-1", "Locked controller index (-1 = Auto-find)", true, true, -1, true, 3).bindTo(manualControllerIndex);

	showDebugInputs = std::make_shared<bool>(false);
	cvarManager->registerCvar("axiscontrol_logInputs", "0", "Show live controller values in console", true, true, 0, true, 1).bindTo(showDebugInputs);
	forceExecuteCommand = make_shared<bool>(false);
	cvarManager->registerCvar("axiscontrol_ForceExecute", "0", "Always use executeCommand").bindTo(forceExecuteCommand);
	logMode = make_shared<int>(0); // Default to 0 (no console logs whenever possible)
	cvarManager->registerCvar("axiscontrol_LogMode", "0", "0=None, 1=On Change, 2=Always", true, true, 0, true, 2).bindTo(logMode);
	//cvarManager->log("WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW ");
	workWhileSpectating = make_shared<bool>(false);
	cvarManager->registerCvar("axiscontrol_workWhileSpectating", "0", "work while spectating").bindTo(workWhileSpectating);
	//cvarManager->registerCvar("on_Always", "0", "make mod active at all times").bindTo(onAlways);
	

	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.BeginState", [&](std::string eventName) {isInGoalReplay = true; });
	gameWrapper->HookEvent("Function GameEvent_Soccar_TA.ReplayPlayback.EndState", [&](std::string eventName) {isInGoalReplay = false; });

	gameWrapper->RegisterDrawable(bind(&axiscontrol::Render, this, std::placeholders::_1));





	/*

	
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
		*/
}

void axiscontrol::GetControllerInput(std::shared_ptr<CVarManagerWrapper> cvarManager){
	XINPUT_STATE state;

	// 1. Sync the locked index with the manual CVar

	lockedControllerIndex = *manualControllerIndex;

	// 2. If index is -1, search all ports for any input
	if (lockedControllerIndex == -1) {
		for (DWORD i = 0; i < 4; i++) {
			ZeroMemory(&state, sizeof(XINPUT_STATE));
			if (XInputGetState(i, &state) == ERROR_SUCCESS) {

				// Check for any significant input (Triggers or Sticks)
				bool hasInput = (state.Gamepad.bLeftTrigger > 10 || state.Gamepad.bRightTrigger > 10 ||
					abs(state.Gamepad.sThumbLX) > 5000 || abs(state.Gamepad.sThumbLY) > 5000 ||
					abs(state.Gamepad.sThumbRX) > 5000 || abs(state.Gamepad.sThumbRY) > 5000);

				if (hasInput) {
					// controller found. Update the CVar so we stay locked to it
					cvarManager->getCvar("axiscontrol_ControllerIndex").setValue((int)i);
					return; // Next frame will use the locked index logic below
				}
			}
		}
	}
	// 3. If an index is locked, only check that specific port
	else {
		ZeroMemory(&state, sizeof(XINPUT_STATE));
		if (XInputGetState((DWORD)lockedControllerIndex, &state) == ERROR_SUCCESS) {

			// Assign ALL variables from the locked controller
			LT = state.Gamepad.bLeftTrigger / 255.0f;
			RT = state.Gamepad.bRightTrigger / 255.0f;
			bothTriggers = RT - LT;

			LSLR = (state.Gamepad.sThumbLX >= 0) ? (state.Gamepad.sThumbLX / 32767.0f) : (state.Gamepad.sThumbLX / 32768.0f);
			LSUD = (state.Gamepad.sThumbLY >= 0) ? (state.Gamepad.sThumbLY / 32767.0f) : (state.Gamepad.sThumbLY / 32768.0f);
			RSLR = (state.Gamepad.sThumbRX >= 0) ? (state.Gamepad.sThumbRX / 32767.0f) : (state.Gamepad.sThumbRX / 32768.0f);
			RSUD = (state.Gamepad.sThumbRY >= 0) ? (state.Gamepad.sThumbRY / 32767.0f) : (state.Gamepad.sThumbRY / 32768.0f);

			// Breakouts...
			LSUp = (LSUD > 0) ? LSUD : 0; LSDown = (LSUD < 0) ? -LSUD : 0;
			LSLeft = (LSLR < 0) ? -LSLR : 0; LSRight = (LSLR > 0) ? LSLR : 0;
			RSUp = (RSUD > 0) ? RSUD : 0; RSDown = (RSUD < 0) ? -RSUD : 0;
			RSLeft = (RSLR < 0) ? -RSLR : 0; RSRight = (RSLR > 0) ? RSRight : 0;

			buttonX = (state.Gamepad.wButtons & XINPUT_GAMEPAD_X) != 0;
			buttonY = (state.Gamepad.wButtons & XINPUT_GAMEPAD_Y) != 0;
		}
		else {
			// Controller disconnected? Revert to auto-find
			cvarManager->getCvar("axiscontrol_ControllerIndex").setValue(-1);
		}
	}
}



//int main() {
//	while (true) {

	//	Sleep(1);  // Wait 100ms between checks (adjust as needed)
	//}
//	return 0;
//}




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
// Helper to identify "Placeholder" minus
const char MINUS_PLACEHOLDER = '~';

std::string replaceVariables(const std::string& input) {
	std::string output = input;
	std::string lowerOutput = input;
	std::transform(lowerOutput.begin(), lowerOutput.end(), lowerOutput.begin(), ::tolower);

	// 1. Update Map
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
	variablesUsed["bt"] = bothTriggers;

	for (size_t i = 0; i < output.length(); ++i) {
		if (i > 0) {
			// Case: 1000( -> 1000*(
			if (output[i] == '(' && std::isdigit(output[i - 1])) {
				output.insert(i, "*");
				lowerOutput.insert(i, "*");
				i++;
			}
			// Case: )1000 -> )*1000 or )rt -> )*rt
			else if (output[i - 1] == ')' && (std::isdigit(output[i]) || std::isalpha(output[i]))) {
				output.insert(i, "*");
				lowerOutput.insert(i, "*");
				i++;
			}
		}
	}
	for (auto const& [name, value] : variablesUsed) {
		size_t pos = lowerOutput.find(name);
		while (pos != std::string::npos) {
			// Logic 1: Insert '*' if preceded by a digit (e.g. 2lt -> 2*lt)
			if (pos > 0 && (std::isdigit(lowerOutput[pos - 1]))) {
				output.insert(pos, "*");
				lowerOutput.insert(pos, "*");
				pos++;
			}

			// Logic 2: Format the value
			// If negative, use ~1.5. If positive, use 1.5
			std::stringstream ss;
			ss << std::fixed << std::setprecision(4);
			if (value < 0) {
				ss << MINUS_PLACEHOLDER << std::abs(value);
			}
			else {
				ss << value; 
			}
			std::string valStr = ss.str();

			output.replace(pos, name.length(), valStr);
			lowerOutput.replace(pos, name.length(), valStr);

			pos = lowerOutput.find(name, pos + valStr.length());
		}
	}
	return output;
}

bool isOperator(char c) {
	return c == '+' || c == '-' || c == '*' || c == '/' || c == '^' || c == '(' || c == ')';
}

// Function to clean spaces around operators but keep spaces between distinct numbers
// Input: "bakkes 5 + ~3"  -> Output: "bakkes 5+~3"
// Input: "set_pos 5 ~3"   -> Output: "set_pos 5 ~3" (Space preserved)
std::string squashOperators(const std::string& input) {
	std::string result = "";
	for (size_t i = 0; i < input.length(); ++i) {
		char c = input[i];
		if (c == ' ') {
			// Check if this space is "glue" for an operator
			bool prevIsOp = (i > 0 && isOperator(input[i - 1]));
			bool nextIsOp = (i + 1 < input.length() && isOperator(input[i + 1]));

			// If neither side is an operator, it's a real separator (keep the space)
			if (!prevIsOp && !nextIsOp) {
				result += c;
			}
		}
		else {
			result += c;
		}
	}
	return result;
}

// Function to break the string into chunks, evaluate math, and rebuild
std::string solveComplexCommand(std::string input) {
	// 1. Remove spaces that are part of math equations
	std::string squashed = squashOperators(input);

	std::stringstream resultSS;
	std::stringstream tokenStream(squashed);
	std::string segment;

	bool first = true;

	// 2. Split by remaining spaces
	while (std::getline(tokenStream, segment, ' ')) {
		if (segment.empty()) continue;
		if (!first) resultSS << " ";
		first = false;

		// 3. Detect if this segment needs math evaluation
		// It needs math if it has an operator OR the placeholder minus
		bool needsMath = false;
		for (char c : segment) {
			if (isOperator(c) || c == MINUS_PLACEHOLDER) {
				needsMath = true;
				break;
			}
		}

		if (needsMath) {
			// Restore the real minus sign
			std::string mathString = segment;
			std::replace(mathString.begin(), mathString.end(), MINUS_PLACEHOLDER, '-');

			try {
				float val = (float)evaluateExpression(mathString);
				// remove trailing zeros logic could go here, but simple to_string is safe
				resultSS << std::to_string(val);
			}
			catch (...) {
				// If math fails (e.g. it was just a word with a hyphen), keep original
				// But we must restore the placeholder in the original text if it existed
				std::replace(segment.begin(), segment.end(), MINUS_PLACEHOLDER, '-');
				resultSS << segment;
			}
		}
		else {
			// No math here (just a word like "set_pos" or a plain number "4")
			resultSS << segment;
		}
	}
	return resultSS.str();
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

	static int logTimer = 0;
	logTimer++;

	// Loop through all 9 possible command slots
	for (int i = 0; i < 9; i++) {
		std::string cmd = *execCommands[i];
		if (cmd.empty()) continue; // Skip if the user cleared the command

		// 1. Variable Replacement (Using your Placeholder logic)
		std::string replaced = replaceVariables(cmd);
		if (replaced.empty() || replaced == cmd) continue;

		// 2. Solve the command (Math and Token logic)
		std::string finalCommandString = solveComplexCommand(replaced);

		// 3. Determine Logging/Execution needs
		bool shouldLogToConsole = false;
		bool shouldExecute = false;

		if (*logMode == 2) {
			if (logTimer % 120 == 0) { shouldLogToConsole = true; shouldExecute = true; }
			else if (finalCommandString != lastExecutedCommands[i]) { shouldExecute = true; }
		}
		else if (*logMode == 1) {
			if (finalCommandString != lastExecutedCommands[i]) { shouldLogToConsole = true; shouldExecute = true; }
		}
		else if (*logMode == 0) {
			if (finalCommandString != lastExecutedCommands[i]) { shouldExecute = true; }
		}

		// 4. Execution
		if (shouldExecute) {
			bool silentSuccess = false;

			// Try silent setValue if not forcing complex and no logging required
			if (!*forceExecuteCommand && !shouldLogToConsole) {
				size_t spacePos = finalCommandString.find(' ');
				if (spacePos != std::string::npos && finalCommandString.find(' ', spacePos + 1) == std::string::npos) {
					std::string cvarName = finalCommandString.substr(0, spacePos);
					try {
						float val = std::stof(finalCommandString.substr(spacePos + 1));
						CVarWrapper target = cvarManager->getCvar(cvarName);
						if (target) { target.setValue(val); silentSuccess = true; }
					}
					catch (...) {}
				}
			}

			if (!silentSuccess) {
				cvarManager->executeCommand(finalCommandString, shouldLogToConsole);
			}

			lastExecutedCommands[i] = finalCommandString;
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
	//PriWrapper target = PriWrapper(reinterpret_cast<std::uintptr_t>(gameWrapper->GetCamera().GetViewTarget().PRI));
	//if (target.IsNull()) return;
	//CarWrapper car = target.GetCar();
	//if (car.IsNull()) return;

	if (*showDebugInputs) {
		cvarManager->log("_________________________________");
		cvarManager->log("LT: " + std::to_string(LT));
		cvarManager->log("RT: " + std::to_string(RT));
		cvarManager->log("BT: " + std::to_string(bothTriggers));
		cvarManager->log("LSUp: " + std::to_string(LSUp));
		cvarManager->log("LSDown: " + std::to_string(LSDown));
		cvarManager->log("LSLeft: " + std::to_string(LSLeft));
		cvarManager->log("LSRight: " + std::to_string(LSRight));
		cvarManager->log("RSUp: " + std::to_string(RSUp));
		cvarManager->log("RSDown: " + std::to_string(RSDown));
		cvarManager->log("RSLeft: " + std::to_string(RSLeft));
		cvarManager->log("RSRight: " + std::to_string(RSRight));
		cvarManager->log("LSUD: " + std::to_string(LSUD));
		cvarManager->log("RSUD: " + std::to_string(RSUD));
		cvarManager->log("LSLR: " + std::to_string(LSLR));
		cvarManager->log("RSLR: " + std::to_string(RSLR));
	}
	/*
	cvarManager->log("buttonX: " + std::to_string(buttonX));
	cvarManager->log("buttonY: " + std::to_string(buttonY));
	*/


	//if (cvarManager->getCvar("sv_soccar_gamespeed").getFloatValue() != (1.0f + 1.0f * LSUD)) {
	//	cvarManager->getCvar("sv_soccar_gamespeed").setValue(1.0f + 1.0f * LSUD);
	//	cvarManager->log("" + std::to_string(LSUD));
	//}



	GetControllerInput(cvarManager);
	executeCommands();

}
