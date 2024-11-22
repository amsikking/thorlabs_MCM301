#ifndef MCM301_COMMAND_LIBRARY
#define MCM301_COMMAND_LIBRARY

#ifdef DLL_EXPORT
#define DLL_API __declspec(dllexport)
#else
#define DLL_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#include "MCM301TypeDef.h"
	/**
	 * @brief List all the possible port on this computer.
	 * 
	 * @param buffer port list returned string include device name,serial number, device descriptor and vendor name, separated by comma.
	 * @param buffer_length the length of buffer.
	 * @return number of device in the list.
	 * @retval "non-negative number" number of device in the list.
	 * @retval "negative number" failed.
	 */
	DLL_API int List(char* buffer, int buffer_length);
	/**
	 * @brief Open port function.
	 * 
	 * @param sn serial number of the device to be opened.
	 * @param nBaud bit per second of port.
	 * @param timeout set timeout value in (s).
	 * @return the unique handle pointing the port.
	 * @retval "non-negative number" hdl number.
	 * @retval "negative number" failed.
	 */
	DLL_API int Open(char* sn, int nBaud, int timeout);
	/**
	 * @brief Check opened status of port.
	 * 
	 * @param sn serial number.
	 * @return port state.
	 * @retval 0 port is not opened.
	 * @retval 1 port is opened.
	 * @retval "negative number" failed.
	 */
	DLL_API int IsOpen(char* sn);
	/**
	 * @brief Close port function.
	 * 
	 * @param hdl handle of port.
	 * @return function execution state. 
	 * @retval "non-negative number" hdl number.
	 * @retval "negative number" failed.
	 */
	DLL_API int Close(int hdl);
	/**
	 * @brief Get the device error state.
	 * 
	 * @param hdl handle of port.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int GetErrorState(int hdl);
	/**
	 * @brief Get handle of port.
	 * 
	 * @param serialNo serial number of the device to be checked.
	 * @return hdl number.
	 * @retval "non-negative number" hdl number.
	 * @retval "negative number" failed.
	 */
	DLL_API int GetHandle(char* serialNo);
	/**
	 * @brief Set enable or disable a stepper.
	 * 
	 * @param hdl handle of port.
	 * @param slot the target slot (4,5,6).
	 * @param enable_state 0: disable the stepper;\n
	 * 1: enable the stepper.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetChanEnableState(int hdl, char slot, char enable_state);
	/**
	 * @brief Set the jogging parameters for the slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6).
	 * @param step_size jog step size (encoder).
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetJogParams(int hdl, char slot, unsigned int step_size);
	/**
	 * @brief Set the encoder count of the stepper to the provided value.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6).
	 * @param encoder_count the encoder count of target slot.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetMOTEncCounter(int hdl, char slot, int encoder_count);
	/**
	 * @brief Set the title of slot.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6).
	 * @param title user-defined title for the slot.
	 * @param title_length the length of title. (it should be larger than 0 and less than 16)
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetSlotTitle(int hdl, char slot, char* title, int title_length);
	/**
	 * @brief Set the maximum brightness of the LEDs connected to the controller.
	 * 
	 * @param hdl handle of port.
	 * @param dim 0<= dim <=100.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetSystemDim(int hdl, char dim);
	/**
	 * @brief Set the soft limits of the slot based on the current encoder position.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6).
	 * @param mode 1: set the counter-clockwise soft limit to the current encoder position;\n
	 * 2: set the clockwise high soft limit to the current encoder position;\n
	 * 3: remove both the high and low soft limits.
	 * 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetSoftLimit(int hdl, char slot, char mode);
	/**
	 * @brief Set the software limit by value.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6).
	 * @param cw_value cw encoder limit, default value:2147483647.
	 * @param ccw_value ccw encoder limit, default value:-2147483648.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetSoftLimitValue(int hdl, char slot, int cw_value, int ccw_value);
	/**
	 * @brief Set the software limit to EEPROM.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetEEPROMPARAMSSoftLimit(int hdl, char slot);
	/**
	 * @brief Set the home info to EEPROM.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetEEPROMPARAMSHome(int hdl, char slot);
	/**
	 * @brief Set the Jog Parameters to EEPROM.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed.
	 */
	DLL_API int SetEEPROMPARAMSJogParams(int hdl, char slot);
	/**
	 * @brief Get the enabled status of stepper.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param enable_state 0: stepper is disabled;
	 * 1: stepper is enabled.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetChanEnableState(int hdl, char slot, char* enable_state);
	/**
	 * @brief Get the maximum brightness of the LEDs connected to the controller.
	 * 
	 * @param hdl handle of port.
	 * @param dim 0<= dim <=100.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetSystemDim(int hdl, char* dim);
	/**
	 * @brief Get the title of slot.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param title user-defined title for the slot.
	 * @param buffer_length the length of title.(it should be larger than 16)
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetSlotTitle(int hdl, char slot, char* title, int buffer_length);
	/**
	 * @brief Get the jogging parameters for the slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param jog_step_size set encoder count per step of the stepper.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetJogParams(int hdl, char slot, unsigned int* jog_step_size);
	/**
	 * @brief Get hardware information from the controller.
	 * 
	 * @param hdl handle of port.
	 * @param firmware_version stored in minor, interim, major order.
	 * @param firmware_version_buffer_len the length of firmware version buffer, it should be larger than 3.
	 * @param cpid_version stored in major, minor order.
	 * @param cpid_version_buffer_len the length of CPID version buffer, it should be larger than 2.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetHardwareInfo(int hdl, char* firmware_version, int firmware_version_buffer_len, char* cpid_version, int cpid_version_buffer_len);
	/**
	 * @brief Get general information (i.e. status) of the stepper driver.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param current_encoder For stages with an encoder, this represents the value from the encoder. For stages without an encoder, this equals to the position divided by the counts per unit of this stepper.
	 * @param status_bit 0x01:On Clockwise Hardware Limit Switch.\n
	 * 0x02:On Counter-Clockwise Hardware Limit Switch.\n
	 * 0x04:On Clockwise Software Limit Switch.\n
	 * 0x08:On Counter-Clockwise Software Limit Switch.\n
	 * 0x10:Moving Clockwise Stage is in motion.\n
	 * 0x20:Moving Counter-Clockwise Stage is in motion.\n
	 * 0x40:Jogging Clockwise Stage is in motion.\n
	 * 0x80:Jogging Counter-Clockwise Stage is in motion.\n
	 * 0x100:Motor Connected The motor has been recognized by the controller.\n
	 * 0x200:Homing Stage is in motion.Homed.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetMotStatus(int hdl, char slot, int* current_encoder, unsigned int* status_bit);
	/**
	 * @brief Get the plug-and-play status of a given slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param status 0x00:Normal.0x01: no Device Connected.\n
	 * 0x02: general One-Wire Error.\n
	 * 0x04: unknown One-Wire Version.\n
	 * 0x08: One-Wire Corruption.\n
	 * 0x10: serial Number Mismatch.\n
	 * 0x20: device Signature Not Allowed.\n
	 * 0x40: general Configuration Error.\n
	 * 0x80: device Configuration Set Miss.\n
	 * 0x100: configuration Struct Miss.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetPNPStatus(int hdl, char slot, unsigned int* status);
	/**
	 * @brief Get the temperature sensors, high-voltage input, and slot card error bits.
	 * 
	 * @param hdl handle of port.
	 * @param border_status_struct the information of board.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetBoardStatus(int hdl, BoardStatusInfoStruct* border_status_struct);
	/**
	 * @brief Get stage parameters of this slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param stage_params_info the stage parameters.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetStageParams(int hdl, char slot, StageParamsInfoStruct* stage_params_info);
	/**
	 * @brief Get identifying information on a device.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param device_type null-terminated part number for the connected device.
	 * @param device_type_length the length of "device_type" buffer.(it should be larger than 16)
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetSlotDeviceType(int hdl, char slot, char* device_type, int device_type_length);
	/**
	 * @brief Get the saved limit switch parameters for this slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param set_software_limit_cw indicate if has software cw restriction.\n
	 * 0: not exist.\n
	 * 1: exist.
	 * @param soft_limit_cw the clockwise software limit.
	 * @param set_software_limit_ccw indicate if has software ccw restriction.\n
	 * 0: not exist.\n
	 * 1: exist.
	 * @param soft_limit_ccw the counter-clockwise software limit.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetSoftwareLimit(int hdl, char slot, int* set_software_limit_cw, int* soft_limit_cw, int* set_software_limit_ccw, int* soft_limit_ccw);
	/**
	 * @brief Sent to initiate the LED identification sequence on the controller(power LED) as well as any HID LEDs for controls mapped to the given slot.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int ChanIdentify(int hdl, char slot);
	/**
	 * @brief Begin a homing movement.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int Home(int hdl, char slot);
	/**
	 * @brief Sets the velocity and direction of a slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param direction 0: counter-clockwise.\n
	 * 1: clockwise.
	 * @param velocity Percentage of the maximum velocity for the slot card. 0<= velocity <=100.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetVelocity(int hdl, char slot, char direction, char velocity);
	/**
	 * @brief Stop any motion on this stepper.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int MoveStop(int hdl, char slot);
	/**
	 * @brief Move the stepper to a specified encoder position.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param target_encoder the encoder count of target position.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int MoveAbsolute(int hdl, char slot, int target_encoder);
	/**
	 * @brief Start a jog movement in the specified direction.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param direction 0: Counter-Clockwise;\n
	 * 1: Clockwise
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int MoveJog(int hdl, char slot, char direction);
	/**
	 * @brief Remove any custom configurations on a given slot card, falling back on the default configuration of controller for the connected device(if one exists).
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int EraseConfiguration(int hdl, char slot);
	/**
	 * @brief Manually restart the board.
	 * 
	 * @param hdl handle of port.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int RestartBoard(int hdl);
	/**
	 * @brief Converter raw encoder to nm
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param encoder_count encoder count.
	 * @param nm the value with unit nm convert from encoder count.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int ConvertEncoderTonm(int hdl, char slot, int encoder_count, double* nm);
	/**
	 * @brief Converter nm to raw encoder.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param nm the value with unit nm.
	 * @param encoder_count the value with unit encoder count convert from nm.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int ConvertnmToEncoder(int hdl, char slot, double nm, int* encoder_count);
	/**
	 * @brief Get the hardware info for the embedded file system of MCM device.
	 * 
	 * @param hdl handle of port.
	 * @param info EFS hardware information.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetEFSHWInfo(int hdl, EFSHWInfoStruct* info);
	/**
	 * @brief Get file information from the file system.
	 * 
	 * @param hdl handle of port.
	 * @param file_name identifier for the file.
	 * @param info the file information.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetEFSFileInfo(int hdl, char file_name, EFSFileInfoStruct* info);
	/**
	 * @brief Set file information from the file system.
	 * 
	 * @param hdl handle of port.
	 * @param file_name identifier for the file.
	 * @param file_attribute 0x01: APT Read Allowed;\n
	 * 0x02: APT Write Allowed;\n
	 * 0x04: APT Delete Allowed;\n
	 * 0x08:Firmware Read Allowed; \n
	 * 0x10:Firmware Write Allowed; \n
	 * 0x20:Firmware Delete Allowed.
	 * @param file_length Length of the file in pages.When zero, this will delete an existing file. When non-zero, this will create a new file.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetEFSFileInfo(int hdl, char file_name, char file_attribute, unsigned short file_length);
	/**
	 * @brief Get the data on a file.
	 * 
	 * @param hdl handle of port.
	 * @param file_name identifier for the file.
	 * @param file_address the file address to begin reading data.
	 * @param read_length the maximum number of bytes to read.
	 * @param data_target target data.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetEFSFileData(int hdl, char file_name, int file_address, unsigned short read_length, char* data_target);
	/**
	 * @brief Set the data on a file.
	 * 
	 * @param hdl handle of port.
	 * @param file_name identifier for the file.
	 * @param file_address the file address to begin writing data.
	 * @param data the data to write to the file.
	 * @param data_length the length of data buffer.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetEFSFileData(int hdl, char file_name, int file_address, char* data, unsigned short data_length);
	/**
	 * @brief Query the homing configuration for the slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param home_dircetion 0: Home in the clockwise direction;\n
	 * 1: Home in the counter - clockwise direction.\n
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int GetHomeInfo(int hdl, char slot, char* home_dircetion);
	/**
	 * @brief Set the homing configuration for the slot card.
	 * 
	 * @param hdl handle of port.
	 * @param slot target slot (4,5,6). 
	 * @param home_direction 0: Home in the clockwise direction;\n
	 * 1: Home in the counter - clockwise direction.
	 * @return function execution state.
	 * @retval 0 success.
	 * @retval "negative number" failed. 
	 */
	DLL_API int SetHomeInfo(int hdl, char slot, char home_direction);
#ifdef __cplusplus
}
#endif

#endif