[![CI Build](https://github.com/Ralim/IronOS/actions/workflows/push.yml/badge.svg)](https://github.com/Ralim/IronOS/actions/workflows/push.yml)
[![Total Downloads](https://img.shields.io/github/downloads/ralim/IronOS/total)](https://github.com/Ralim/IronOS)
[![Contributors](https://img.shields.io/github/contributors-anon/ralim/ironos?color=blue&style=flat)](https://github.com/Ralim/IronOS/graphs/contributors)
[![Latest Release](https://img.shields.io/github/v/release/ralim/IronOS)](https://github.com/Ralim/IronOS/releases/latest)

# IronOS - Open Source Flexible Firmware for Soldering Hardware

_This repository was formerly known as TS100, it's the same great code. Just with more supported devices._

Originally conceived as an alternative firmware for the _TS100_, this firmware has evolved into a complex soldering hardware control firmware.

The firmware implements all of the standard features of a _smart_ soldering hardware, with lots of little extras and tweaks.
I highly recommend reading the installation guide fully when installing on your device. And after install just explore the settings menu.

For soldering hardware that is designed to be powered by _smart_ power sources such as _PD_ or _QC_, the firmware supports settings around the negotiated power and voltage.
For soldering hardware that is designed to be powered by batteries (_TS100_ & _Pinecil_), settings for a cutoff voltage for battery protection are supported.

Currently **31** languages are supported. When downloading the firmware for your soldering hardware, take note of the _language code_ in the file name.

This project is considered feature complete for use on a daily basis, _so please suggest any feature improvements you would like!_

_This firmware does **NOT** support the USB port while running for changing settings (this is done through the onscreen menu only). Custom logos are edited on a computer and flashed in the same manner as firmware._

## Supported Hardware

|     Device     | DC  | QC  | PD  | EPR\*\*\*\* | BLE | Tip Sense | Recommended Purchase |                  Notes                  |
| :------------: | :-: | :-: | :-: | :-: | :-: | :-------: | :------------------: | :-------------------------------------: |
| Miniware MHP30 | ❌  | ❌  | ✔️  | ❌  | ❌  |    ✔️     |          ✔️          |                                         |
|   Pinecil V1   | ✔️  | ✔️  | ✔️  | ❌  | ❌  |    ❌     |        ❌ \*         |                                         |
|   Pinecil V2   | ✔️  | ✔️  | ✔️  | ✔️  | ✔️  |    ✔️     |          ✔️          |                                         |
| Miniware TS101 | ✔️  | ❌  | ✔️  | ✔️  | ❌  |    ✔️     |          ✔️          | Full OLED resolution not yet supported. |
|   Sequre S60   | ❌  | ❌  | ✔️  | ❌  | ❌  |    ❌     |          ✔️          | Full OLED resolution not yet supported. |
|  Sequre S60P   | ❌  | ❌  | ✔️  | ❌  | ❌  |    ❌     |          ✔️          | Full OLED resolution not yet supported. |
|   Sequre T55   | ❌  | ❌  | ✔️  | ❌  | ❌  |    N/A    |          ✔️          | Full OLED resolution not yet supported. |
| Miniware TS80P | ❌  | ✔️  | ✔️  | ❌  | ❌  |    N/A    |          ✔️          |                                         |
| Miniware TS100 | ✔️  | ❌  | ❌  | ❌  | ❌  |    ❌     |        ❌\*\*        |                                         |
| Miniware TS80  | ❌  | ✔️  | ❌  | ❌  | ❌  |    N/A    |       ❌\*\*\*       |                                         |

_Tip Sense_ refers to the device being able to choose between the _"regular"_ _TS100_ or _Hakko T12 style_ tips and _Pine64_'s custom shorter tips which have lower resistance and allow for more power. This is N/A for _TS80(P)_ as there is only one model of tip for them.

_Recommended Purchase_ is only referring to if you are buying a **new** device. Of course all the devices listed are supported and will work excellently for years to come.

The _TS101_ & _S60(P)_ irons and _MHP30_ & _T55_ plates feature a higher resolution OLED than other devices. Work is ongoing to support this fully, for now a cropped view is usable.

\* _PinecilV1_ stopped being manufactured a long time ago now, all models for sale online are generally clones (or old stock). Vendors are trying to sell these for more than _Pine64_ sells the _V2_ for now. Thus the _V1_ is **_no longer recommended_**.

\*\* Please note that _Miniware_ started shipping _TS100_'s using **cloned STM32 chips**. While these do work with _IronOS_, their **DFU bootloader** works terribly, and it is hard to get it to successfully flash larger firmware images like _IronOS_ without timing out. This is the main reason why the _TS100_ is **_no longer recommended_**.

\*\*\* _TS80_ is replaced by _TS80P_. Production ramped down a long time ago and it's just existing stock clearing the system. It's marked not recommended being optimistic that people might pause and buy the far superior _TS80P_ instead. This is the main reason why the _TS80_ is **_no longer recommended_**.

\*\*\*\* **EPR/PPS with 28V support** is _**disabled by default**_ due to [safety concerns](https://github.com/Ralim/IronOS/pull/2073), but to turn it back on set  
_PD Mode_ option in _Power settings_ submenu to _Safe_ or _Default_.

## Getting Started

To get started with _IronOS firmware_, please jump to [Getting Started Guide](https://ralim.github.io/IronOS/GettingStarted/).

## Installation

For notes on installation for your device, please refer to the flashing guide for your device:

- [MHP30](https://ralim.github.io/IronOS/Flashing/MHP30)
- [Pinecil V1](https://ralim.github.io/IronOS/Flashing/Pinecil%20V1/)
- [Pinecil V2](https://ralim.github.io/IronOS/Flashing/Pinecil%20V2/)
- [TS80 / TS80P](https://ralim.github.io/IronOS/Flashing/TS80%28P%29/)
- [TS100](https://ralim.github.io/IronOS/Flashing/TS100)

## Builds

The links in the table below allow to download available builds directly:
- current _Stable Release_ is **`v2.22`**;
- _Development Build_ **dynamically** provides _**the latest successful build**_ from **`dev`** branch.

|        Device         | Stable Release | Development Build |
|:---------------------:|:--------------:|:-----------------:|
| Pinecil  V1           | [Pinecil.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/Pinecil.zip)                           | [Pinecil.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/Pinecil.zip)                           |
| Pinecil  V1/multilang | [Pinecil_multi-lang.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/Pinecil_multi-lang.zip)     | [Pinecil_multi-lang.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/Pinecil_multi-lang.zip)     |
| Pinecil  V2           | [PinecilV2.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/PinecilV2.zip)                       | [PinecilV2.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/Pinecilv2.zip)                       |
| Pinecil  V2/multilang | [PinecilV2_multi-lang.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/PinecilV2_multi-lang.zip) | [PinecilV2_multi-lang.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/Pinecilv2_multi-lang.zip) |
| Miniware TS100        | [TS100.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/TS100.zip)                               | [TS100.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/TS100.zip)                               |
| Miniware TS101        | [TS101.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/TS101.zip)                               | [TS101.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/TS101.zip)                               |
| Miniware TS80         | [TS80.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/TS80.zip)                                 | [TS80.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/TS80.zip)                                 |
| Miniware TS80P        | [TS80P.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/TS80P.zip)                               | [TS80P.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/TS80P.zip)                               |
| Miniware MHP30        | [MHP30.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/MHP30.zip)                               | [MHP30.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/MHP30.zip)                               |
| Sequre   S60          | [S60.zip](https://github.com/Ralim/IronOS/releases/download/v2.22/S60.zip)                                   | [S60.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/S60.zip)                                   |
| Sequre   S60P         | Not Released                                                                                                 | [S60P.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/S60P.zip)                                 |
| Sequre   T55          | Not Released                                                                                                 | [T55.zip](https://nightly.link/Ralim/IronOS/workflows/push/dev/T55.zip)                                   |

## Key Features

- PID style iron temperature control;
- automatic sleep with selectable sensitivity;
- adjustable & tweakable motion wake support;
- all settings exposed in the intuitive menu;
- (_TS100_) set a voltage lower limit for Lithium batteries so you don't kill your battery pack;
- (_TS80_) set 18W or 24W settings for your power bank;
- (_TS80P_) automatically negotiates appropriate PD and falls back to QC mode like _TS80_;
- (_Pinecil_) supports all 3 power modes (PD, QC, DC In);
- (_Pinecilv2_) supports _USB-PD EPR_ for **28V** operation;
- improved readability Fonts, supporting multiple languages;
- use hardware features to improve reliability;
- boost mode lets you temporarily change the temperature when soldering (i.e. raise the temperature for short periods);
- (_TS100_/_Pinecil_) battery charge level indicator if power source set to a LiPo cell count;
- (_TS80_/_TS80P_/_Pinecil_) power bank operating voltage is displayed;
- [custom boot up logo support](https://ralim.github.io/IronOS/Logo/)[^bootlogo];
- automatic LCD rotation based on the orientation;
- ... and many many other cool & hackable features![^changelog]

[^bootlogo]:
    **BOOTUP LOGO NOTICE**:
    IronOS supports both a bootup logo _AND_ bootup animations.
    However, _**they are no longer included in this repo**_.
    **Please, [read the docs](https://ralim.github.io/IronOS/Logo/) for more information**.

[^changelog]:
    [See the full changelog here](https://ralim.github.io/IronOS/History).

## Basic Control

Supported device is controlled by two buttons which can be pressed in the following ways:
 - short: ~1 second or so;
 - long: more than 1 second;
 - both (press & hold both of them together).

Available buttons are:
 - `+/A` button: near the front closer to the tip (for irons) or on the left side of the device (for plates);
 - `-/B` button: near the back far from the tip (for irons) or on the right side of the device (for plates).

After powering on the device for the first time with _IronOS_ installed and having the tip/plate plugged in, on the main menu in _standby mode_ the unit shows a pair of prompts for the two most common operations:
- pressing the `+/A` button enters the _soldering mode_;
- pressing the `-/B` button enters the _settings menu_;
- in _soldering mode_:
  - short press of `+/A` / `-/B` buttons changes the soldering temperature;
  - long press of the `+/A` button enables _boost mode_ (increasing soldering temperature to the adjustable setting as long as the button is pressed);
  - long press of the `-/B` button enters _standby mode_ and stops heating;
- in _standby mode_:
  - long press of the `+/A` button enters _soldering temperature adjust mode_ (the same as the one in the _soldering mode_, but allows to adjust the temperature before heating up);
  - long hold of the `-/B` button enters the [_debug menu_](https://ralim.github.io/IronOS/DebugMenu/);
- in _menu mode_ (to make it short here):
  - `-/B` scrolls & cycles through menus and submenus;
  - `+/A` enters to menu & submenu settings or changes their values if they are activated already.

Additional details are described in the [menu information](https://ralim.github.io/IronOS/Menu/).

## Remote Control

### Pinecil V2 only

Pinecil V2 has [_Bluetooth Low Energy_ module](https://ralim.github.io/IronOS/Bluetooth), which is supported by _IronOS_ since `2.21` release to control some of the settings using additional tools like [PineSAM](https://github.com/builder555/PineSAM) or [PineTool](https://github.com/lachlanbell/PineTool). In `2.21` and `2.22` releases the module was _on_ by default. However, **_Bluetooth_ is turned off in the settings by default in current `dev` builds and for the next releases** [due to security concerns](#1856).[^ble]

To enable _Bluetooth_ back:
- go to _Settings_ menu;
- press `-/B` button four times to scroll the menu for `Advanced settings`;
- press `+/A` button to open submenu;
- press `+/A` button to toggle/enable _Bluetooth_ feature;
- press `-/B` **and hold it** for just more than five seconds to exit from the _Settings_ menu.

[^ble]:
    This is related only to situations when a user restores default settings using menu, or when _IronOS_ update is taking place on a new device or on a device with a previous firmware version.

## Translations

Is your preferred language missing localisation of some of the text?
Translations are stored as `json` files in the `Translations` folder.
_Pull requests_ are loved and accepted to enhance the firmware.

## Thanks

If you love this firmware and want to continue my caffeine addiction, you can do so [here](https://paypal.me/RalimTek) (or email me for other options).
I also want to give a shout out to all of the [Fantastic Contributors](https://github.com/Ralim/IronOS/graphs/contributors).

Especially to the following users, who have helped in various ways that are massively appreciated:

- [Dhiltonp](https://github.com/dhiltonp)
- [Mrkvozrout](https://github.com/Mrkvozrout)
- [JonnieZG](https://github.com/jonnieZG)
- [Federck](https://github.com/federck)
- [Jvitkauskas](https://github.com/jvitkauskas)
- [Doegox](https://github.com/doegox)
- [Perillamint](https://github.com/perillamint)
- [GeminiServer](https://github.com/GeminiServer)
- [Patrick Horlebein](https://github.com/PixelPirate)
- [Firebie](https://github.com/Firebie)
- [Agatti](https://github.com/agatti)
- [Discip](https://github.com/discip)
- [Paul Fertser](https://github.com/paulfertser)

Plus the huge number of people who have contributed translations, your effort is massively appreciated.

## License

The code created by the community is covered by the [GNU GPLv3](https://www.gnu.org/licenses/gpl-3.0.html#license-text) license **unless noted elsewhere**.
Other components such as _FreeRTOS_ and _USB-PD_ have their own licenses.

## Commercial Use

This software is provided _**"AS IS"**_, so I cannot provide any commercial support for the firmware.
However, you are more than welcome to distribute links to the firmware or provide hardware with this firmware.
**Please do not re-host the files, but rather link to this page, so that there are no old versions of the firmware scattered around**.
