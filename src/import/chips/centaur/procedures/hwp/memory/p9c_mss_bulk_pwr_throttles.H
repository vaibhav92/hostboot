/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/centaur/procedures/hwp/memory/p9c_mss_bulk_pwr_throttles.H $ */
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

/// @file p9c_mss_bulk_pwr_throttles.H
/// @brief  Sets the throttle attributes based on a power limit for the dimms on the channel pair
///
/// *HWP HWP Owner: Luke Mulkey <lwmulkey@us.ibm.com>
/// *HWP HWP Backup: Mike Pardeik <pardeik@us.ibm.com>
/// *HWP Team: Memory
/// *HWP Level: 2
/// *HWP Consumed by: HB
///

#ifndef MSS_BULK_PWR_THROTTLES_H_
#define MSS_BULK_PWR_THROTTLES_H_

//------------------------------------------------------------------------------
//  Includes
//------------------------------------------------------------------------------
#include <fapi2.H>

typedef fapi2::ReturnCode (*p9c_mss_bulk_pwr_throttles_FP_t)
(
    const fapi2::Target<fapi2::TARGET_TYPE_MBA>&
);

extern "C"
{
    /// @brief mss_bulk_pwr_throttles procedure. Set dimm and channel throttle attributes based on available centaur mba port power
    /// @param[in]  i_target_mba  Reference to centaur mba target
    /// @return ReturnCode
    fapi2::ReturnCode p9c_mss_bulk_pwr_throttles(const fapi2::Target<fapi2::TARGET_TYPE_MBA>& i_target_mba);

} // extern "C"

#endif // MSS_BULK_PWR_THROTTLES_H_
