/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/generic/memory/lib/spd/lrdimm/ddr4/lrdimm_raw_cards.H $ */
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

///
/// @file raw_cards.H
/// @brief Raw card data structure
///
// *HWP HWP Owner: Andre Marin <aamarin@us.ibm.com>
// *HWP HWP Backup: Jacob Harvey <jlharvey@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: HB:FSP

#ifndef _MSS_LRDIMM_RAW_CARDS_H_
#define _MSS_LRDIMM_RAW_CARDS_H_

#include <fapi2.H>
#include <cstdint>
#include <vector>
#include <generic/memory/lib/spd/common/rcw_settings.H>

namespace mss
{
namespace lrdimm
{

///
/// @brief raw card revs for LRDIMMs
/// @note In the order of the vector below which needs to be sorted by enum value
///
enum raw_card_rev : uint8_t
{
    // TODO RTC:160116 Fill in valid RCD data for LRDIMM
    /// LRDIMM Raw Card encoding for B0
    B0 = 0x01,
};

// Raw cards can share the same raw card # between RDIMM and LRDIMMs so
// we track them independently. Since all of these don't come from SPD for DDR4,
// we have to set some RCWs (we want limit these and derive as many as possible)
extern const std::vector< std::pair< uint8_t, rcw_settings> > RAW_CARDS;

}// lrdimm

// Exposed so we can test them.
extern rcw_settings lrdimm_rc_b0;

}// mss

#endif //_MSS_LRDIMM_RAW_CARDS_H_
