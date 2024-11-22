# thorlabs_MCM301
Python device adaptor: Thorlabs MCM301 three-channel stepper motor controller for Cerna components.
## Quick start:
- Install the 'Thorlabs MCM301' GUI (from Thorlabs) and check the controller driver. It should be 
straightforward to run the GUI and control a stage (GUI version 1.2.0 and stage MPM250/M used here).
- The GUI should install the device drivers and include a copy of the essential "MCM301Lib_x64.dll" file (a version included here for convenience).
- For Python control, download and run "thorlabs_MCM301.py" with a copy of the .dll file in the same folder.

![social_preview](https://github.com/amsikking/thorlabs_MCM301/blob/main/social_preview.png)

## Details:
- This adaptor was generated with reference to the "MCM301CommandLibrary.h" header and "MCM301_COMMAND_LIB_EXAMPLE.py" files (versions included here for convenience) that were installed by the GUI at location:
  - C:\Program Files (x86)\Thorlabs\MCM301\Sample 
- The essential "MCM301Lib_x64.dll" file was found here:
  - C:\Program Files (x86)\Thorlabs\MCM301\Sample\Thorlabs_MCM301_PythonSDK
- Writing the adaptor was somewhat tricky hence the use of both the C (.h) and Python (.py) SDK files.**

**Note: currently the "set_velocity" method is not working and causing a move to the limit switch! For this reason the method currently prints a warning and returns without calling the .dll**
