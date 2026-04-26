## Axis-Control
Mod that allows you to control any game variable using triggers or joystick values.

### WARNING: 
This mod currently only detects input from Xbox controllers using XInput, if you use a PS4 controller, you will need to use an external program like DS4 Windows to simulate a Xbox controller for the mod to work.

## How to use:
To use the mod, open your BakkesMod console (F6) and type:
aCommand1 "[Your Command Here]"
Using the logic variables described below in any simple mathematical expression.

### Examples:
Here are some examples of starter commands that work (remember to put the command to execute in quotes):

#### Joystick Gravity: 
change gravity with right stick (push up to lower gravity and down for stronger gravity)
aCommand1 "sv_soccar_gravity 650*RSUD-650"
(default gravity is -650).

#### Control game time: 
Pull Left trigger to slow down time to any speed you want (very good for practicing):
aCommand2 "sv_soccar_gamespeed 1 - LT"
Game speed will be updated every frame that LT's value changes.

#### Boost Power: 
Scale boost with Right Trigger (very fun):
aCommand3 "sv_soccar_boostmodifier 1 + rt * 5"

#### Note:
The mod can even change values of any other mod that is loaded, for example:
aCommand4 "car_size_scale_blue 1+RSUD"
Updates a variable of the CarSize plugin.

## Logic Variables:
None of the following variables are case sensitive.

#### Trigger variables:
LT: [0.0 to 1.0]
RT: [0.0 to 1.0]
BT: Short for both triggers, this value is always RT - LT [-1.0 to 1.0]

#### Joysticks (Directional):
all of the following values are stored in the range [0.0 to 1.0]
LSup
LSdown
LSleft
LSright
RSup
RSdown
RSleft
RSright

#### Joysticks (Full Axis)
LSUD (Y axis, [-1.0 (Down) to 1.0 (Up)])
LSLR (X axis, [-1.0 (Left) to 1.0 (Right)])
RSUD (Y axis, [-1.0 (Down) to 1.0 (Up)])
RSLR (X axis, [-1.0 (Left) to 1.0 (Right)])

## Rules for Writing Commands
Math: Supports +, -, *, /, ^ (exponents), and () (parentheses).
The mod currently does not support any more advanced mathematical functions.

## Mod Management:
Up to 9 commands can be running at the same time (aCommand1-aCommand9).
To clear a command, type:
aCommand1 ""
into the f6 console to clear that command.

## Other commands and mod variables:
#### axiscontrol_workWhileSpectating: 
Value either 0 for false or 1 for true. Defaulted to false. When true, enables the mod to work while spectating.

#### axiscontrol_LogMode: 
(0 1 or 2). 0 (default) means avoid logging commands to the console whenever possible. 1 makes it log the console every time a controller value is changed. 2 makes it log commands to the console every couple of seconds no matter what for debugging purposes.

#### axiscontrol_ForceExecute: 
Defaulted to 0 (false). To avoid spamming the console, the mod tries to retrieve a Bakkes mod variable and directly change its value whenever possible. Setting this value to 1 will always send execution commands to the console which could make the mod more reliable at the cost of visually flooding the console with commands.

#### axiscontrol_logInputs: 
logs every controller input to the console every frame (helpful for determining whether your controller is being detected properly.)

#### axiscontrol_ControllerIndex: 
starts out at -1 which corresponds to "search mode". While in search mode, the mod checks all 4 slots that a controller can be connected in. Whenever the mod first detects input from any one of those slots, this value is updated to the corresponding controller slot (0, 1, 2, or 3) and the mod gets locked to that slot, no longer checking any other slots for inputs. You can set this value to 0-3 to manually set a controller slot, or set it to -1 to put the mod back into search mode.

## One final remark: 
Use with caution. I have implemented some safe guards, but if you type the wrong command that runs at the wrong time, it could potentially cause a crash. Also the mod does not work in online games and bakkes mod has preventative measures to prevent you from changing something you shouldn't be able to. With that being said, (this should go without saying) please do not use this mod for malicious purposes or to somehow gain a competitive advantage if a method of doing so is ever found.
