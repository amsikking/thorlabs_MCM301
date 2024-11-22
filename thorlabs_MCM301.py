# Imports from the python standard library:
import ctypes as C
import os

class Controller:
    '''
    Basic device adaptor for thorlabs MCM301 three-channel stepper motor
    controller for Cerna components. Many more commands are available and
    have not been implemented. Test code runs and seems robust.
    '''
    def __init__(self,
                 sn, # MCM301 serial number string e.g. 'TP03522143-695014'
                 stages=3*(None,), # 3-tuple e.g. ('MPM-283298', None, None)
                 min_mm=3*(None,), # 3-tuple software limit e.g. (0, 5, None)
                 max_mm=3*(None,), # 3-tuple software limit e.g. (10, 20, None)
                 velocity=3*(100,), # 3-tuple velocity % e.g. (10, 50, 100)
                 home_to_min=3*(True,), # 3-tuple e.g. (False, True, True)
                 name='MCM301',
                 verbose=True,
                 very_verbose=False):
        self.name = name
        self.verbose = verbose
        self.very_verbose = very_verbose
        # Find MCM301 controller:
        if self.verbose: print("%s: opening..."%self.name)
        devices = self._list_devices()
        assert sn in devices, (
            "%s: device (sn=%s) not found"%(self.name, sn))
        self.hdl = self._open(sn, nBaud=115200, timeout=1)
        assert self._is_open(sn)
        if self.verbose: print("%s: -> open and ready."%self.name)
        # Find attached stages and assign channels:
        attached_stages, channels = [], []
        self.ch_to_slot = {0:4, 1:5, 2:6} # map channels to available 'slots'
        for ch in range(3):
            stage = self._get_device_type(self.ch_to_slot[ch])
            attached_stages.append(stage)
            if stage is not None:
                channels.append(ch)
        self.attached_stages = tuple(attached_stages)
        self.channels = tuple(channels)
        if self.verbose:
            print("%s: attached stages = %s"%(self.name, self.attached_stages))
            print("%s: available channels = %s"%(self.name, self.channels))
        assert stages == self.attached_stages, (
            "%s: initialized stages (%s) do not match attached stages (%s)"%(
                self.name, stages, self.attached_stages))
        # Check limits and set home direction:
        self.min_mm       = len(self.channels)*[None]
        self.max_mm       = len(self.channels)*[None]
        self._home_to_min = len(self.channels)*[None]
        for ch in self.channels:
            assert min_mm[ch] is not None, (
                "%s(ch%s): must specify 'min_mm' for this channel"%(
                    self.name, ch))
            assert max_mm[ch] is not None, (
                "%s(ch%s): must specify 'max_mm' for this channel"%(
                    self.name, ch))
            self.min_mm[ch], self.max_mm[ch] = min_mm[ch], max_mm[ch]
            if not home_to_min[ch]: # range goes negative
                self.min_mm[ch], self.max_mm[ch] = -max_mm[ch], -min_mm[ch]
            self._set_home_to_min(ch, home_to_min[ch])
        # Get stage parameters:
        self._counts_per_step   = len(self.channels)*[None]
        self._nm_per_count      = len(self.channels)*[None]
        self._min_count         = len(self.channels)*[None]
        self._max_count         = len(self.channels)*[None]
        self._max_speed         = len(self.channels)*[None]
        self._max_acceleration  = len(self.channels)*[None]
        for ch in self.channels:
            self._get_stage_parameters(ch)
        # Get status and enable:
        self._enabled       = len(self.channels)*[None]
        self._homed         = len(self.channels)*[None]
        self._moving        = len(self.channels)*[None]
        self._encoder_count = len(self.channels)*[None]
        self.position_mm    = len(self.channels)*[None]
        for ch in self.channels:
            self._get_status(ch)
            if not self._enabled[ch]:
                self._set_enable(ch, True)
        # Home if needed, set velocity and get position:
        for ch in self.channels:
            if not self._homed[ch]:
                self._home(ch, block=False) # send home commands back to back
        for ch in self.channels:
            if not self._homed[ch]:
                self._finish_moving(ch)
            self.set_velocity(ch, velocity[ch])
            self.get_position_mm(ch)

    def _list_devices(self):
        if self.very_verbose:
            print("%s: listing devices"%self.name)
        buffer = (10240 * C.c_char)()
        dll.list_devices(buffer, len(buffer))
        devices = buffer.value.decode('ascii').split(',')
        if self.very_verbose:
            print("%s: devices = %s"%(self.name, devices))
        return devices

    def _open(self, sn, nBaud, timeout):
        if self.very_verbose:
            print("%s: opening device (sn=%s, nBaud=%i, timeout=%i)"%(
                self.name, sn, nBaud, timeout))
        hdl = dll.open(sn.encode('ascii'), nBaud, timeout)
        if hdl < 0:
            raise Exception("%s: device (sn=%s) not found"%(self.name, sn))
        if self.very_verbose:
            print("%s: -> device open (hdl=%s)"%(self.name, hdl))
        return hdl

    def _is_open(self, sn):
        if self.very_verbose:
            print("%s: checking device is open (sn=%s)"%(self.name, sn))
        assert dll.is_open(sn.encode('ascii')) == 1, (
            "%s: device (sn=%s) is not open"%(self.name, sn))
        if self.very_verbose:
            print("%s: -> device is open"%self.name)
        return True

    def _get_device_type(self, slot):
        if self.very_verbose:
            print("%s: getting device type (slot=%s)"%(self.name, slot))
        buffer = (16 * C.c_char)()
        dll.get_device_type(self.hdl, slot, buffer, len(buffer))
        device_type = buffer.value.decode('ascii')
        if len(device_type) == 0: device_type = None
        if self.very_verbose:
            print("%s: = %s"%(self.name, device_type))
        return device_type

    def _get_stage_parameters(self, ch):
        if self.very_verbose:
            print("%s(ch%s): getting stage parameters"%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        parameters = StageParamStruct()
        dll.get_stage_parameters(self.hdl, self.ch_to_slot[ch], parameters)
        self._counts_per_step[ch]  = parameters.counts_per_step
        self._nm_per_count[ch]     = parameters.nm_per_count
        self._min_count[ch]        = parameters.min_count
        self._max_count[ch]        = parameters.max_count
        self._max_speed[ch]        = parameters.max_speed
        self._max_acceleration[ch] = parameters.max_acceleration
        if self.very_verbose:
            print("%s(ch%s): counts_per_step  = %s "%(
                self.name, ch, self._counts_per_step[ch]))
            print("%s(ch%s): nm_per_count     = %s "%(
                self.name, ch, self._nm_per_count[ch]))
            print("%s(ch%s): min_count        = %s "%(
                self.name, ch, self._min_count[ch]))
            print("%s(ch%s): max_count        = %s "%(
                self.name, ch, self._max_count[ch]))
            print("%s(ch%s): max_speed        = %s "%(
                self.name, ch, self._max_speed[ch]))
            print("%s(ch%s): max_acceleration = %s "%(
                self.name, ch, self._max_acceleration[ch]))
        return parameters

    def _get_home_to_min(self, ch):
        if self.very_verbose:
            print("%s(ch%s): getting home to min"%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        home_to_min = (1 * C.c_char)()
        dll.get_home_to_min(self.hdl, self.ch_to_slot[ch], home_to_min)
        self._home_to_min[ch] = bool(home_to_min.value)
        if self.very_verbose:
            print("%s(ch%s): = %s"%(self.name, ch, self._home_to_min[ch]))
        return self._home_to_min[ch]

    def _set_home_to_min(self, ch, home_to_min):
        if self.very_verbose:
            print("%s(ch%s): setting home to min = %s"%(
                self.name, ch, home_to_min))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        assert isinstance(home_to_min, bool)
        dll.set_home_to_min(self.hdl, self.ch_to_slot[ch], home_to_min)
        assert self._get_home_to_min(ch) == home_to_min
        if self.very_verbose:
            print("%s(ch%s): -> done setting home to min"%(self.name, ch))
        return None

    def _get_status(self, ch):
        """
        0x00000001:'On positive direction hardware limit switch'
        0x00000002:'On negative direction hardware limit switch'
        0x00000004:'On positive direction software limit switch'
        0x00000008:'On negative direction software limit switch'
        0x00000010:'Moving in positive direction'
        0x00000020:'Moving in negative direction'
        0x00000040:'Jogging in positive direction'
        0x00000080:'Jogging in negative direction'
        0x00000100:'Motor connected'
        0x00000200:'Homing'
        0x00000400:'Homed'
        0x80000000:'Channel enabled'
        """
        if self.very_verbose:
            print("%s(ch%s): getting status"%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        encoder_count, status_bit = C.c_int(), C.c_uint()
        dll.get_status(
            self.hdl, self.ch_to_slot[ch], encoder_count, status_bit)
        self._encoder_count[ch] = encoder_count.value
        status_bit = status_bit.value
        # check if enabled, homed or moving:
        self._enabled[ch] = False
        if status_bit & 0x80000000 == 0x80000000:
            self._enabled[ch] = True
        self._homed[ch] = False
        if status_bit & 0x00000400 == 0x00000400:
            self._homed[ch] = True
        moving_masks = (
            0x00000010, 0x00000020, 0x00000040, 0x00000080, 0x00000200)
        self._moving[ch] = False
        for mask in moving_masks:
            if status_bit & mask == mask:
                self._moving[ch] = True
        if self.very_verbose:
            print("%s(ch%s): status_bit = %s (encoder_count=%i)"%(
                self.name, ch, hex(status_bit), self._encoder_count[ch]))
            print("%s(ch%s): enabled = %s"%(self.name, ch, self._enabled[ch]))
            print("%s(ch%s): homed   = %s"%(self.name, ch, self._homed[ch]))
            print("%s(ch%s): moving  = %s"%(self.name, ch, self._moving[ch]))
        return status_bit

    def _get_enable(self, ch):
        if self.very_verbose:
            print("%s(ch%s): getting enable"%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        enable = (1 * C.c_char)()
        dll.get_enable(self.hdl, self.ch_to_slot[ch], enable)
        self._enabled[ch] = bool(enable.value)
        if self.very_verbose:
            print("%s(ch%s): = %s"%(self.name, ch, self._enabled[ch]))
        return self._enabled[ch]

    def _set_enable(self, ch, enable):
        if self.very_verbose:
            print("%s(ch%s): setting enable = %s"%(self.name, ch, enable))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        assert isinstance(enable, bool)
        dll.set_enable(self.hdl, self.ch_to_slot[ch], enable)
        assert self._get_enable(ch) == enable
        if self.very_verbose:
            print("%s(ch%s): -> done setting enable"%(self.name, ch))
        return None

    def _finish_moving(self, ch):
        while self._moving[ch]:
            self._get_status(ch)
        if self.verbose:
            print('%s(ch%s): -> finished moving'%(self.name, ch))
        return None

    def _home(self, ch, block=True):
        if self.verbose:
            print("%s(ch%s): homing..."%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        dll.home(self.hdl, self.ch_to_slot[ch])
        self._moving[ch] = True
        if block:
            self._finish_moving(ch)
        return None

    def _stop(self, ch):
        if self.very_verbose:
            print("%s(ch%s): stopping"%(self.name, ch))
        dll.stop(self.hdl, self.ch_to_slot[ch])
        if self.very_verbose:
            print("%s(ch%s): -> done stopping"%(self.name, ch))
        return None

    def set_velocity(self, ch, velocity_pct):
##        ***currently not working and causing move to limit switch!***
        print("%s(ch%s): ***ERROR*** setting velocity"%(self.name, ch))
        return None
        if self.verbose:
            print("%s(ch%s): setting velocity = %s%%"%(
                self.name, ch, velocity_pct))
        assert 0 <= velocity_pct <= 100
        dll.set_velocity(self.hdl, self.ch_to_slot[ch], 0, velocity_pct)
        if self.verbose:
            print("%s(ch%s): -> done setting velocity"%(self.name, ch))
        return None

    def get_position_mm(self, ch):
        if self.verbose:
            print("%s(ch%s): getting position"%(self.name, ch))
        assert ch in self.channels, (
            "%s: channel (%s) not available"%(self.name, ch))
        nm = C.c_double()
        dll.get_position(
            self.hdl, self.ch_to_slot[ch], self._encoder_count[ch], nm)
        self.position_mm[ch] = round(1e-6 * nm.value, 3)
        if self.verbose:
            print("%s(ch%s): = %7.3f"%(self.name, ch, self.position_mm[ch]))
        return self.position_mm[ch]

    def move_mm(self, ch, position_mm, relative=True, block=True):
        if self.verbose:
            print("%s(ch%s): moving to %10.06fmm (relative=%s)"%(
                self.name, ch, position_mm, relative))
        if relative: position_mm = self.position_mm[ch] + position_mm
        if not self.min_mm[ch] <= position_mm <= self.max_mm[ch]:
            if self.verbose:
                print('%s: ***WARNING*** -> move out of limits'%self.name)
            return None
        encoder_count = C.c_int()
        dll.get_encoder_count(
            self.hdl, self.ch_to_slot[ch], 1e6 * position_mm, encoder_count)
        dll.move(self.hdl, self.ch_to_slot[ch], encoder_count.value)
        self._moving[ch] = True
        self.position_mm[ch] = position_mm
        if block:
            self._finish_moving(ch)
        return None

    def close(self):
        if self.verbose: print("%s: closing..."%self.name, end='')
        dll.close(self.hdl)
        if self.verbose: print("done.")
        return None

### Tidy and store DLL calls away from main program:

os.add_dll_directory(os.getcwd())
dll = C.cdll.LoadLibrary("MCM301Lib_x64.dll") # needs .dll in directory

def check_error(error_code):
    if error_code != 0:
        raise UserWarning("Thorlabs MCM301 error: %i"%(error_code))
    return error_code

dll.list_devices = dll.List
dll.list_devices.argtypes = [
    C.POINTER(C.c_char),        # buffer
    C.c_int]                    # buffer_length
dll.list_devices.restype = C.c_int

dll.open = dll.Open
dll.open.argtypes = [
    C.POINTER(C.c_char),        # sn
    C.c_int,                    # nBaud
    C.c_int]                    # timeout
dll.open.restype = C.c_int

dll.is_open = dll.IsOpen
dll.is_open.argtypes = [
    C.POINTER(C.c_char)]        # sn
dll.is_open.restype = C.c_int

dll.get_device_type = dll.GetSlotDeviceType
dll.get_device_type.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.POINTER(C.c_char),        # device_type
    C.c_int]                    # device_type_length
dll.get_device_type.restype = check_error

class StageParamStruct(C.Structure):
    _fields_ = [("counts_per_step", C.c_uint),
                ("nm_per_count",    C.c_float),
                ("min_count",       C.c_uint),
                ("max_count",       C.c_uint),
                ("max_speed",       C.c_double),
                ("max_acceleration",C.c_double)]

dll.get_stage_parameters = dll.GetStageParams
dll.get_stage_parameters.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.POINTER(StageParamStruct)]# stage_params_info
dll.get_stage_parameters.restype = check_error

dll.get_home_to_min = dll.GetHomeInfo
dll.get_home_to_min.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.POINTER(C.c_char)]        # home_direction
dll.get_home_to_min.restype = check_error

dll.set_home_to_min = dll.SetHomeInfo
dll.set_home_to_min.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_char]                   # home_direction
dll.set_home_to_min.restype = check_error

dll.get_status = dll.GetMotStatus
dll.get_status.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.POINTER(C.c_int),         # current_encoder
    C.POINTER(C.c_uint)]        # status_bit
