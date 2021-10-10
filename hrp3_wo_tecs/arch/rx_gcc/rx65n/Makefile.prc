#
#		Makefile のプロセッサ依存部（RX65N用）
#

#
#  プロセッサ依存部ディレクトリ名の定義
#
PRCDIR = $(SRCDIR)/arch/$(CORE)_$(TOOL)/$(PRC)

#
#  コンパイルオプション
#
INCLUDES += -I$(PRCDIR)
COPTS += -misa=v2
ASFLAGS += -misa=v2

#
#  カーネルに関する定義
#
KERNEL_DIRS += $(PRCDIR)
KERNEL_COBJS += prc_timer.o prc_kernel_impl.o
KERNEL_ASMOBJS += start.o
SYSSVC_COBJS += prc_serial.o

#
#  コンフィギュレータ関係の変数の定義
#
CFG_TABS := $(CFG_TABS) --symval-table $(PRCDIR)/prc_sym.def

#
#  コア依存部
#
include $(SRCDIR)/arch/$(CORE)_$(TOOL)/common/Makefile.core
