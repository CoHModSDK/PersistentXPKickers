import pymem
import re

pm = pymem.Pymem('RelicCOH.exe')
ww2mod = pymem.process.module_from_name(pm.process_handle, 'WW2Mod.dll')

ww2_module = pm.read_bytes(ww2mod.lpBaseOfDll, ww2mod.SizeOfImage)
address = ww2mod.lpBaseOfDll + re.search(rb'\xE8....\x84\xC0\x75\x18\x8B\x54\x24\x10', ww2_module).start() + 7

pm.write_bytes(address, b'\x90\x90', 2)
pm.close_process()

print('Patch applied successfully')
