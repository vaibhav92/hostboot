/* IBM_PROLOG_BEGIN_TAG                                                   */
/* This is an automatically generated prolog.                             */
/*                                                                        */
/* $Source: src/import/chips/p9/procedures/hwp/memory/lib/eff_config/plug_rules.C $ */
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
/// @file plug_rules.C
/// @brief Enforcement of rules for plugging in DIMM
///
// *HWP HWP Owner: Jacob L Harvey <jlharvey@us.ibm.com>
// *HWP HWP Backup: Andre Marin <aamarin@us.ibm.com>
// *HWP Team: Memory
// *HWP Level: 3
// *HWP Consumed by: FSP:HB

#include <fapi2.H>
#include <vpd_access.H>
#include <mss.H>
#include <lib/mss_vpd_decoder.H>

#include <lib/dimm/rank.H>
#include <lib/utils/assert_noexit.H>
#include <lib/eff_config/plug_rules.H>

using fapi2::TARGET_TYPE_MCA;
using fapi2::TARGET_TYPE_MCS;
using fapi2::TARGET_TYPE_DIMM;

using fapi2::FAPI2_RC_SUCCESS;
using fapi2::FAPI2_RC_INVALID_PARAMETER;

namespace mss
{

namespace plug_rule
{

namespace code
{

///
/// @brief Checks for invalid LRDIMM plug combinations
/// @param[in] i_kinds a vector of DIMM (sorted while procesing)
/// @return fapi2::FAPI2_RC_SUCCESS if no LRDIMM, otherwise a MSS_PLUG_RULE error code
/// @note This function will commit error logs representing the mixing failure
///
fapi2::ReturnCode check_lrdimm( const std::vector<dimm::kind>& i_kinds )
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // If we have 0 DIMMs on the port, we don't care
    for(const auto& l_kind : i_kinds)
    {
        FAPI_ASSERT( l_kind.iv_dimm_type != fapi2::ENUM_ATTR_EFF_DIMM_TYPE_LRDIMM,
                     fapi2::MSS_PLUG_RULES_LRDIMM_UNSUPPORTED()
                     .set_DIMM_TARGET(l_kind.iv_target),
                     "%s has an LRDIMM plugged and is currently unsupported",
                     mss::c_str(l_kind.iv_target) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // code

///
/// @brief Enforce DRAM width checks
/// @note DIMM0's width needs to equal DIMM1's width
/// @param[in] i_target the port
/// @param[in] i_kinds a vector of DIMM (sorted while processing)
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note Expects the kind array to represent the DIMM on the port.
///
fapi2::ReturnCode check_dram_width(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                   const std::vector<dimm::kind>& i_kinds)
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // Only do this check if we have the maximum number DIMM kinds
    if(i_kinds.size() == MAX_DIMM_PER_PORT)
    {
        FAPI_ASSERT( i_kinds[0].iv_dram_width == i_kinds[1].iv_dram_width,
                     fapi2::MSS_PLUG_RULES_INVALID_DRAM_WIDTH_MIX()
                     .set_DIMM_SLOT_ZERO(i_kinds[0].iv_dram_width)
                     .set_DIMM_SLOT_ONE(i_kinds[1].iv_dram_width)
                     .set_MCA_TARGET(i_target),
                     "%s has DIMM's with two different DRAM widths installed of type %d and of type %d. Cannot mix DIMM of different widths on a single port",
                     mss::c_str(i_target), i_kinds[0].iv_dram_width, i_kinds[1].iv_dram_width );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforce hybrid DIMM checks
/// @note No hybrid/non-hybrid and no different hybrid types
/// @param[in] i_target the port
/// @param[in] i_kinds a vector of DIMM (sorted while processing)
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note Expects the kind array to represent the DIMM on the port.
///
fapi2::ReturnCode check_hybrid(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                               const std::vector<dimm::kind>& i_kinds)
{
    // Make sure we don't get a stale error
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // Skip the below checks if we have less than the maximum number of DIMM's
    if(i_kinds.size() < MAX_DIMM_PER_PORT)
    {
        return fapi2::FAPI2_RC_SUCCESS;
    }

    // Assert that we do not have an error
    FAPI_ASSERT( i_kinds[0].iv_hybrid == i_kinds[1].iv_hybrid,
                 fapi2::MSS_PLUG_RULES_INVALID_HYBRID_MIX()
                 .set_DIMM_SLOT_ZERO(i_kinds[0].iv_hybrid)
                 .set_DIMM_SLOT_ONE(i_kinds[1].iv_hybrid)
                 .set_MCA_TARGET(i_target),
                 "%s has DIMM's with two different hybrid types installed (type %d and type %d). Cannot mix DIMM of different hybrid types on a single port",
                 mss::c_str(i_target), i_kinds[0].iv_hybrid, i_kinds[1].iv_hybrid );

    // Only do the below check if the DIMM's are hybrid DIMM's
    if(i_kinds[0].iv_hybrid == fapi2::ENUM_ATTR_EFF_HYBRID_IS_HYBRID)
    {
        // Assert that we do not have an error
        FAPI_ASSERT( i_kinds[0].iv_hybrid_memory_type == i_kinds[1].iv_hybrid_memory_type,
                     fapi2::MSS_PLUG_RULES_INVALID_HYBRID_MEMORY_TYPE_MIX()
                     .set_DIMM_SLOT_ZERO(i_kinds[0].iv_hybrid_memory_type)
                     .set_DIMM_SLOT_ONE(i_kinds[1].iv_hybrid_memory_type)
                     .set_MCA_TARGET(i_target),
                     "%s has DIMM's with two different hybrid memory types installed (type %d and type %d). Cannot mix DIMM of different hybrid memory types on a single port",
                     mss::c_str(i_target), i_kinds[0].iv_hybrid_memory_type, i_kinds[1].iv_hybrid_memory_type );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforce DRAM stack type checks
/// @note No 3DS and non-3DS DIMM's can mix
/// @param[in] i_target the port
/// @param[in] i_kinds a vector of DIMM (sorted while processing)
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note Expects the kind array to represent the DIMM on the port.
///
fapi2::ReturnCode check_stack_type(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                   const std::vector<dimm::kind>& i_kinds)
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // Only do this check if we have the maximum number DIMM kinds
    if(i_kinds.size() == MAX_DIMM_PER_PORT)
    {
        // Only do the assert if we have any 3DS DIMM, as the chip bug is for mixed config between DIMM that use and do not use CID
        // Note: we should be able to mix SDP and DDP ok, as both DIMM's do not use CID
        const bool l_has_3ds = (i_kinds[0].iv_stack_type == fapi2::ENUM_ATTR_EFF_PRIM_STACK_TYPE_3DS) ||
                               (i_kinds[1].iv_stack_type == fapi2::ENUM_ATTR_EFF_PRIM_STACK_TYPE_3DS);

        // We have an error if we have the below scenario of 3DS and the stack types not being equal
        const auto l_error = l_has_3ds && i_kinds[0].iv_stack_type != i_kinds[1].iv_stack_type;

        FAPI_DBG("%s %s 3DS. Stack types are %s (%u,%u). configuration %s ok.",
                 mss::c_str(i_target),
                 l_has_3ds ? "has" : "does not have",
                 (i_kinds[0].iv_stack_type != i_kinds[1].iv_stack_type) ? "not equal" : "equal",
                 i_kinds[0].iv_stack_type,
                 i_kinds[1].iv_stack_type,
                 l_error ? "isn't" : "is"
                );

        // Assert that we do not have an error
        FAPI_ASSERT( !l_error,
                     fapi2::MSS_PLUG_RULES_INVALID_STACK_TYPE_MIX()
                     .set_DIMM_SLOT_ZERO(i_kinds[0].iv_stack_type)
                     .set_DIMM_SLOT_ONE(i_kinds[1].iv_stack_type)
                     .set_MCA_TARGET(i_target),
                     "%s has DIMM's with two different stack types installed (type %d and type %d). Cannot mix DIMM of different stack types on a single port",
                     mss::c_str(i_target), i_kinds[0].iv_stack_type, i_kinds[1].iv_stack_type );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Helper to evaluate the unsupported rank config override attribute
/// @param[in] i_dimm0_ranks count of the ranks on DIMM in slot 0
/// @param[in] i_dimm1_ranks count of the ranks on DIMM in slot 1
/// @param[in] i_attr value of the attribute containing the unsupported rank configs
/// @return true iff this rank config is supported according to the unsupported attribute
/// @note not to be used to enforce populated/unpopulated - e.g., 0 ranks in both slots is ignored
///
bool unsupported_rank_helper(const uint64_t i_dimm0_ranks,
                             const uint64_t i_dimm1_ranks,
                             const fapi2::buffer<uint64_t>& i_attr)
{
    // Quick - if the attribute is 0 (typically is) then we're out.
    if (i_attr == 0)
    {
        FAPI_INF("(%d, %d) is supported, override empty", i_dimm0_ranks, i_dimm1_ranks);
        return true;
    }

    // Quick - if both rank configs are 0 (no ranks seen in any slots) we return true. This is always OK.
    if ((i_dimm0_ranks == 0) && (i_dimm1_ranks == 0))
    {
        FAPI_INF("(%d, %d) is always supported", i_dimm0_ranks, i_dimm1_ranks);
        return true;
    }

    // We use 8 bits to represent a config in the unsupported ranks attribute. Each 'config' is a byte in
    // the attribute. The left nibble is the count of ranks on DIMM0, right nibble is the count of unsupported
    // ranks on DIMM1. Total ranks so we need the bits to represent stacks too.
    uint64_t l_current_byte  = 0;

    do
    {
        uint8_t  l_config = 0;
        uint64_t l_current_dimm0 = 0;
        uint64_t l_current_dimm1 = 0;

        i_attr.extractToRight(l_config, l_current_byte * BITS_PER_BYTE, BITS_PER_BYTE);

        fapi2::buffer<uint8_t>(l_config).extractToRight<0,               BITS_PER_NIBBLE>(l_current_dimm0);
        fapi2::buffer<uint8_t>(l_config).extractToRight<BITS_PER_NIBBLE, BITS_PER_NIBBLE>(l_current_dimm1);

        FAPI_INF("Seeing 0x%x for unsupported rank config (%d, %d)", l_config, l_current_dimm0, l_current_dimm1);

        if ((l_current_dimm0 == i_dimm0_ranks) && (l_current_dimm1 == i_dimm1_ranks))
        {
            FAPI_INF("(%d, %d) is unsupported", i_dimm0_ranks, i_dimm1_ranks);
            return false;
        }

    }
    while (++l_current_byte < sizeof(uint64_t));

    FAPI_INF("(%d, %d) is supported", i_dimm0_ranks, i_dimm1_ranks);
    return true;
}

///
/// @brief Enforce no mixing DIMM types
/// @param[in] i_target port target
/// @param[in] i_kinds a vector of DIMMs plugged into target
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note This function will commit error logs representing the mixing failure
///
fapi2::ReturnCode dimm_type_mixing(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target,
                                   const std::vector<dimm::kind>& i_kinds)
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    //If we have 1 or 0 DIMMs on the port, we don't care
    if (i_kinds.size() == MAX_DIMM_PER_PORT)
    {
        FAPI_ASSERT( i_kinds[0].iv_dimm_type == i_kinds[1].iv_dimm_type,
                     fapi2::MSS_PLUG_RULES_INVALID_DIMM_TYPE_MIX()
                     .set_DIMM_SLOT_ZERO(i_kinds[0].iv_dimm_type)
                     .set_DIMM_SLOT_ONE(i_kinds[1].iv_dimm_type)
                     .set_MCA_TARGET(i_target),
                     "%s has two different types of DIMM installed of type %d and of type %d. Cannot mix DIMM types on port",
                     mss::c_str(i_target), i_kinds[0].iv_dimm_type, i_kinds[1].iv_dimm_type );
    }

fapi_try_exit:
    return fapi2::current_err;
}


///
/// @brief Enforce having one solitary DIMM plugged into slot 0
/// @param[in] i_target port target
/// @return fapi2::FAPI2_RC_SUCCESS if okay
///
template<>
fapi2::ReturnCode empty_slot_zero(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target)
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;
    const auto l_dimms = mss::find_targets<TARGET_TYPE_DIMM>(i_target);

    // If there's one dimm, make sure it's in slot 0
    if ( l_dimms.size() == 1 )
    {
        FAPI_ASSERT(  mss::index(l_dimms[0]) == 0,
                      fapi2::MSS_PLUG_RULES_SINGLE_DIMM_IN_WRONG_SLOT()
                      .set_MCA_TARGET(i_target)
                      .set_DIMM_TARGET(l_dimms[0]),
                      "%s DIMM is plugged into the wrong slot. Must plug into slot 0", mss::c_str(l_dimms[0]) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforce having one solitary DIMM plugged into slot 0
/// @param[in] i_target the MCS
/// @return fapi2::FAPI2_RC_SUCCESS if okay
///
template<>
fapi2::ReturnCode empty_slot_zero( const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target)
{
    fapi2::ReturnCode l_rc(fapi2::current_err);

    for (const auto& p : mss::find_targets<fapi2::TARGET_TYPE_MCA>(i_target) )
    {
        FAPI_TRY( empty_slot_zero( p ) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforces the DEAD LOAD rules
/// @param[in] i_target the MCA/ port target
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note This function will deconfigure the port if it's dual drop and one of the dimms is deconfigured
///
template<>
fapi2::ReturnCode check_dead_load( const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target)
{
    const auto l_plugged_dimms = i_target.getChildren<TARGET_TYPE_DIMM>(fapi2::TARGET_STATE_PRESENT);
    const auto l_functional_dimms = mss::find_targets<TARGET_TYPE_DIMM>(i_target);
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    // If we have one deconfigured dimm and one functional dimm, we need to deconfigure the functional dimm
    FAPI_DBG(" Plugged dimm size is %d functional dimms size is %d",
             l_plugged_dimms.size(),
             l_functional_dimms.size());


    // So we check to see if there are functional dimms on the port,
    // if so, we check to see if there are less functional dimms than plugged in dimms (meaning one was deconfigured)
    // Third let's just double check we have two plugged dimms
    // The last check is for clarity
    if ( (l_functional_dimms.size() != 0)
         && (l_plugged_dimms.size() != l_functional_dimms.size())
         && (l_plugged_dimms.size() == 2) )
    {
        auto l_dead_dimm = l_plugged_dimms[0];
        auto l_live_dimm = l_plugged_dimms[1];

        // Now we determine if present_dimm[0] is functional by searching functional dimms
        const auto l_found = std::find(l_functional_dimms.begin(), l_functional_dimms.end(), l_dead_dimm);

        // if we don't find the dimm in the list of functional dimms, then it's deconfigured and the guess was good
        // Otherwise, we swap because our first guess was wrong
        l_dead_dimm = ( l_found == l_functional_dimms.end() ) ? l_dead_dimm : l_plugged_dimms[1];
        l_live_dimm = ( l_found == l_functional_dimms.end() ) ? l_live_dimm : l_plugged_dimms[0];
        FAPI_ASSERT( false,
                     fapi2::MSS_DEAD_LOAD_ON_PORT()
                     .set_FUNCTIONAL_DIMM(l_live_dimm),
                     "%s has two DIMMs installed, but one is deconfigured (%d), so deconfiguring the other (%d) because of dead load",
                     mss::c_str(i_target), mss::index(l_dead_dimm), mss::index(l_live_dimm));
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforces the DEAD LOAD rules
/// @param[in] i_target the MCA/ port target
/// @return fapi2::FAPI2_RC_SUCCESS if okay
///
template<>
fapi2::ReturnCode check_dead_load( const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target)
{
    fapi2::ReturnCode l_rc(fapi2::current_err);

    for (const auto& p : mss::find_targets<fapi2::TARGET_TYPE_MCA>(i_target) )
    {
        FAPI_TRY( check_dead_load( p ) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Function to check the DRAM generation for DDR4
/// @param[in] i_kinds a vector of DIMM kind structs
/// @return fapi2::ReturnCode
///
fapi2::ReturnCode check_gen( const std::vector<dimm::kind>& i_kinds )
{
    fapi2::current_err = fapi2::FAPI2_RC_SUCCESS;

    for (const auto& k : i_kinds)
    {
        // This should never fail ... but Just In Case a little belt-and-suspenders never hurt.
        // TODO RTC:160395 This needs to change for controllers which support different generations
        // Nimbus only supports DDR4 for now
        FAPI_ASSERT( k.iv_dram_generation == fapi2::ENUM_ATTR_EFF_DRAM_GEN_DDR4 ||
                     k.iv_dram_generation == fapi2::ENUM_ATTR_EFF_DRAM_GEN_EMPTY,
                     fapi2::MSS_PLUG_RULES_INVALID_DRAM_GEN()
                     .set_DRAM_GEN(k.iv_dimm_type)
                     .set_DIMM_TARGET(k.iv_target),
                     "%s is not DDR4 it is %d", mss::c_str(k.iv_target), k.iv_dram_generation );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforce rank configs
/// Enforces rank configurations which are not part of the VPD/rank config thing.
/// @note Reads an MRW attribute to further limit rank configs.
/// @param[in] i_target the port
/// @param[in] i_kinds a vector of DIMM (sorted while processing)
/// @param[in] i_ranks_override value of mrw_unsupported_rank_config attribute
/// @return fapi2::FAPI2_RC_SUCCESS if okay
/// @note Expects the kind array to represent the DIMM on the port.
///
fapi2::ReturnCode check_rank_config(const fapi2::Target<TARGET_TYPE_MCA>& i_target,
                                    const std::vector<dimm::kind>& i_kinds,
                                    const uint64_t i_ranks_override)
{
    // We need to keep track of current_err ourselves as the FAPI_ASSERT_NOEXIT macro doesn't.
    fapi2::current_err = FAPI2_RC_SUCCESS;


    // The user can avoid plug rules with an attribute. This is handy in partial good scenarios
    uint8_t l_ignore_plug_rules = 0;
    FAPI_TRY( mss::ignore_plug_rules(mss::find_target<TARGET_TYPE_MCS>(i_target), l_ignore_plug_rules) );

    if (fapi2::ENUM_ATTR_MSS_IGNORE_PLUG_RULES_YES == l_ignore_plug_rules)
    {
        FAPI_INF("attribute set to ignore plug rules");
        return FAPI2_RC_SUCCESS;
    }

    // If we have one DIMM, make sure it's in slot 0 and we're done.
    if (i_kinds.size() == 1)
    {
        // Sets fapi2::current_err
        FAPI_ASSERT( mss::index(i_kinds[0].iv_target) == 0,
                     fapi2::MSS_PLUG_RULES_SINGLE_DIMM_IN_WRONG_SLOT()
                     .set_MCA_TARGET(i_target)
                     .set_DIMM_TARGET(i_kinds[0].iv_target),
                     "%s is in slot 1, should be in slot 0", mss::c_str(i_kinds[0].iv_target));

        // Check to see if the override attribute limits this single slot configuration. Since we assert above
        // we know now i_kinds[0].iv_target is the DIMM in slot 0
        FAPI_ASSERT( unsupported_rank_helper(i_kinds[0].iv_total_ranks, 0, i_ranks_override) == true,
                     fapi2::MSS_PLUG_RULES_OVERRIDDEN_RANK_CONFIG()
                     .set_RANKS_ON_DIMM0(i_kinds[0].iv_total_ranks)
                     .set_RANKS_ON_DIMM1(0)
                     .set_TARGET(i_target),
                     "MRW overrides this rank configuration (single DIMM) ranks: %d %s",
                     i_kinds[0].iv_total_ranks, mss::c_str(i_target) );
    }

    // So if we're here we know we have more than one DIMM on this port.
    else
    {
        // Total up the number of ranks on this port. If it's more than MAX_PRIMARY_RANKS_PER_PORT we have a problem.
        // Notice that totaling up the ranks and using that as a metric also catches the 4R-is-not-the-only-DIMM case
        // (really probably that's the only case it catches but <shhhhh>.)
        // I don't think f/w supports std::count ... There aren't many DIMM on this port ...
        uint64_t l_rank_count = 0;
        const dimm::kind* l_dimm0_kind = nullptr;
        const dimm::kind* l_dimm1_kind = nullptr;

        for (const auto& k : i_kinds)
        {
            // While we're here, lets look for the DIMM on slots 0/1 - we'll need them later
            if (mss::index(k.iv_target) == 0)
            {
                l_dimm0_kind = &k;
            }
            else
            {
                l_dimm1_kind = &k;
            }

            l_rank_count += k.iv_master_ranks;
        }

        // If we get here and we see there's no DIMM in slot 0, we did something very wrong. We shouldn't have
        // passed the i_kinds.size() == 1 test above. So lets assert, shouldn't happen, but tracking the nullptr
        // dereference is harder <grin>
        if ((l_dimm0_kind == nullptr) || (l_dimm1_kind == nullptr))
        {
            FAPI_ERR("seeing a nullptr for DIMM0 or DIMM1, which is terrible %s %d", mss::c_str(i_target), i_kinds.size() );
            fapi2::Assert(false);
        }

        // Belt-and-suspenders as we make this assumption below
        if (i_kinds.size() > 2)
        {
            FAPI_ERR("seeing more than 2 DIMM on this port %s %d", mss::c_str(i_target), i_kinds.size() );
            fapi2::Assert(false);
        }

        // Safe to use l_dimm0_kind.
        MSS_ASSERT_NOEXIT( l_rank_count <= MAX_PRIMARY_RANKS_PER_PORT,
                           fapi2::MSS_PLUG_RULES_INVALID_PRIMARY_RANK_COUNT()
                           .set_TOTAL_RANKS(l_rank_count)
                           .set_DIMM_ZERO_MASTER_RANKS(l_dimm0_kind->iv_master_ranks)
                           .set_DIMM_ONE_MASTER_RANKS(l_dimm1_kind->iv_master_ranks)
                           .set_MCA_TARGET(i_target),
                           "There are more than %d master ranks on %s (%d)",
                           MAX_PRIMARY_RANKS_PER_PORT, mss::c_str(i_target), l_rank_count );

        FAPI_INF("DIMM in slot 0 %s has %d master ranks, DIMM1 has %d",
                 mss::c_str(l_dimm0_kind->iv_target), l_dimm0_kind->iv_master_ranks, l_dimm1_kind->iv_master_ranks);

        // The DIMMs master ranks have to be the same, we allow different slave ranks
        FAPI_ASSERT( l_dimm0_kind->iv_master_ranks == l_dimm1_kind->iv_master_ranks,
                     fapi2::MSS_PLUG_RULES_INVALID_RANK_CONFIG()
                     .set_RANKS_ON_DIMM0(l_dimm0_kind->iv_master_ranks)
                     .set_RANKS_ON_DIMM1(l_dimm1_kind->iv_master_ranks)
                     .set_TARGET(i_target),
                     "The DIMM configuration on %s is incorrect. Master ranks on [1][0]: %d,%d",
                     mss::c_str(i_target), l_dimm0_kind->iv_master_ranks, l_dimm1_kind->iv_master_ranks );

        // Check to see if the override attribute limits this configuration.
        FAPI_ASSERT( unsupported_rank_helper(l_dimm0_kind->iv_total_ranks,
                                             l_dimm1_kind->iv_total_ranks,
                                             i_ranks_override) == true,
                     fapi2::MSS_PLUG_RULES_OVERRIDDEN_RANK_CONFIG()
                     .set_RANKS_ON_DIMM0(l_dimm0_kind->iv_total_ranks)
                     .set_RANKS_ON_DIMM1(l_dimm1_kind->iv_total_ranks)
                     .set_TARGET(i_target),
                     "MRW overrides this rank configuration ranks: %d, %d %s",
                     l_dimm0_kind->iv_total_ranks, l_dimm1_kind->iv_total_ranks, mss::c_str(i_target) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

} // close namespace plug_rule

///
/// @brief Enforce the plug-rules per MCS
/// @param[in] i_target FAPI2 target (MCS)
/// @return fapi2::FAPI2_RC_SUCCESS if okay, otherwise a MSS_PLUG_RULE error code
///
fapi2::ReturnCode plug_rule::enforce_plug_rules(const fapi2::Target<fapi2::TARGET_TYPE_MCS>& i_target)
{
    // Check per-MCS plug rules. If those all pass, check each of our MCA
    const auto l_dimms = mss::find_targets<TARGET_TYPE_DIMM>(i_target);
    fapi2::ReturnCode l_rc (fapi2::FAPI2_RC_SUCCESS);

    // Check to see that we have DIMM on this MCS. If we don't, just carry on - this is valid.
    // Cronus does this often; they don't deconfigure empty ports or controllers. However, f/w
    // does. So if we're here we're running on Cronus or f/w has a bug <grin>
    if (l_dimms.size() == 0)
    {
        FAPI_INF("No DIMM configured for MCS %s, but it itself seems configured", mss::c_str(i_target));
        return FAPI2_RC_SUCCESS;
    }

    // The user can avoid plug rules with an attribute. This is handy in partial good scenarios
    uint8_t l_ignore_plug_rules = 0;
    FAPI_TRY( mss::ignore_plug_rules(i_target, l_ignore_plug_rules) );

    if (fapi2::ENUM_ATTR_MSS_IGNORE_PLUG_RULES_YES == l_ignore_plug_rules)
    {
        FAPI_INF("attribute set to ignore plug rules");
        return FAPI2_RC_SUCCESS;
    }

    for (const auto& p : mss::find_targets<TARGET_TYPE_MCA>(i_target))
    {
        // Difference between cronus and hostboot make it really annoying to loop over the targets,
        // So we'll just error out if we find a bad port, this could make 2 deconfig loops instead of one,
        // but it's worth for proper behavior and really shouldn't happen often
        FAPI_TRY( enforce_plug_rules(p) );
    }

fapi_try_exit:
    return fapi2::current_err;
}

///
/// @brief Enforce the plug-rules per MCA
/// @param[in] i_target FAPI2 target (MCA)
/// @return fapi2::FAPI2_RC_SUCCESS if okay, otherwise a MSS_PLUG_RULE error code
///
fapi2::ReturnCode plug_rule::enforce_plug_rules(const fapi2::Target<fapi2::TARGET_TYPE_MCA>& i_target)
{
    const auto l_dimms = mss::find_targets<TARGET_TYPE_DIMM>(i_target);

    // Check to see that we have DIMM on this MCA. If we don't, just carry on - this is valid.
    // Cronus does this often; they don't deconfigure empty ports or controllers. However, f/w
    // does. So if we're here we're running on Cronus or f/w has a bug <grin>
    if (l_dimms.size() == 0)
    {
        FAPI_INF("No DIMM configured for MCA %s, but it itself seems configured", mss::c_str(i_target));
        return FAPI2_RC_SUCCESS;
    }

    // Safe, even though the VPD decoder can get us here before the rest of eff_config has completed.
    // We'll only use the master rank information to enforce the rank config rules (which will have been
    // decoded and are valid before VPD was asked for.)
    const auto l_dimm_kinds = mss::dimm::kind::vector(l_dimms);

    uint64_t l_ranks_override = 0;

    // The user can avoid plug rules with an attribute. This is handy in partial good scenarios
    uint8_t l_ignore_plug_rules = 0;
    FAPI_TRY( mss::ignore_plug_rules(mss::find_target<TARGET_TYPE_MCS>(i_target), l_ignore_plug_rules) );

    if (fapi2::ENUM_ATTR_MSS_IGNORE_PLUG_RULES_YES == l_ignore_plug_rules)
    {
        FAPI_INF("attribute set to ignore plug rules");
        return FAPI2_RC_SUCCESS;
    }

    FAPI_TRY( check_gen( l_dimm_kinds ) );

    FAPI_TRY( dimm_type_mixing( i_target, l_dimm_kinds ) );

    // Get the MRW blacklist for rank configurations
    FAPI_TRY( mss::mrw_unsupported_rank_config(i_target, l_ranks_override) );

    // Note that we do limited rank config checking here. Most of the checking is done via VPD decoding,
    // meaning that if the VPD decoded the config then there's only a few rank related issues we need
    // to check here.
    FAPI_TRY( plug_rule::check_rank_config(i_target, l_dimm_kinds, l_ranks_override) );

    // Ensures that the port has a valid combination of DRAM widths
    FAPI_TRY( plug_rule::check_dram_width(i_target, l_dimm_kinds) );

    // Ensures that the port has a valid combination of stack types
    FAPI_TRY( plug_rule::check_stack_type(i_target, l_dimm_kinds) );

    // Ensures that the port has a valid combination of hybrid DIMM
    FAPI_TRY( plug_rule::check_hybrid(i_target, l_dimm_kinds) );

    // Checks to see if any DIMM are LRDIMM
    FAPI_TRY( plug_rule::code::check_lrdimm(l_dimm_kinds) );

fapi_try_exit:
    return fapi2::current_err;
}
}// mss