dll.get_status.restype = check_error

dll.get_enable = dll.GetChanEnableState
dll.get_enable.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.POINTER(C.c_char)]        # enable_state
dll.get_enable.restype = check_error

dll.set_enable = dll.SetChanEnableState
dll.set_enable.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_char]                   # enable_state
dll.set_enable.restype = check_error

dll.home = dll.Home
dll.home.argtypes = [
    C.c_int,                    # hdl
    C.c_char]                   # slot
dll.home.restype = check_error

dll.stop = dll.MoveStop
dll.stop.argtypes = [
    C.c_int,                    # hdl
    C.c_char]                   # slot
dll.stop.restype = check_error

dll.set_velocity = dll.SetVelocity
dll.set_velocity.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_char,                   # direction
    C.c_char]                   # velocity
dll.set_velocity.restype = check_error

dll.get_position = dll.ConvertEncoderTonm
dll.get_position.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_int,                    # encoder_count
    C.POINTER(C.c_double)]      # nm
dll.get_position.restype = check_error

dll.get_encoder_count = dll.ConvertnmToEncoder
dll.get_encoder_count.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_double,                 # nm
    C.POINTER(C.c_int)]         # encoder_count
dll.get_encoder_count.restype = check_error

dll.move = dll.MoveAbsolute
dll.move.argtypes = [
    C.c_int,                    # hdl
    C.c_char,                   # slot
    C.c_int]                    # encoder_count
