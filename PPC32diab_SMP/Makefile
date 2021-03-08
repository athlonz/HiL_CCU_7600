# Wind River Workbench generated Makefile.
# Do not edit!!!
#
# The file ".wrmakefile" is the template used by the Wind River Workbench to
# generate the makefiles of this project. Add user-specific build targets and
# make rules only(!) in this project's ".wrmakefile" file. These will then be
# automatically dumped into the makefiles.

WIND_HOME := $(subst \,/,$(WIND_HOME))
WIND_BASE := $(subst \,/,$(WIND_BASE))
WIND_USR := $(subst \,/,$(WIND_USR))
WRVX_COMPBASE := $(subst \,/,$(WRVX_COMPBASE))

all : pre_build main_all post_build

_clean ::
	@echo "make: removing targets and objects of `pwd`"

TRACE=0
TRACEON=$(TRACE:0=@)
TRACE_FLAG=$(TRACEON:1=)

MAKEFILE := Makefile

FLEXIBLE_BUILD := 1

BUILD_SPEC = PPC32diab_SMP
DEBUG_MODE = 1
ifeq ($(DEBUG_MODE),1)
MODE_DIR := Debug
else
MODE_DIR := NonDebug
endif
OBJ_DIR := .
WS_ROOT_DIR := C:/WindRiver/workspace2
PRJ_ROOT_DIR := E:/0_RT_Lab_WorkBench/20201216MainForceControl/CPS11_v4_PLC



#Global Build Macros
PROJECT_TYPE = DKM
DEFINES = 
EXPAND_DBG = 0


#BuildSpec specific Build Macros
VX_CPU_FAMILY = ppc
CPU = PPC32
TOOL_FAMILY = diab
TOOL = diab
TOOL_PATH = 
CC_ARCH_SPEC = -tPPCFH:vxworks69 -Xstmw-slow -DPPC32_fp60x -Xstsw-slow
VSB_DIR = $(WIND_BASE)/target/lib_smp
VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
LIBPATH = 
LIBS = 

IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 

IDE_LIBRARIES = 

IDE_DEFINES = -DCPU=_VX_$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_WRS_VX_SMP -D_WRS_CONFIG_SMP -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   -DIP_PORT_VXWORKS=69 



#BuildTool flags
ifeq ($(DEBUG_MODE),1)
DEBUGFLAGS_C-Compiler = -g
DEBUGFLAGS_C++-Compiler = -g
DEBUGFLAGS_Linker = -g
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler = -g
else
DEBUGFLAGS_C-Compiler =  -XO -Xsize-opt 
DEBUGFLAGS_C++-Compiler =  -XO -Xsize-opt 
DEBUGFLAGS_Linker =  -XO -Xsize-opt 
DEBUGFLAGS_Partial-Image-Linker = 
DEBUGFLAGS_Librarian = 
DEBUGFLAGS_Assembler =  -XO -Xsize-opt 
endif


#Project Targets
PROJECT_TARGETS = cps11_v4_PLC/$(MODE_DIR)/cps11_v4_PLC.out


#Rules

