# IBM_PROLOG_BEGIN_TAG
# This is an automatically generated prolog.
#
# $Source: src/usr/secureboot/trusted/makefile $
#
# OpenPOWER HostBoot Project
#
# Contributors Listed Below - COPYRIGHT 2015,2017
# [+] International Business Machines Corp.
#
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
# implied. See the License for the specific language governing
# permissions and limitations under the License.
#
# IBM_PROLOG_END_TAG
ROOTPATH = ../../../..
MODULE = secureboot_trusted
PERV_HWP_PATH += ${ROOTPATH}/src/import/chips/p9/procedures/hwp/perv

#Hardware procedure includes
EXTRAINCDIR += ${PERV_HWP_PATH}
EXTRAINCDIR += ${ROOTPATH}/src/include/usr/fapi2/
EXTRAINCDIR += ${ROOTPATH}/src/import/hwpf/fapi2/include/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/common/utils/imageProcs/
EXTRAINCDIR += ${ROOTPATH}/src/import/chips/p9/common/include/

#Trustedboot compilation units
OBJS += $(if $(CONFIG_TPMDD),trustedboot.o,)
OBJS += $(if $(CONFIG_TPMDD),trustedbootCmds.o,)
OBJS += $(if $(CONFIG_TPMDD),trustedTypes.o,)
OBJS += $(if $(CONFIG_TPMDD),tpmLogMgr.o,)
OBJS += $(if $(CONFIG_TPMDD),trustedbootUtils.o,)

#Handle the hardware procedure link dependency
VPATH += ${PERV_HWP_PATH}

SUBDIRS += $(if $(CONFIG_TPMDD),test.d,)

CFLAGS += -iquote ../

#Include HWP procedure makefiles
include ${ROOTPATH}/procedure.rules.mk
include ${PERV_HWP_PATH}/p9_update_security_ctrl.mk

include ${ROOTPATH}/config.mk
