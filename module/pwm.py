from migen import *
from migen.genlib.cdc import MultiReg
from litex.soc.interconnect.csr import *
from litex.soc.interconnect.csr_eventmanager import *

class servoUS(Module, AutoCSR):
    def __init__(self, servo):
            self.clk = ClockSignal()
            self.ctr = CSRStorage(2)
            self.servo = servo

            self.specials += Instance("servo_radar",
                i_clk = self.clk,
                i_ctr = self.ctr.storage,
                o_servo = self.servo,
)
