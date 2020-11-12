# stm32_ps2

stm32 ps2 keyboard and ps2 mouse driver

Features:
- 1 keyboard + 1 mouse support (if you only need one, you know)
- keyboard asc2 codes and scan codes can also be queried
- currently 3 language tables can be selected in ps2_codepage.h (US, D, HU)
- automatic operation of lock buttons
- mouse wheel query (Z axis)
- the physical connection runs completely interrupt (1 or 2 EXTI + 1 timer)
- freely adjustable pins
- freely adjustable timer
- adjustable buffer size
- adjustable interrupt priority
- 3 mouse modes
- callback function option to indicate received data and error indication
  
Example app:
- appPs2test:
    This program initializes the mouse and then sends keyboard codes and mouse events via printf. 
