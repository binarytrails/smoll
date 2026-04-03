import math
import time
from evdev import UInput, ecodes as e

cap = {
    e.EV_REL: [e.REL_X, e.REL_Y],
    e.EV_KEY: [e.BTN_LEFT, e.BTN_RIGHT]
}

with UInput(cap, name='virtual-mouse') as ui:
    print("Virtual mouse created. Moving in 2 seconds...")
    print('---------------------------------------------------')
    print('We shall keep your Remote Desktop sessions! uWu ><"')
    print('---------------------------------------------------')
    print('Tested with https://github.com/SupremeTechnopriest/react-idle-timer')
    time.sleep(2)
    
    radius = 2
    steps = 100
    
    while True:
        for i in range(steps):
            angle = (i / steps) * (2 * math.pi)
            
            dx = int(radius * math.cos(angle))
            dy = int(radius * math.sin(angle))
            
            ui.write(e.EV_REL, e.REL_X, dx)
            ui.write(e.EV_REL, e.REL_Y, dy)
            ui.syn()
            time.sleep(0.01)

print("Done.")
