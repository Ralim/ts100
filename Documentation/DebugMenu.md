# Debugging Menu

In this firmware there is extra debugging information in a hidden sub-menu.
This menu is meant to be simple, so it has no fancy GUI animations.

- Access it by pressing the rear button (`-/B`) on the iron while it is on the home screen.
- Use the front button (`+/A`) to scroll through the menu.
- To exit, use the rear button (`-/B`) again.

## Menu items

Items are shown in the menu on a single line, so they use short codes.

### Version

There is a static line on top which is presented on every sub-screen and reflects exact version of firmware. Version line on top has the following format - `vX.YYN.[ZZZZZZZZ]`:

- X: major version
- Y: minor version
- N: build type:
  - R - git-related **r**elease tag vXX.YY
  - T - git-related release **t**ag but version is not vXX.YY !
  - D - git-related **d**ev branch
  - B - git-related custom **b**ranch
  - E - git-related from d**e**tached commit
  - G - neither above but **g**it-related
  - C - build from github **C**I during _pull request_
  - H - build outside of a git tree (i.e. release tarball or **h**omebrew customization without git)
  - S - something **s**pecial[^ERR]
  - V - something **v**ery special[^ERR]
[^ERR]: `S` and `V` are reserved letters for cases when source of firmware is having very unique origin & configuration
- Z: short commit ID hash with 8 digits generated automatically from git (for git-related build types only)

I.e.:
- `v2.22H` means firmware built locally from tarball with release version of `2.22`
- `v2.22D.1A2B3C4D` means firmware with development version of `2.22` from git `dev` branch & with commit ID `1A2B3C4D` (so it can be traced for debug purposes)
- `v2.22R.5E6F7G8H` means firmware with official release version of `2.22` and it's properly tagged with `v2.22` git tag & with commit ID `5E6F7G8H`'

---

**Additional scroll-able items appear in this order**:

### Timestamp

- This is a timestamp of firmware compilation and it has the following format: `YYYYMMDD HHMMSS` (i.e., `20230701 213456` means it has been built in July, 1st, 2023 at 9:34:56 pm)

### ID

