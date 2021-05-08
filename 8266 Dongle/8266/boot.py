# This file is executed on every boot (including wake-boot from deepsleep)
#import esp
#esp.osdebug(None)
import uos, machine
uart = machine.UART(0, 115200)
uos.dupterm(uart, 1)
#
import gc
#import webrepl
#webrepl.start()
gc.collect()
