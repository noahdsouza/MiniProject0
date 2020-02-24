


import usb.core

class paddle:

    def __init__(self):
        self.SPRING = 0
        self.DAMPER = 1
        self.TEXTURE = 2
        self.READ_SW1 = 3
        self.READ_SW2 = 4
        self.READ_SW3 = 5
        self.READ_ANGLE = 6
        self.SET_DUTY_VAL = 7
        self.GET_DUTY_VAL = 8
        self.GET_DUTY_MAX = 9

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def spring(self):
        try:
            self.dev.ctrl_transfer(0x40, self.SPRING)
        except usb.core.USBError:
            print("Could not send SPRING vendor request.")

    def damper(self):
        try:
            self.dev.ctrl_transfer(0x40, self.DAMPER)
        except usb.core.USBError:
            print("Could not send DAMPER vendor request.")

    def texture(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TEXTURE)
        except usb.core.USBError:
            print("Could not send TEXTURE vendor request.")

    def wall(self):
        try:
            self.dev.ctrl_transfer(0x40, self.WALL)
        except usb.core.USBError:
            print("Could not send TEXTURE vendor request.")

    def read_sw1(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_SW1, 0, 0, 1)
        except usb.core.USBError:
            print("Could not send READ_SW1 vendor request.")
        else:
            return int(ret[0])

    # def read_sw2(self):
    #     try:
    #         ret = self.dev.ctrl_transfer(0xC0, self.READ_SW2, 0, 0, 1)
    #     except usb.core.USBError:
    #         print("Could not send READ_SW2 vendor request.")
    #     else:
    #         return int(ret[0])
    #
    # def read_sw3(self):
    #     try:
    #         ret = self.dev.ctrl_transfer(0xC0, self.READ_SW3, 0, 0, 1)
    #     except usb.core.USBError:
    #         print("Could not send READ_SW3 vendor request.")
    #     else:
    #         return int(ret[0])

    def read_angle(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.READ_ANGLE, 0, 0, 2)
        except usb.core.USBError:
            print("Could not send READ_ANGLE vendor request.")
        else:
            return int(ret[0]) + 256 * int(ret[1])

    def set_duty_val(self, val):
        try:
            self.dev.ctrl_transfer(0x40, self.SET_DUTY_VAL, val)
        except usb.core.USBError:
            print("Could not send SET_DUTY_VAL vendor request.")

    def get_duty_val(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_DUTY_VAL, 0, 0, 2)
        except usb.core.USBError:
            print("Could not send GET_DUTY_VAL vendor request.")
        else:
            return int(ret[0]) + 256 * int(ret[1])

    def get_duty_max(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_DUTY_MAX, 0, 0, 2)
        except usb.core.USBError:
            print("Could not send GET_DUTY_MAX vendor request.")
        else:
            return int(ret[0]) + 256 * int(ret[1])

    def set_duty(self, duty_cycle):
        val = int(round(duty_cycle * self.get_duty_max() / 100.))
        self.set_duty_val(val)

    def get_duty(self):
        return 100. * self.get_duty_val() / self.get_duty_max()