- This is used by Irons that have an ID and serial number to help check if the iron is authentic. All Pinecil V1 show the same ID number as this is the number programmed into the MCU.
- The new Pinecil V2 released Aug. 2, 2022 now uses MCU BL706, which enables generating a unique ID/Serial number to every iron. This can be used to verify your [Pinecil authenticity here](https://pinecil.pine64.org/).

### ACC

This indicates the accelerometer that is fitted inside the unit.

- MMA8652
- LIS2DH12
- BMA223
- MSA301
- SC7A20
- None -> running in fallback without movement detection
- Scanning -> Still searching I2C for one

### PWR

This indicates the current power source for the iron.
This may change during power up as the sources are negotiated in turn.

- **DC** input (dumb)
- **QC** input (We used QC2/3 negotiation for current supply)
- **PD W. VBus** input (PD subsystem is used to negotiate for current supply); and VBus is connected to your input power source
- **PD No VBus** input (PD subsystem is used to negotiate for current supply); and VBus is **NOT** connected to your input power source. If it is Not required or possible to do a special mod of your PCB (i.e. late model V1, some early Green PCB models) then [PD No VBus] displays on-screen ([see details and PD Debug section below](https://ralim.github.io/IronOS/DebugMenu/#pd-debug-menu)).

### Vin

The input voltage as read by the internal ADC. Can be used to sanity check it is being read correctly.

### Tip C

This is the tip temperature in °C.
This can be used with RTip for assessing temperature processing performance.

### Han C

This is the handle temperature or more accurately the reading of the Cold Junction Compensation (CJC) temperature sensor. This is expressed in °C. Range of 20-40 °C is normal depending on how hot/cold the room is and how long power has been plugged in which warms the PCB further.
This is used for CJC of the tip temperature.
 > If CHan is extremely high, this indicates the temperature sensor isn't reading correctly ([see Troubleshooting](https://ralim.github.io/IronOS/Troubleshooting/))

### Max C

This indicates the max temperature in °C that the system estimates it can measure the tip reliably to.
This is dependent on a few factors including the handle temperature so it can move around during use. As you use the iron, the Max increases to a point.

### UpTime

This shows how many deciseconds the unit has been powered for (600 ds = 1 minute).

### Move

This is the last timestamp of movement. When the iron is moved, this should update to match the Time field (previous menu item).
This can be used for checking performance of the movement detection code.

### Tip Res

This indicates the tip resistance that the device is currently using. For devices with multiple possible values to choose from (Pinecil V2), the appropriate value is automatically detected at every boot-up. Tip should be installed before boot-up or reading can not be done.

### Tip R

This is the raw tip reading in μV. Tip must be installed or reading will be high/inaccurate. At cool, the range of 700-1000 is normal for larger tips and ~1500 for smaller tips (TS80). This is used to evaluate the calibration routines.

### Tip O

This is the offset resulting from the *'Cold Junction Compensation Calibration'*.

### HW G

This indicates the high water mark for the stack for the GUI thread. The smaller this number is, the less headroom we have in the stack.
As this is a high-water mater, you should only trust this once you have walked through all GUI options to "hit" the worst one.

### HW M

This indicates the high-water mark for the stack for the movement detection thread. The smaller this number is, the less headroom we have in the stack.

### HW P

This indicates the high-water mark for the stack for the PID thread. The smaller this number is, the less headroom we have in the stack.

### Hall

This appears if your device is capable of having a hall effect sensor installed (Pinecil).
This shows the current magnetic field strength reading from the sensor. It is used to check if the sensor is operational, and for diagnostics and optimal placement of magnets on a stand (higher number is better/stronger). [See Hall Sensor for details](https://ralim.github.io/IronOS/HallSensor/). 

# PD Debug menu

On the Pinecil; if the iron is booted up while long holding the front button (`+`); it will show an extra hidden menu for inspecting USB-PD power adapters. We can also connect to any PD USB power to check Vbus status, even some cell phones with a USB-C port will work if it is PD. It will not show PD messages when Pinecil is powered by DC port, QC, or USB 5V (non-PD). For example, if you connect to a QC charger, you may simply see "PD State 6" which indicates "waiting for source" as no PD messages will be ever be sent and you will not be able to use (`+`) to scroll through PD negotiated messages.

Pressing (`+`) cycles through elements, and (`-`) or unplugging will exit the menu.

The first page shows the PD negotiation stage number; which can be used for diagnosing if PD is not working. Once negotiation is complete; use (`+`) button to advance to other screens which show the different proposals advertised for voltage and current (State 12 means all is good with the PD charger).

#### Below is a method for user modification to convert some early models of Pinecil V1 to safely support 24V on the DC5525 barrel.

⚠️ Warning: do this at your own risk, read everything in this document, and go to the [Pine64 community chat](https://wiki.pine64.org/wiki/Pinecil#Community_links) if you desire advice. An incorrect cut of the trace could render the Pinecil non-working.

Background: a simple user modification to the PCB on _some models_ of original V1 allows it to safely use DC barrel 24V by cutting a trace line to the Vbus which held it back to 21V. You can check whether your Pinecil V1 needs the update or can benefit from it by using a hidden trick in the PD debug menu.

- Follow instructions above to enter the PD Debug menu.
- After a few seconds or after PD negotiates (state above 5) it will show `[PD No VBus]` if it is not needed (i.e., late model V1). Alternately, if it shows `[VBus]`, then the mod has not been done and there is still a connection to the Vbus (the Vbus connection limits you to 21V until you do the mod).
- If you need to do the mod, then follow the instructions/links below which have photos. Careful to only cut the trace and nothing else.
- Then use the PD debug menu again to check for `[PD No Vbus]` before attaching any 24V PSU to the DC barrel. If you do not get the message, then try cutting the trace a little deeper or using alcohol to clear the gap of copper dust. Then check PD messages again. If you need advice/tips, join the Pine64 chat room. 

The mod method is shown in the [February 2022 PINE64 community updates](https://www.pine64.org/2022/02/15/february-update-chat-with-the-machine/). Early Pinecil V1 models required cutting a trace to achieve 24V safety with DC barrel PSU. Late model V1 made sometime in 2022 came with `[No Vbus]` already displayed, and no mod is required.

| Pinecil V2 model released Aug. 2, 2022 is an overhaul of the PCB with all relevant components capable of 28V. V2 requires no mods to support the use of 24V DC Barrel jack charger. |
:--------


