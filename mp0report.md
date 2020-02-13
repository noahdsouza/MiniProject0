# MiniProject 0
MiniProject 0 for ENGR 3110: Elecanisms at Olin College

## Simple Coin Selector LED Toggle
**1. Board setup**

  We used `bootloadergui.py` to connect to the PIC board and loaded the given `blink.c` file onto it. To do this, we had to change some elements of the SConstruct file in the C file's directory to work with our repository (note that our local forked copies of the Elecanisms repository are at the same file folder level as our local copies of the MiniProject 0 repository) and computers. We had to change the `PrependENVPath` attribute of the `env` variable in the SConstruct file to the correct location of the MPLAB compiler with our code, which was located deep in the `opt` directory on Ubuntu.

  We then edited the `blink.c` code to work with the coin selector and toggle `LED1` on and off with the insertion of a penny.

**2. Coin Selector setup**

  For us to get our coin selector to work we need to configure the coin selector. At the start if you try putting in a penny (or any coin) it will reject it. To change this we put it into a training mode where it will accept 10 pennies to learn what they feel like. Power cycle the coin selector and put in 10 pennies one at a time. Once this is done, it will be able to take in pennies and output a pulse through the `COIN` wire. You will need a pullup resistor to help the PIC board recognize the pulse that the coin selector outputs (using the 3.3 V `VDD` pin on the PIC). We used a 10 k$\Omega$ resistor for this. We connected the coin selector's power to `VIN`, its ground to `GND`, and its "coin" output to `D0`.

**3. Code**

  We modified the `blink.c` code in the `blinkint` example to complete the assignment, programmatically saying that if the board reads a pulse in `D0` from the coin selector, then it will switch the state of an LED. The code for this can be found in the `blinkint` folder.
