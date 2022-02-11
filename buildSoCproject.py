#!/usr/bin/env python3

from migen import *
from migen.genlib.io import CRG
from migen.genlib.cdc import MultiReg

import nexys4ddr as tarjeta
#import c4e6e10 as tarjeta

from litex.soc.integration.soc_core import *
from litex.soc.integration.builder import *
from litex.soc.interconnect.csr import *

from litex.soc.cores import gpio
from litex.soc.cores.bitbang import I2CMaster
from module import pwm
from module import ultrasonido
from module import motores
from module import VGA_Mapa
from module import infrarrojo

# BaseSoC ------------------------------------------------------------------------------------------

class BaseSoC(SoCCore):
	def __init__(self):
		platform = tarjeta.Platform()
		
		#add sources verilog

		platform.add_source("module/verilog/infrarrojo/infrarrojo.v" )
		platform.add_source("module/verilog/motor/ruedas.v" )
		platform.add_source("module/verilog/radar/servo_radar.v" )
		platform.add_source("module/verilog/radar/ultrasonido.v" )
		platform.add_source("module/verilog/VGA100/CamaraVGADriver.v" )
		platform.add_source("module/verilog/VGA100/clk24_25_nexys4.v" )
		platform.add_source("module/verilog/VGA100/DivisorFrecuencia.v" )
		platform.add_source("module/verilog/VGA100/Im.mem" )
		platform.add_source("module/verilog/VGA100/ImBuffer.v" )
		platform.add_source("module/verilog/VGA100/ImBufferv2.v" )
		platform.add_source("module/verilog/VGA100/Mapa.mem" )
		platform.add_source("module/verilog/VGA100/Mapa2.mem" )
		platform.add_source("module/verilog/VGA100/OV7670.v" )
		platform.add_source("module/verilog/VGA100/VGA_driver.v" )

		# SoC with CPU
		SoCCore.__init__(self, platform,
 			cpu_type="picorv32",
#			cpu_type="vexriscv",
			clk_freq=100e6,
			integrated_rom_size=0x8000,
			integrated_main_ram_size=16*1024)

		# Clock Reset Generation
		self.submodules.crg = CRG(platform.request("clk"), ~platform.request("cpu_reset"))

		# Leds
		SoCCore.add_csr(self,"leds")
		user_leds = Cat(*[platform.request("led", i) for i in range(10)])
		self.submodules.leds = gpio.GPIOOut(user_leds)
		
		# Buttons
		SoCCore.add_csr(self,"buttons")
		user_buttons = Cat(*[platform.request("btn%c" %c) for c in ['c','r','l']])
		self.submodules.buttons = gpio.GPIOIn(user_buttons)

		# servo :3
		SoCCore.add_csr(self,"servo_radar")
		self.submodules.servo_radar = pwm.servoUS(platform.request("servo"))
		
		# ultrasonido
		SoCCore.add_csr(self, "ultrasonido")
		self.submodules.ultrasonido = ultrasonido.US(platform.request("trig"), platform.request("echo"))
		
		# infrarojo
		SoCCore.add_csr(self,"infrarrojo")
		infras_in = Cat(*[platform.request("infras", i) for i in range(5)])
		self.submodules.infrarrojo = infrarrojo.Infrarrojo(infras_in)
		
		#motores
		SoCCore.add_csr(self,"ruedas")
		right = Cat(*[platform.request("right", i) for i in range(2)])
		left = Cat(*[platform.request("left", i) for i in range(2)])
		self.submodules.ruedas = motores.wheels(right, left)

		#I2C
		self.submodules.i2c0 = I2CMaster(platform.request("i2c0"))
		self.add_csr("i2c0")

		 #VGA_Driver
		SoCCore.add_csr(self,"VGA_Mapa")
		Pclock = platform.request("Aclock",0)
		Vsync = platform.request("vsync",0)
		Hsync = platform.request("hsync",0)
		RGB = Cat(*[platform.request("vga",i) for i in range(0,12)])
		XClock = platform.request("XClock",0)
		Data = Cat(*[platform.request("Data",i) for i in range(0,8)])
		Vsync_cam = platform.request("Vsync_cam",0)
		Href = platform.request("Href",0)
		Forma = Cat(*[platform.request("Forma",i) for i in range(2)])
		PromedioColor = Cat(*[platform.request("PromedioColor",i) for i in range(2)])
		self.submodules.VGA_Mapa = VGA_Mapa._VGA_Mapa(RGB, Hsync, Vsync, Pclock, Href, Vsync_cam, Data, XClock)
						
		# Build --------------------------------------------------------------------------------------------
if __name__ == "__main__":
	builder = Builder(BaseSoC(),csr_csv="Soc_MemoryMap.csv")
	builder.build()

