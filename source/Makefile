ifndef model
model:=Pinecilv2
endif

ALL_MINIWARE_MODELS=TS100 TS80 TS80P TS101
ALL_PINECIL_MODELS=Pinecil
ALL_PINECIL_V2_MODELS=Pinecilv2
ALL_MHP30_MODELS=MHP30
ALL_SEQURE_MODELS=S60 S60P T55 S99
ALL_MODELS=$(ALL_MINIWARE_MODELS) $(ALL_PINECIL_MODELS) $(ALL_MHP30_MODELS) $(ALL_PINECIL_V2_MODELS) $(ALL_SEQURE_MODELS)

ifneq ($(model),$(filter $(model),$(ALL_MODELS)))
$(error Invalid model '$(model)', valid options are: $(ALL_MODELS))
endif

# Output folder
HEXFILE_DIR=Hexfile

# Temporary objects folder
OUTPUT_DIR_BASE=Objects
OUTPUT_DIR=Objects/$(model)

ALL_LANGUAGES=BE BG CS DA DE EL EN ES ET FI FR HR HU IT JA_JP LT NB NL_BE NL PL PT RO RU SK SL SR_CYRL SR_LATN SV TR UK UZ VI YUE_HK ZH_CN ZH_TW

LANGUAGE_GROUP_CJK_LANGS=EN JA_JP YUE_HK ZH_TW ZH_CN
LANGUAGE_GROUP_CJK_NAME=Chinese+Japanese