# cps11_v4_PLC
ifeq ($(DEBUG_MODE),1)
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler = -g
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler = -g
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Linker = -g
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Assembler = -g
else
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_C-Compiler =  -XO -Xsize-opt 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_C++-Compiler =  -XO -Xsize-opt 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Linker =  -XO -Xsize-opt 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Partial-Image-Linker = 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Librarian = 
cps11_v4_PLC/$(MODE_DIR)/% : DEBUGFLAGS_Assembler =  -XO -Xsize-opt 
endif
cps11_v4_PLC/$(MODE_DIR)/% : IDE_INCLUDES = -I$(WIND_BASE)/target/h -I$(WIND_BASE)/target/h/wrn/coreip 
cps11_v4_PLC/$(MODE_DIR)/% : IDE_LIBRARIES = 
cps11_v4_PLC/$(MODE_DIR)/% : IDE_DEFINES = -DCPU=_VX_$(CPU) -DTOOL_FAMILY=$(TOOL_FAMILY) -DTOOL=$(TOOL) -D_WRS_KERNEL -D_WRS_VX_SMP -D_WRS_CONFIG_SMP -D_VSB_CONFIG_FILE=\"$(VSB_DIR)/h/config/vsbConfig.h\"   -DIP_PORT_VXWORKS=69 
cps11_v4_PLC/$(MODE_DIR)/% : PROJECT_TYPE = DKM
cps11_v4_PLC/$(MODE_DIR)/% : DEFINES = 
cps11_v4_PLC/$(MODE_DIR)/% : EXPAND_DBG = 0
cps11_v4_PLC/$(MODE_DIR)/% : VX_CPU_FAMILY = ppc
cps11_v4_PLC/$(MODE_DIR)/% : CPU = PPC32
cps11_v4_PLC/$(MODE_DIR)/% : TOOL_FAMILY = diab
cps11_v4_PLC/$(MODE_DIR)/% : TOOL = diab
cps11_v4_PLC/$(MODE_DIR)/% : TOOL_PATH = 
cps11_v4_PLC/$(MODE_DIR)/% : CC_ARCH_SPEC = -tPPCFH:vxworks69 -Xstmw-slow -DPPC32_fp60x -Xstsw-slow
cps11_v4_PLC/$(MODE_DIR)/% : VSB_DIR = $(WIND_BASE)/target/lib_smp
cps11_v4_PLC/$(MODE_DIR)/% : VSB_CONFIG_FILE = $(VSB_DIR)/h/config/vsbConfig.h
cps11_v4_PLC/$(MODE_DIR)/% : LIBPATH = 
cps11_v4_PLC/$(MODE_DIR)/% : LIBS = 
cps11_v4_PLC/$(MODE_DIR)/% : OBJ_DIR := cps11_v4_PLC/$(MODE_DIR)

cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ADRC.o : $(PRJ_ROOT_DIR)/ADRC.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Inv.o : $(PRJ_ROOT_DIR)/GlobalVar_Inv.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Rec.o : $(PRJ_ROOT_DIR)/GlobalVar_Rec.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SHEPWM_Core_INV.o : $(PRJ_ROOT_DIR)/SHEPWM_Core_INV.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_INV.o : $(PRJ_ROOT_DIR)/SVPWM_Core_INV.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_REC.o : $(PRJ_ROOT_DIR)/SVPWM_Core_REC.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/anybus_pfb.o : $(PRJ_ROOT_DIR)/anybus_pfb.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ast7600Srio.o : $(PRJ_ROOT_DIR)/ast7600Srio.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/chopper.o : $(PRJ_ROOT_DIR)/chopper.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/closeControl.o : $(PRJ_ROOT_DIR)/closeControl.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/main.o : $(PRJ_ROOT_DIR)/main.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/protect.o : $(PRJ_ROOT_DIR)/protect.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/pwmControl.o : $(PRJ_ROOT_DIR)/pwmControl.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/slaveBoardRW.o : $(PRJ_ROOT_DIR)/slaveBoardRW.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ControlFunction.o : $(PRJ_ROOT_DIR)/subMod_ControlFunction.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_DataDisplayUDP.o : $(PRJ_ROOT_DIR)/subMod_DataDisplayUDP.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ModbusTcp.o : $(PRJ_ROOT_DIR)/subMod_ModbusTcp.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_Rfm2g.o : $(PRJ_ROOT_DIR)/subMod_Rfm2g.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/variable_main.o : $(PRJ_ROOT_DIR)/variable_main.c
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@"; $(TOOL_PATH)dcc $(DEBUGFLAGS_C-Compiler) $(CC_ARCH_SPEC) -W:c:,-Xclib-optim-off -Xansi -Xlocal-data-area-static-only  -W:c++:.CPP -Xc-new -Xdialect-c89  -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations   -Xmake-dependency=0xd $(IDE_DEFINES) $(DEFINES) $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES) -o "$@" -c "$<"


OBJECTS_cps11_v4_PLC = cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ADRC.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Inv.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Rec.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SHEPWM_Core_INV.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_INV.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_REC.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/anybus_pfb.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ast7600Srio.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/chopper.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/closeControl.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/main.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/protect.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/pwmControl.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/slaveBoardRW.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ControlFunction.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_DataDisplayUDP.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ModbusTcp.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_Rfm2g.o \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/variable_main.o