dll.move.restype = check_error

dll.close = dll.Close
dll.close.argtypes = [
    C.c_int]                    # hdl
dll.close.restype = check_error

if __name__ == '__main__':
    controller = Controller(sn='TP03522143-695014',
                            stages=('MPM-283298', 'MPM-283299', None),
                            min_mm=( 0,  0, None),
                            max_mm=(10, 10, None),
                            velocity=3*(100,),
                            home_to_min=(False, False, True),
                            verbose=True,
                            very_verbose=False)

    print('\nAbsolute and relative moves:')
    for ch in controller.channels:
        controller.move_mm(ch, -1, relative=False) # home_to_max = -ve range
        controller.move_mm(ch, -1)

    for ch in controller.channels:
        print('\nNon-blocking call:')
        controller.move_mm(ch, -1, block=False)
        print(' do something else...')
        controller.move_mm(ch, -1)

    print('\nMove and stop:')
    for ch in controller.channels:
        controller.move_mm(
            ch, controller.min_mm[ch], relative=False, block=False)
    for ch in controller.channels:
        controller._stop(ch)
        controller._finish_moving(ch)
        controller.get_position_mm(ch)

    print('\nRe-zero:')
    for ch in controller.channels:
        controller.move_mm(ch, 0, relative=False)

    controller.close()
