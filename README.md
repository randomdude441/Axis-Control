This is a Rocket League mod that allows you to control any game variable using triggers or joystick values.

WARNING: This mod currently only detects input from Xbox controllers using XInput, if you use a PS4 or other controller, you will need to use an external program like DS4 Windows to simulate a Xbox controller for the mod to work.

To use the mod, open your BakkesMod console (F6) and type:
aCommand1 "[Your Command Here]" 
Using the logic variables below.

You can set up to 9 different commands at once, "aCommand1"- "aCommand9". They all work the same and the order that you set them does not matter. You can type any simple mathematical expression containing one of the below variables. Every frame that a controller input variable used within an active aCommand is updated, the mod will 
1. replace each variable in the command with their corresponding values representing the state of that input (0.00 to 1.00 normally, or -1.00 to 1.00 for full stick ranges).
2. calculate any mathematical expressions within the command
3. send the updated command to the Bakkes mod console.

Logic Variables:
None of the following variables are case sensitive, which means the variable name can be all capital, all lowercase, or anything in between and it will work the same.

Trigger variables:
LT: Left Trigger, [0.0 to 1.0]
RT: Right Trigger, [0.0 to 1.0]
BT: Short for both triggers, this value is always RT - LT [-1.0 to 1.0]

Joysticks (Directional)
LSup (Left Stick Up, [0.0 to 1.0])
LSdown (Left Stick Down, [0.0 to 1.0])
LSleft (Left Stick Left, [0.0 to 1.0])
LSright (Left Stick Right, [0.0 to 1.0])
RSup (Right Stick Up, [0.0 to 1.0])
RSdown (Right Stick Down, [0.0 to 1.0])
RSleft (Right Stick Left, [0.0 to 1.0])
RSright (Right Stick Right, [0.0 to 1.0])

Joysticks (Full Axis)
LSUD (Left Stick Up/Down, [-1.0 (Down) to 1.0 (Up)])
LSLR (Left Stick Left/Right, [-1.0 (Left) to 1.0 (Right)])
RSUD (Right Stick Up/Down, [-1.0 (Down) to 1.0 (Up)])
RSLR (Right Stick Left/Right, [-1.0 (Left) to 1.0 (Right)])

Quick example:
aCommand1 “sv_soccar_gamespeed 1-LT”
This command makes time slow down the more that the left trigger is pressed. Game speed will be updated every frame that LT's value changes.

3. Examples
Here are some examples of commands that work (remember to put the command to execute in quotes):

Joystick Gravity: change gravity with right stick (push up for 0 gravity and down for stronger gravity)
aCommand1 "sv_soccar_gravity (-650 + RSUD * 650)"

Control game time: Pull Left trigger to slow down time (very good for practicing):
aCommand2 "sv_soccar_gamespeed 1 - LT"

Boost Power: Scale boost with Right Trigger (very fun):
aCommand3 "sv_soccar_boostmodifier 1 + rt * 5"

For all of the above commands, you can replace the controller variable and the mathematical calculations with any controller variable that you want listed above. Additionally, you can make it change any Bakkes mod value you want, not just these. Because the mod works simply by sending commands to the f6 console, it will even work with any other Bakkes mod plugin that is loaded at the same time. Any variable that accepts decimal values and can be changed through the f6 console, this mod will work with.

2. Rules for Writing Commands
Math: Supports +, -, *, /, ^ (exponents), and () (parentheses).
Mathematical formatting in commands: The parsing logic should interpret mathematical abbreviations like 100rt as 100 * rt. It should also calculate fine if there are spaces between operators (like 4 * 5 instead of 4*5). However, minimizing unnecessary spaces and making sure to explicitly add operators will minimize the chance of a potential error. Also make sure to add operators between two variables (like write LT*RT, not just LTRT, as that may cause errors). Also note that complex mathematical operators and functions will not work (such as trigonometric functions like sin(x), summations, i(square root of -1), log(x) etc), though support for some of these more complex functions may be added in the future, especially if it is desired by users.

4. Mod Management
Number of commands: Up to 9 commands running at the same time (aCommand1-aCommand9). 
To clear a command type:
aCommand1 "" 
into the f6 console to that command. aCommand2 "" would clear command 2 and so on.

Other commands and mod variables:
axiscontrol_workWhileSpectating: Value either 0 for false or 1 for true. Defaulted to false. When true, enables the mod to work while spectating.

axiscontrol_LogMode: (0 1 or 2). 0 means avoid logging commands to the console whenever possible. 1 makes it log the console every time a controller value is changed. 2 makes it log commands to the console every couple of seconds no matter what for debugging purposes.

axiscontrol_ForceExecute: Defaulted to 0 (false). To avoid spamming the console, the mod tries to retrieve a Bakkes mod value and directly change its value whenever possible. Setting this value to 1 will always send execution commands to the console which could make the mod more reliable at the cost of visually flooding the console with commands.

axiscontrol_logInputs: logs every controller input to the console every frame (helpful for determining whether your controller is being detected properly.)

axiscontrol_ControllerIndex: starts out a -1 which corresponds to "search mode". While in search mode, the mod checks all 4 slots that a controller can be connected in. Whenever the mod first detects input from any one of those slots, this value is updated to the corresponding controller slot (0, 1, 2, or 3) and the mod gets locked to that slot, no longer checking any other slots for inputs. You can set this value to 0-3 to manually set a controller slot, or set it to -1 to put the mod back into search mode. 

Fun tip: While gyro inputs from the controller cannot be directly detected by the mod, you can use a software like DS4 windows to make gyro be bound to other axis values. So, for example, you can assign gyro values to the right joystick, after doing that, you can then use RS variables in exec commands to control Bakkes mod variables with the gyro of the controller. I like to make it to where tilting my controller up and down controls gamespeed through using this method. 

One final remark: Use with caution. This mod gives you total control to do whatever you want with it, but my game crashed multiple times in developing and testing this mod because I left a command running that tried to change a value that it should not be able to. I have implemented safety guards to hopefully prevent that from happening to you, but if you type the wrong command that runs at the wrong time, it could potentially cause a crash. Also the mod does not work in online games so it should be impossible to gain a competitive advantage with it. Also, because all it does is change Bakkes mod variables, there should already be preventions in place elsewhere that prevent you from changing something you shouldn't. With that being said, (this should go without saying) please do not use this mod for malicious purposes or to somehow gain a competitive advantage against others if a method of doing so is ever found.