cps11_v4_PLC/$(MODE_DIR)/cps11_v4_PLC.out : $(OBJECTS_cps11_v4_PLC)
	$(TRACE_FLAG)if [ ! -d "`dirname "$@"`" ]; then mkdir -p "`dirname "$@"`"; fi;echo "building $@";rm -f "$@";ddump -Ng $(OBJECTS_cps11_v4_PLC) | tclsh $(WIND_BASE)/host/resource/hutils/tcl/munch.tcl -c ppc -tags $(VSB_DIR)/tags/ppc/PPC32/common/dkm.tags > $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dcc $(DEBUGFLAGS_Linker) $(CC_ARCH_SPEC) -Xdollar-in-ident -ei1518,4177,4223,4301,4550,5409,1606 -ei4193,4826,4381,4237,1573,4007,4082,4177,4223,4260,4550,5361,5828,2273,5387,5388 -ei1522,4092,4111,4152,4167,4171,4174,4186,4188,4191,4192,4223,4231,4236,4284,4375,4494,4513,5152,5457 -Xforce-declarations  $(ADDED_CFLAGS) $(IDE_INCLUDES) $(ADDED_INCLUDES)  $(IDE_DEFINES) $(DEFINES) -o $(OBJ_DIR)/ctdt.o -c $(OBJ_DIR)/ctdt.c; $(TOOL_PATH)dld -tPPCFH:vxworks69 -X -r5 -r4 -o "$@" $(OBJ_DIR)/ctdt.o $(OBJECTS_cps11_v4_PLC) $(IDE_LIBRARIES) $(LIBPATH) $(LIBS) $(ADDED_LIBPATH) $(ADDED_LIBS) && if [ "$(EXPAND_DBG)" = "1" ]; then plink "$@";fi

cps11_v4_PLC/$(MODE_DIR)/cps11_v4_PLC_compile_file : $(FILE) ;

_clean :: cps11_v4_PLC/$(MODE_DIR)/cps11_v4_PLC_clean

cps11_v4_PLC/$(MODE_DIR)/cps11_v4_PLC_clean : 
	$(TRACE_FLAG)if [ -d "cps11_v4_PLC" ]; then cd "cps11_v4_PLC"; rm -rf $(MODE_DIR); fi

DEP_FILES := cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ADRC.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Inv.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/GlobalVar_Rec.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SHEPWM_Core_INV.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_INV.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/SVPWM_Core_REC.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/anybus_pfb.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/ast7600Srio.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/chopper.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/closeControl.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/main.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/protect.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/pwmControl.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/slaveBoardRW.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ControlFunction.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_DataDisplayUDP.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_ModbusTcp.d cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/subMod_Rfm2g.d \
	 cps11_v4_PLC/$(MODE_DIR)/Objects/cps11_v4_PLC/variable_main.d
-include $(DEP_FILES)


WIND_SCOPETOOLS_BASE := $(subst \,/,$(WIND_SCOPETOOLS_BASE))

clean_scopetools :
	$(TRACE_FLAG)rm -rf $(PRJ_ROOT_DIR)/.coveragescope/db

CLEAN_STEP := clean_scopetools


-include $(PRJ_ROOT_DIR)/*.makefile

-include *.makefile

JOBS?=1
ifeq ($(JOBS),1)
main_all : external_build  $(PROJECT_TARGETS)
	@echo "make: built targets of `pwd`"
else
main_all : external_build 
	@$(MAKE) -f $(MAKEFILE) --jobs $(JOBS) $(MFLAGS) $(PROJECT_TARGETS);\
	echo "make: built targets of `pwd`"
endif 

# entry point for extending the build
external_build ::
	@echo ""

# main entry point for pre processing prior to the build
pre_build :: $(PRE_BUILD_STEP) generate_sources
	@echo ""

# entry point for generating sources prior to the build
generate_sources ::
	@echo ""

# main entry point for post processing after the build
post_build :: $(POST_BUILD_STEP) deploy_output
	@echo ""

# entry point for deploying output after the build
deploy_output ::
	@echo ""

clean :: external_clean $(CLEAN_STEP) _clean

# entry point for extending the build clean
external_clean ::
	@echo ""
