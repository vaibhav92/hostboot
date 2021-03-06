/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/cache/p9_l3err_extract.H $ */
/*                                                                        */
/* OpenPOWER HostBoot Project                                             */
/*                                                                        */
/* Contributors Listed Below - COPYRIGHT 2016,2017                        */
/* [+] International Business Machines Corp.                              */
/*                                                                        */
/*                                                                        */
/* Licensed under the Apache License, Version 2.0 (the "License");        */
/* you may not use this file except in compliance with the License.       */
/* You may obtain a copy of the License at                                */
/*                                                                        */
/*     http://www.apache.org/licenses/LICENSE-2.0                         */
/*                                                                        */
/* Unless required by applicable law or agreed to in writing, software    */
/* distributed under the License is distributed on an "AS IS" BASIS,      */
/* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or        */
/* implied. See the License for the specific language governing           */
/* permissions and limitations under the License.                         */
/*                                                                        */
/* IBM_PROLOG_END_TAG                                                     */

///----------------------------------------------------------------------------
///
/// @file p9_l3err_extract.H
///
/// @brief Parse and extract error information from L3 Scom Registers (FAPI2)
///
/// *HWP HWP Owner   : Chen Qian <qianqc@cn.ibm.com>
/// *HWP FW Owner    : Thi Tran <thi@us.ibm.com>
/// *HWP Team        : Quad
/// *HWP Consumed by : HB
/// *HWP Level       : 3
///----------------------------------------------------------------------------
#ifndef _P9_L3ERR_EXTRACT_H_
#define _P9_L3ERR_EXTRACT_H_

//------------------------------------------------------------------------------
// Includes
//------------------------------------------------------------------------------
#include <fapi2.H>

//------------------------------------------------------------------------------
// Structure definitions
//------------------------------------------------------------------------------

// enum used to denote what type of error occured or which we want to find
enum p9_l3err_extract_err_type
{
    L3ERR_CE,
    L3ERR_UE,
    L3ERR_SUE,
    L3ERR_CE_UE
};

// structure to document control/data registers for each supported trace array
struct p9_l3err_extract_err_data
{
    p9_l3err_extract_err_type ce_ue;
    uint8_t member;
    uint8_t dw;      //3-bit
    uint8_t bank;    //3-bit
    uint8_t dataout; //decimal 0-144, 8-bit
    uint16_t hashed_real_address_45_56;//12-bit
    uint16_t cache_read_address;//14-bit
};

// function pointer typedef definition for HWP call support
typedef fapi2::ReturnCode (*p9_l3err_extract_FP_t)(
    const fapi2::Target<fapi2::TARGET_TYPE_EX>& i_target,
    p9_l3err_extract_err_data& o_err_data,
    bool& o_err_found);

extern "C"
{

//------------------------------------------------------------------------------
// Function prototype
//------------------------------------------------------------------------------

///
/// @brief  FAPI p9_l3err_extract HWP entry point
///         Parse and extract L3 error information from provided
///
/// @param[in]  i_target      => EX chiplet target
/// @param[out] o_err_data    => Failing location information for CE or UE
/// @param[out] o_error_found => Error is found
///
    fapi2::ReturnCode p9_l3err_extract(
        const fapi2::Target<fapi2::TARGET_TYPE_EX>& i_target,
        p9_l3err_extract_err_data& o_err_data,
        bool& o_error_found);

} // extern "C"

#endif // _P9_L3ERR_EXTRACT_H_