ifdef custom_multi_langs
RUN_SHELL_CMD:=$(shell rm -Rf {Core/Gen,$(OUTPUT_DIR)/Core/Gen,$(HEXFILE_DIR)/*_Custom.*})
LANGUAGE_GROUP_CUSTOM_LANGS=$(custom_multi_langs)
LANGUAGE_GROUP_CUSTOM_NAME=Custom
endif

LANGUAGE_GROUP_CYRILLIC_LANGS=EN BE BG RU SR_CYRL SR_LATN UK
LANGUAGE_GROUP_CYRILLIC_NAME=Belorussian+Bulgarian+Russian+Serbian+Ukrainian

LANGUAGE_GROUP_EUR_LANGS=EN $(filter-out $(LANGUAGE_GROUP_CJK_LANGS) $(LANGUAGE_GROUP_CYRILLIC_LANGS),$(ALL_LANGUAGES))
LANGUAGE_GROUP_EUR_NAME=European

LANGUAGE_GROUPS=CUSTOM CJK CYRILLIC EUR

# Define for host Python
ifndef HOST_PYTHON
HOST_PYTHON:=python3
endif

# Defines for host tools
ifeq ($(HOST_CC),)
HOST_CC:=gcc
endif

HOST_OUTPUT_DIR=Objects/host

# DFU packing address to use
DEVICE_DFU_ADDRESS=0x08000000
DEVICE_DFU_VID_PID=0x28E9:0x0189


# Enumerate all of the include directories (HAL source dirs are used for clang-format only)
APP_INC_DIR=./Core/Inc
MIDDLEWARES_DIR=./Middlewares
BSP_INC_DIR=./Core/BSP
THREADS_DIR=./Core/Threads
SOURCE_CORE_DIR=./Core/Src
BRIEFLZ_DIR=./Core/brieflz
DRIVERS_DIR=./Core/Drivers
PD_DRIVER_DIR=./Core/Drivers/usb-pd
# Exclude USB-PD tests
PD_DRIVER_TESTS_DIR=./Core/Drivers/usb-pd/tests


# Excludes for clang-format

ALL_INCLUDES_EXCEPT:=-path $(PD_DRIVER_DIR) -o -not -name "configuration.h"
ALL_SOURCE_EXCEPT:=-path $(PD_DRIVER_DIR)
# Find-all's used for formatting; have to exclude external modules
ALL_INCLUDES=$(shell find  ./Core -type d \( $(ALL_INCLUDES_EXCEPT) \) -prune -false -o \( -type f \( -name '*.h' -o -name '*.hpp' \) \) )
ALL_SOURCE=$(shell find    ./Core -type d \( $(ALL_SOURCE_EXCEPT)   \) -prune -false -o \( -type f \( -name '*.c' -o -name '*.cpp' \) \) )

# Device dependent settings
ifeq ($(model),$(filter $(model),$(ALL_MINIWARE_MODELS)))
$(info Building for Miniware )
DEVICE_BSP_DIR=./Core/BSP/Miniware
LDSCRIPT=./Core/BSP/Miniware/stm32f103.ld

ifeq ($(model),$(filter $(model),TS101))
# 128K, but logo must be at 99K so their broken ass DFU can flash it
flash_size=98k
bootldr_size=0x8000
DEVICE_DFU_ADDRESS=0x08008000
else
flash_size=62k
bootldr_size=0x4000
DEVICE_DFU_ADDRESS=0x08004000
endif

DEV_GLOBAL_DEFS=-D STM32F103T8Ux       \
                -D STM32F1             \
                -D STM32               \
                -D USE_HAL_DRIVER      \
                -D STM32F103xB         \
                -D USE_RTOS_SYSTICK    \
                -D GCC_ARMCM3          \
                -D ARM_MATH_CM3        \
                -D STM32F10X_MD        \
                -finline-limit=9999999

DEV_LDFLAGS=-Wl,--wrap=printf  -Wl,--no-wchar-size-warning
DEV_AFLAGS=
DEV_CFLAGS=-D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS=
CPUFLAGS=-mcpu=cortex-m3  \
         -mthumb          \
         -mfloat-abi=soft

DEVICE_DFU_VID_PID=0x1209:0xDB42
endif # ALL_MINIWARE_MODELS

ifeq ($(model),$(filter $(model),$(ALL_SEQURE_MODELS)))
$(info Building for Sequre )

DEVICE_BSP_DIR=./Core/BSP/Sequre
S_SRCS:=$(shell find $(DEVICE_BSP_DIR) -type f -name '*.S')
LDSCRIPT=./Core/BSP/Sequre/stm32f103.ld
DEV_GLOBAL_DEFS=-D STM32F103T8Ux       \
                -D STM32F1             \
                -D STM32               \
                -D USE_HAL_DRIVER      \
                -D STM32F103xB         \
                -D USE_RTOS_SYSTICK    \
                -D GCC_ARMCM3          \
                -D ARM_MATH_CM3        \
                -D STM32F10X_MD        \
                -finline-limit=9999999

DEV_LDFLAGS=-Wl,--wrap=printf  -Wl,--no-wchar-size-warning
DEV_AFLAGS=
DEV_CFLAGS=-D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS=
CPUFLAGS=-mcpu=cortex-m3  \
         -mthumb          \
         -mfloat-abi=soft

flash_size=62k
ifeq ($(model), S60P)
bootldr_size=0x5000
DEVICE_DFU_ADDRESS=0x08005000
else ifeq ($(model), S99)
bootldr_size=0x4c00
DEVICE_DFU_ADDRESS=0x08004c00
else 
# S60 or T55
bootldr_size=0x4400
DEVICE_DFU_ADDRESS=0x08004400
endif
DEVICE_DFU_VID_PID=0x1209:0xDB42
endif # ALL_SEQURE_MODELS

ifeq ($(model),$(filter $(model),$(ALL_MHP30_MODELS)))
$(info Building for MHP30 )

DEVICE_BSP_DIR=./Core/BSP/MHP30
LDSCRIPT=./Core/BSP/MHP30/stm32f103.ld
DEV_GLOBAL_DEFS=-D STM32F103T8Ux    \
                -D STM32F1          \
                -D STM32            \
                -D USE_HAL_DRIVER   \
                -D STM32F103xB      \
                -D USE_RTOS_SYSTICK \
                -D GCC_ARMCM3       \
                -D ARM_MATH_CM3     \
                -D STM32F10X_MD

DEV_LDFLAGS=
DEV_AFLAGS=
DEV_CFLAGS=-D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS=
CPUFLAGS=-mcpu=cortex-m3 \
         -mthumb         \
         -mfloat-abi=soft

flash_size=126k
bootldr_size=32k
DEVICE_DFU_ADDRESS=0x08008000
DEVICE_DFU_VID_PID=0x1209:0xDB42
endif # ALL_MHP30_MODELS

ifeq ($(model),$(ALL_PINECIL_MODELS))
$(info Building for Pine64 Pinecilv1)


DEVICE_BSP_DIR=./Core/BSP/Pinecil
S_SRCS:=$(shell find $(DEVICE_BSP_DIR) -type f -name '*.S')
LDSCRIPT=./Core/BSP/Pinecil/Vendor/SoC/gd32vf103/Board/pinecil/Source/GCC/gcc_gd32vf103_flashxip.ld
flash_size=128k
bootldr_size=0x0

# Flags
CPUFLAGS=-march=rv32imaczicsr \
         -mabi=ilp32          \
         -mcmodel=medany      \
         -fsigned-char        \
         -fno-builtin         \
         -nostartfiles

DEV_LDFLAGS=-nostartfiles
DEV_AFLAGS=
DEV_GLOBAL_DEFS=-DRTOS_FREERTOS -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP
DEV_CFLAGS=-D VECT_TAB_OFFSET=$(bootldr_size)U
DEV_CXXFLAGS=
endif # ALL_PINECIL_MODELS

ifeq ($(model),$(ALL_PINECIL_V2_MODELS))
$(info Building for Pine64 Pinecilv2 )


DEVICE_BSP_DIR=./Core/BSP/Pinecilv2
LDSCRIPT=./Core/BSP/Pinecilv2/bl_mcu_sdk/drivers/bl702_driver/bl702_flash.ld
DEVICE_DFU_ADDRESS=0x23000000
# DFU starts at the beginning of flash
# Flags
CPUFLAGS=-march=rv32imafczicsr                                \
         -mabi=ilp32f                                         \
         -mcmodel=medany                                      \
         -fsigned-char                                        \
         -fno-builtin                                         \
         -nostartfiles                                        \
         -DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler \
         -DARCH_RISCV                                         \
         -D__RISCV_FEATURE_MVE=0                              \
         -DBL702                                              \
         -DBFLB_USE_ROM_DRIVER=0                              \

# Binary blobs suck and they should be ashamed
PINECILV2_SDK_DIR=$(DEVICE_BSP_DIR)/bl_mcu_sdk
PINECILV2_COMPONENTS_DIR=$(PINECILV2_SDK_DIR)/components
PINECILV2_BLE_CRAPWARE_BLOB_DIR=$(PINECILV2_COMPONENTS_DIR)/ble/blecontroller/lib
PINECILV2_RF_CRAPWARE_BLOB_DIR=$(PINECILV2_COMPONENTS_DIR)/ble/bl702_rf/lib

DEV_LDFLAGS=-nostartfiles                         \
            -L $(PINECILV2_BLE_CRAPWARE_BLOB_DIR) \
            -L $(PINECILV2_RF_CRAPWARE_BLOB_DIR)  \
            -l blecontroller_702_m0s1s            \
            -l bl702_rf                           \
            -Wl,--wrap=printf                     \
            -Wl,--defsym=__wrap_printf=bflb_platform_printf

DEV_AFLAGS=
DEV_GLOBAL_DEFS=-DCFG_FREERTOS                                       \
                -DARCH_RISCV                                         \
                -DBL702                                              \
                -DCFG_BLE_ENABLE                                     \
                -DBFLB_BLE                                           \
                -DCFG_BLE                                            \
                -DOPTIMIZE_DATA_EVT_FLOW_FROM_CONTROLLER             \
                -DBL_MCU_SDK                                         \
                -DCFG_CON=1                                          \
                -DCFG_BLE_TX_BUFF_DATA=2                             \
                -DCONFIG_BT_PERIPHERAL                               \
                -DCONFIG_BT_L2CAP_DYNAMIC_CHANNEL                    \
                -DCONFIG_BT_GATT_CLIENT                              \
                -DCONFIG_BT_CONN                                     \
                -DCONFIG_BT_GATT_DIS_PNP                             \
                -DCONFIG_BT_GATT_DIS_SERIAL_NUMBER                   \
                -DCONFIG_BT_GATT_DIS_FW_REV                          \
                -DCONFIG_BT_GATT_DIS_HW_REV                          \
                -DCONFIG_BT_GATT_DIS_SW_REV                          \
                -DCONFIG_BT_ECC                                      \
                -DCONFIG_BT_GATT_DYNAMIC_DB                          \
                -DCONFIG_BT_GATT_SERVICE_CHANGED                     \
                -DCONFIG_BT_KEYS_OVERWRITE_OLDEST                    \
                -DCONFIG_BT_KEYS_SAVE_AGING_COUNTER_ON_PAIRING       \
                -DCONFIG_BT_GAP_PERIPHERAL_PREF_PARAMS               \
                -DCONFIG_BT_BONDABLE                                 \
                -DCONFIG_BT_HCI_VS_EVT_USER                          \
                -DCONFIG_BT_ASSERT                                   \
                -DCONFIG_BT_SIGNING                                  \
                -DCONFIG_BT_SETTINGS_CCC_LAZY_LOADING                \
                -DCONFIG_BT_SETTINGS_USE_PRINTK                      \
                -DCFG_SLEEP                                          \
                -DCONFIG_BT_OBSERVER                                 \
                -DCONFIG_BT_BROADCASTER                              \
                -DportasmHANDLE_INTERRUPT=FreeRTOS_Interrupt_Handler \
                -DCONFIG_BT_DEVICE_NAME=\"Pinecil\"                  \
                -DCONFIG_BT_DEVICE_APPEARANCE=0x06C1
                # -DCFG_BLE_STACK_DBG_PRINT                            \
                # -DCONFIG_BT_CENTRAL                                  \
                # -DCONFIG_BT_ALLROLES                                 \
# -DBFLB_USE_HAL_DRIVER \
# -DCONFIG_BT_SMP

# Required to be turned off due to their drivers tripping warnings
DEV_CFLAGS=-Wno-error=enum-conversion -Wno-type-limits -Wno-implicit-fallthrough
DEV_CXXFLAGS=$(DEV_CFLAGS)
flash_size=128k
bootldr_size=0x0

endif # ALL_PINECIL_V2_MODELS

DEVICE_BSP_INCLUDE_DIRS:= ${shell find ${DEVICE_BSP_DIR} -type d -print}
THREADS_INCLUDE_DIRS:= ${shell find ${THREADS_DIR} -type d -print}
DRIVERS_INCLUDE_DIRS:= ${shell find ${DRIVERS_DIR} -type d -print}
BRIEFLZ_INCLUDE_DIRS:= ${shell find ${BRIEFLZ_DIR} -type d -print}
MIDDLEWARES_INCLUDE_DIRS:= ${shell find ${MIDDLEWARES_DIR} -type d -print}


INCLUDES=-I$(APP_INC_DIR)                              \
         -I$(BSP_INC_DIR)                              \
          ${patsubst %,-I%,${DEVICE_BSP_INCLUDE_DIRS}} \
          ${patsubst %,-I%,${THREADS_INCLUDE_DIRS}}    \
          ${patsubst %,-I%,${DRIVERS_INCLUDE_DIRS}}    \
          ${patsubst %,-I%,${BRIEFLZ_INCLUDE_DIRS}}    \
          ${patsubst %,-I%,${MIDDLEWARES_INCLUDE_DIRS}}

ASM_INC=$(INCLUDES)


SOURCE:=$(shell find ${THREADS_DIR}            -type f -name '*.c') \
        $(shell find ${SOURCE_CORE_DIR}        -type f -name '*.c') \
        $(shell find ${DRIVERS_DIR}            -type f -name '*.c') \
        $(shell find ${DEVICE_BSP_DIR}         -type f -name '*.c') \
        $(shell find ${MIDDLEWARES_DIR}        -type f -name '*.c') \
        $(BRIEFLZ_DIR)/depack.c

# We exclude the USB-PD stack tests $(PD_DRIVER_TESTS_DIR)
SOURCE_CPP:=$(shell find ${THREADS_DIR}      -type f -name '*.cpp') \
            $(shell find ${SOURCE_CORE_DIR}  -type f -name '*.cpp') \
            $(shell find ${DRIVERS_DIR}      -type f -name '*.cpp' -not -path "${PD_DRIVER_TESTS_DIR}/*" ) \
            $(shell find ${DEVICE_BSP_DIR}   -type f -name '*.cpp') \
            $(shell find ${MIDDLEWARES_DIR}  -type f -name '*.cpp')


S_SRCS:=$(shell find $(DEVICE_BSP_DIR) -type f -name '*.S')


# Code optimisation ------------------------------------------------------------
OPTIM=-Os                             \
      -fno-jump-tables                \
      -foptimize-strlen               \
      -faggressive-loop-optimizations \
      -fdevirtualize-at-ltrans        \
      -fmerge-all-constants           \
      -fshort-wchar                   \
      -flto=auto                      \
      -finline-small-functions        \
      -finline-functions              \
      -findirect-inlining             \
      -fdiagnostics-color             \
      -ffunction-sections             \
      -fdata-sections                 \
      -fshort-enums                   \
      -fsingle-precision-constant     \
      -fno-common                     \
      -fno-math-errno                 \
      -ffast-math                     \
      -ffinite-math-only              \
      -fno-signed-zeros               \
      -fsingle-precision-constant

# Global defines ---------------------------------------------------------------
GLOBAL_DEFINES+=$(DEV_GLOBAL_DEFS) -D USE_RTOS_SYSTICK -D MODEL_$(model) -D VECT_TAB_OFFSET=$(bootldr_size)U -fshort-wchar

ifdef swd_enable
GLOBAL_DEFINES+=-DSWD_ENABLE
endif

# Libs -------------------------------------------------------------------------
LIBS=

# Compilers --------------------------------------------------------------------
COMPILER=gcc

# arm-none-eabi is the general ARM compiler
# riscv-none-embed is the riscv compiler
# riscv-nuclei-elf is the nuclei tuned one for their cores
ifeq ($(model),$(filter $(model),$(ALL_MINIWARE_MODELS) $(ALL_MHP30_MODELS)))
COMPILER_PREFIX=arm-none-eabi
endif
ifeq ($(model),$(filter $(model),$(ALL_SEQURE_MODELS) ))
COMPILER_PREFIX=arm-none-eabi
endif
ifeq ($(model),$(filter $(model),$(ALL_PINECIL_MODELS) $(ALL_PINECIL_V2_MODELS)))
COMPILER_PREFIX=riscv-none-elf
endif

# Programs ---------------------------------------------------------------------
CC=$(COMPILER_PREFIX)-gcc
CPP=$(COMPILER_PREFIX)-g++
OBJCOPY=$(COMPILER_PREFIX)-objcopy
SIZE=$(COMPILER_PREFIX)-size
OBJDUMP=$(COMPILER_PREFIX)-objdump

# Use gcc in assembler mode so we can use defines etc in assembly
AS=$(COMPILER_PREFIX)-gcc -x assembler-with-cpp

# Linker flags -----------------------------------------------------------------
LINKER_FLAGS=-Wl,--gc-sections                             \
             -Wl,--wrap=malloc                             \
             -Wl,--wrap=free                               \
             -Wl,--undefined=vTaskSwitchContext            \
             -Wl,--undefined=pxCurrentTCB                  \
             -Wl,--defsym=__FLASH_SIZE__=$(flash_size)     \
             -Wl,--defsym=__BOOTLDR_SIZE__=$(bootldr_size) \
             -Wl,--print-memory-usage                      \
                 --specs=nosys.specs                       \
                 --specs=nano.specs                        \
             $(DEV_LDFLAGS)

# Compiler flags ---------------------------------------------------------------
CHECKOPTIONS=-Wtrigraphs                  \
             -Wuninitialized              \
             -Wmissing-braces             \
             -Wfloat-equal                \
             -Wunreachable-code           \
             -Wswitch-default             \
             -Wreturn-type                \
             -Wundef                      \
             -Wparentheses                \
             -Wnonnull                    \
             -Winit-self                  \
             -Wmissing-include-dirs       \
             -Wsequence-point             \
             -Wswitch                     \
             -Wformat                     \
             -Wsign-compare               \
             -Waddress                    \
             -Waggregate-return           \
             -Wmissing-field-initializers \
             -Wshadow                     \
             -Wno-unused-parameter        \
             -Wdouble-promotion

CHECKOPTIONS_C=$(CHECKOPTIONS) -Wbad-function-cast

CXXFLAGS=$(DEV_CXXFLAGS)        \
         $(CPUFLAGS)            \
         $(INCLUDES)            \
         $(GLOBAL_DEFINES)      \
       -D${COMPILER}            \
       -MMD                     \
         $(CHECKOPTIONS)        \
       -std=c++17               \
         $(OPTIM)               \
       -fno-rtti                \
       -fno-exceptions          \
       -fno-non-call-exceptions \
       -fno-use-cxa-atexit      \
       -fno-strict-aliasing     \
       -fno-threadsafe-statics  \
       -T$(LDSCRIPT)

CFLAGS=$(DEV_CFLAGS)     \
       $(CPUFLAGS)       \
       $(INCLUDES)       \
       $(CHECKOPTIONS_C) \
       $(GLOBAL_DEFINES) \
     -D${COMPILER}       \
     -MMD                \
     -std=gnu11          \
       $(OPTIM)          \
     -T$(LDSCRIPT)       \
     -c

AFLAGS=$(CPUFLAGS)       \
       $(DEV_AFLAGS)     \
       $(GLOBAL_DEFINES) \
       $(OPTIM)          \
       $(ASM_INC)        \
       $(INCLUDES)

OBJS=$(SOURCE:.c=.o)
OBJS_CPP=$(SOURCE_CPP:.cpp=.o)
OBJS_S=$(S_SRCS:.S=.o)

OUT_OBJS=$(addprefix $(OUTPUT_DIR)/,$(OBJS))
OUT_OBJS_CPP=$(addprefix $(OUTPUT_DIR)/,$(OBJS_CPP))
OUT_OBJS_S=$(addprefix $(OUTPUT_DIR)/,$(OBJS_S))

default: firmware-EN

firmware-%: $(HEXFILE_DIR)/$(model)_%.hex $(HEXFILE_DIR)/$(model)_%.bin $(HEXFILE_DIR)/$(model)_%.dfu
	@true

# Targets for binary files

%.hex: %.elf Makefile
	$(OBJCOPY) $< -O ihex $@

%.bin: %.elf Makefile
	$(OBJCOPY) $< -O binary $@
	$(SIZE) $<

%.dfu: %.bin Makefile
	$(HOST_PYTHON) dfuse-pack.py -b $(DEVICE_DFU_ADDRESS)@0:$< -D $(DEVICE_DFU_VID_PID) $@

$(HEXFILE_DIR)/$(model)_%.elf: \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)    \
		$(OUTPUT_DIR)/Core/Gen/Translation.%.o       \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation.$*.o      \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(HEXFILE_DIR)/$(model)_string_compressed_%.elf: \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)      \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz.%.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o   \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz.$*.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o    \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(HEXFILE_DIR)/$(model)_font_compressed_%.elf: \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)           \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_font.%.o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o        \
		Makefile $(LDSCRIPT)
	@test -d $(@D) || mkdir -p $(@D)
	@echo Linking $@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_font.$*.o  \
		$(OUTPUT_DIR)/Core/LangSupport/lang_single.o          \
		$(LIBS) $(LINKER_FLAGS) -o$@ -Wl,-Map=$@.map

$(OUT_OBJS): $(OUTPUT_DIR)/%.o: %.c Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@$(CC) -c $(CFLAGS) $< -o $@

$(OUTPUT_DIR)/%.o: %.cpp Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@$(CPP) -c $(CXXFLAGS) $< -o $@

$(OUT_OBJS_S): $(OUTPUT_DIR)/%.o: %.S Makefile
	@test -d $(@D) || mkdir -p $(@D)
	@echo 'Building file: $<'
	@$(AS) -c $(AFLAGS) $< -o $@

Core/Gen/Translation.%.cpp $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle: ../Translations/translation_%.json \
		../Translations/make_translation.py           \
		../Translations/translations_definitions.json \
		../Translations/font_tables.py                \
		Makefile ../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf \
		Core/Gen/macros.txt
	@test -d Core/Gen || mkdir -p Core/Gen
	@test -d $(OUTPUT_DIR)/Core/Gen/translation.files || mkdir -p $(OUTPUT_DIR)/Core/Gen/translation.files
	@echo 'Generating translations for language $*'
	@$(HOST_PYTHON) ../Translations/make_translation.py \
		--macros $(CURDIR)/Core/Gen/macros.txt      \
		-o $(CURDIR)/Core/Gen/Translation.$*.cpp    \
		--output-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		$*

Core/Gen/macros.txt: Makefile
	@test -d $(CURDIR)/Core/Gen || mkdir -p $(CURDIR)/Core/Gen
	echo "#include <configuration.h>" | $(CC) -dM -E $(CFLAGS) -MF $(CURDIR)/Core/Gen/macros.tmp - > $(CURDIR)/Core/Gen/macros.txt

# The recipes to produce compressed translation data

$(OUTPUT_DIR)/Core/Gen/translation.files/%.o: Core/Gen/Translation.%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating $@
	@$(CPP) -c $(filter-out -flto=auto ,$(CXXFLAGS)) $< -o $@

$(OUTPUT_DIR)/Core/Gen/translation.files/multi.%.o: Core/Gen/Translation_multi.%.cpp
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating $@
	@$(CPP) -c $(filter-out -flto=auto ,$(CXXFLAGS)) $< -o $@

$(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so: Core/brieflz/brieflz.c Core/brieflz/depack.c
	@test -d $(@D) || mkdir -p $(@D)
	@echo Building host brieflz shared library $@
	@$(HOST_CC) -fPIC -shared -DBLZ_DLL -DBLZ_DLL_EXPORTS -O $^ -o $@

Core/Gen/Translation_brieflz.%.cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/%.o $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so Core/Gen/macros.txt
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating BriefLZ compressed translation for $*
	@OBJCOPY=$(OBJCOPY) $(HOST_PYTHON) ../Translations/make_translation.py \
		--macros $(CURDIR)/Core/Gen/macros.txt           \
		-o $(CURDIR)/Core/Gen/Translation_brieflz.$*.cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		--strings-obj $(OUTPUT_DIR)/Core/Gen/translation.files/$*.o        \
		$*

Core/Gen/Translation_brieflz_font.%.cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/%.pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so Core/Gen/macros.txt
	@test -d $(@D) || mkdir -p $(@D)
	@echo Generating BriefLZ compressed translation font for $*
	@$(HOST_PYTHON) ../Translations/make_translation.py        \
		--macros $(PWD)/Core/Gen/macros.txt                \
		-o $(PWD)/Core/Gen/Translation_brieflz_font.$*.cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/$*.pickle \
		--compress-font \
		$*

# The recipes to produce multi-language firmwares:

# Usage: $(eval $(call multi_lang_rule,$(1)=group_code,$(2)=group_name,$(3)=lang_codes))
define multi_lang_rule

$(HEXFILE_DIR)/$(model)_multi_$(2).elf: \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)       \
		$(OUTPUT_DIR)/Core/Gen/Translation_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o     \
		Makefile $(LDSCRIPT)
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Linking $$@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP) \
		$(OUTPUT_DIR)/Core/Gen/Translation_multi.$(1).o       \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o           \
		$(LIBS) $(LINKER_FLAGS) -o$$@ -Wl,-Map=$$@.map

$(HEXFILE_DIR)/$(model)_multi_compressed_$(2).elf: \
		$(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)               \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o             \
		Makefile $(LDSCRIPT)
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Linking $$@
	@$(CPP) $(CXXFLAGS) $(OUT_OBJS_S) $(OUT_OBJS) $(OUT_OBJS_CPP)   \
		$(OUTPUT_DIR)/Core/Gen/Translation_brieflz_multi.$(1).o \
		$(OUTPUT_DIR)/Core/LangSupport/lang_multi.o             \
		$(LIBS) $(LINKER_FLAGS) -o$$@ -Wl,-Map=$$@.map

Core/Gen/Translation_multi.$(1).cpp: $(patsubst %,../Translations/translation_%.json,$(3)) \
		../Translations/make_translation.py           \
		../Translations/translations_definitions.json \
		../Translations/font_tables.py                \
		Makefile ../Translations/wqy-bitmapsong/wenquanyi_9pt.bdf \
		Core/Gen/macros.txt
	@test -d Core/Gen || mkdir -p Core/Gen
	@test -d $(OUTPUT_DIR)/Core/Gen/translation.files || mkdir -p $(OUTPUT_DIR)/Core/Gen/translation.files
	@echo 'Generating translations for multi-language $(2)'
	@$(HOST_PYTHON) ../Translations/make_translation.py   \
		--macros $(CURDIR)/Core/Gen/macros.txt           \
		-o $(CURDIR)/Core/Gen/Translation_multi.$(1).cpp \
		--output-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle \
		$(3)

$(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle: Core/Gen/Translation_multi.$(1).cpp

Core/Gen/Translation_brieflz_multi.$(1).cpp: $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).o $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle $(HOST_OUTPUT_DIR)/brieflz/libbrieflz.so Core/Gen/macros.txt
	@test -d $$(@D) || mkdir -p $$(@D)
	@echo Generating BriefLZ compressed translation for multi-language $(2)
	@OBJCOPY=$(OBJCOPY) $(HOST_PYTHON) ../Translations/make_translation.py \
		--macros $(CURDIR)/Core/Gen/macros.txt                   \
		-o $(CURDIR)/Core/Gen/Translation_brieflz_multi.$(1).cpp \
		--input-pickled $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).pickle \
		--strings-obj $(OUTPUT_DIR)/Core/Gen/translation.files/multi.$(1).o        \
		--compress-font \
		$(3)

endef # multi_lang_rule

# Add multi-language firmware rules:
$(foreach group_code,$(LANGUAGE_GROUPS),$(eval $(call multi_lang_rule,$(group_code),$(LANGUAGE_GROUP_$(group_code)_NAME),$(LANGUAGE_GROUP_$(group_code)_LANGS))))

# Clean up targets

clean:
	rm -Rf Core/Gen
	rm -Rf $(OUTPUT_DIR_BASE)
	rm -Rf $(HEXFILE_DIR)/*
	rm -Rf ../Translations/__pycache__

clean-all: clean
	rm -Rf $(HEXFILE_DIR)

# Style formatting helper targets

# Overwrite source files in your local repo copy according to IronOS code style rules (source/.clang-format) WITHOUT A WARNING!
# Use `git diff` or your favorite diff tool via `git difftool` before commit to make sure there are no false-negative changes.
# If so, report an issue, please.
style:
	@for src in $(ALL_SOURCE) $(ALL_INCLUDES); do echo "Formatting $$src" ; clang-format -i "$$src" ; done;
	@echo "Done! Please, check the changes before commit."

# Code style checks using clang-format:
# - show output in gcc-like error compatible format for IDEs/editors;
# - external variables for debug purposes (can be used at the same time, i.e. STOP=1 LIST=1 ...):
#   * call `make check-style STOP=1` to exit after first failed file;
#   * call `make check-style LIST=1` to show failed file names only;
# - here we process only list of files;
# - per-file check happens in scripts/deploy.sh : check_style_file - since shell commands involved, the check logic moved to shell script for better maintainance outside of makefile syntax crossing.
# - $? / error / STOP conditional logic needed to:
#   * check errors in formatting from deploy.sh
#   * process STOP env variable
check-style:
	@error=0; export LIST=$$LIST; for src in $(ALL_SOURCE) $(ALL_INCLUDES) ; do \
		../scripts/deploy.sh  check_style_file  "$$src" ; \
		test "$${?}" -eq 1 && export error=1 ; \
		test "$${error}" -eq 1 && test -n "$${STOP}" && break; \
	done; \
	if [ $$error -eq 0 ] ; then  echo "" && echo "" && echo "Style check: PASS" && echo "" && echo "" && exit 0 ; \
	else  echo "" && echo "" && echo "Style check: FAIL! Please, check the log above for the details." && echo "If there is a false-negative trigger, please, report an issue attaching the log or link to the log!" && echo "" && echo "" && exit 1 ; \
	fi;

.PHONY: style  check-style  all  clean  default  clean-all
.SECONDARY:

# Pull in dependency info for *existing* .o files
-include $(OUT_OBJS:.o=.d)
-include $(OUT_OBJS_CPP:.o=.d)
-include $(OUTPUT_DIR)/Core/Gen/Translation.*.d
-include $(OUTPUT_DIR)/Core/Gen/Translation_*.d
-include $(OUTPUT_DIR)/Core/Gen/translation.files/*.d
