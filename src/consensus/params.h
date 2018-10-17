// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2018 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_PARAMS_H
#define BITCOIN_CONSENSUS_PARAMS_H

#include <amount.h>
#include <uint256.h>
#include <limits>
#include <map>
#include <string>

#include <memory>

namespace Consensus {

/**
 * Identifiers for forks done on the network, so that validation code can
 * easily just query whether or not a particular fork should be active and
 * does not have to bother with the particular heights or other aspects.
 */
enum class Fork
{

  /**
   * Fork done after the token sale.  This removed the requirement that the
   * main (non-fakeheader) nonce must be zero in order to resolve
   * https://github.com/xaya/xaya/issues/50.
   *
   * TODO: Also adjust block rewards to give the final coin supply.
   */
  POST_ICO,

};

/**
 * Interface for classes that define consensus behaviour in more
 * complex ways than just by a set of constants.
 */
class ConsensusRules
{
public:

    /* Provide a virtual destructor since we have virtual methods.  */
    virtual ~ConsensusRules() = default;

    /* Return minimum locked amount in a name.  */
    virtual CAmount MinNameCoinAmount(unsigned nHeight) const = 0;

    /**
     * Checks whether a given fork is in effect at the given block height.
     */
    virtual bool ForkInEffect(Fork type, unsigned height) const = 0;

};

class MainNetConsensus : public ConsensusRules
{
public:

    CAmount MinNameCoinAmount(unsigned nHeight) const override
    {
        return COIN / 100;
    }

    bool ForkInEffect(const Fork type, const unsigned height) const override
    {
        switch (type)
        {
            case Fork::POST_ICO:
                /* FIXME: Set correct height once determined.  */
                return height >= 1000000;
            default:
                assert (false);
        }
    }

};

class TestNetConsensus : public MainNetConsensus
{
public:

    bool ForkInEffect(const Fork type, const unsigned height) const override
    {
        switch (type)
        {
            case Fork::POST_ICO:
                /* FIXME: Set correct height once determined.  */
                return height >= 1000000;
            default:
                assert (false);
        }
    }

};

class RegTestConsensus : public TestNetConsensus
{
public:

    bool ForkInEffect(const Fork type, const unsigned height) const override
    {
        switch (type)
        {
            case Fork::POST_ICO:
                return height >= 500;
            default:
                assert (false);
        }
    }
};

enum DeploymentPos
{
    DEPLOYMENT_TESTDUMMY,
    // NOTE: Also add new deployments to VersionBitsDeploymentInfo in versionbits.cpp
    MAX_VERSION_BITS_DEPLOYMENTS
};

/**
 * Struct for each individual consensus rule change using BIP9.
 */
struct BIP9Deployment {
    /** Bit position to select the particular bit in nVersion. */
    int bit;
    /** Start MedianTime for version bits miner confirmation. Can be a date in the past */
    int64_t nStartTime;
    /** Timeout/expiry MedianTime for the deployment attempt. */
    int64_t nTimeout;

    /** Constant for nTimeout very far in the future. */
    static constexpr int64_t NO_TIMEOUT = std::numeric_limits<int64_t>::max();

    /** Special value for nStartTime indicating that the deployment is always active.
     *  This is useful for testing, as it means tests don't need to deal with the activation
     *  process (which takes at least 3 BIP9 intervals). Only tests that specifically test the
     *  behaviour during activation cannot use this. */
    static constexpr int64_t ALWAYS_ACTIVE = -1;
};

/**
 * Parameters that influence chain consensus.
 */
struct Params {
    uint256 hashGenesisBlock;
    int nSubsidyHalvingInterval;
    /** Initial block reward.  */
    CAmount initialSubsidy;
    /** Block height at which BIP16 becomes active */
    int BIP16Height;
    /** Block height at which BIP34 becomes active */
    int BIP34Height;
    /** Block height at which BIP65 becomes active */
    int BIP65Height;
    /** Block height at which BIP66 becomes active */
    int BIP66Height;
    /**
     * Minimum blocks including miner confirmation of the total of 2016 blocks in a retargeting period,
     * (nPowTargetTimespan / nPowTargetSpacing) which is also used for BIP9 deployments.
     * Examples: 1916 for 95%, 1512 for testchains.
     */
    uint32_t nRuleChangeActivationThreshold;
    uint32_t nMinerConfirmationWindow;
    BIP9Deployment vDeployments[MAX_VERSION_BITS_DEPLOYMENTS];
    /** Proof of work parameters */
    uint256 powLimitNeoscrypt;
    bool fPowNoRetargeting;
    int64_t nPowTargetSpacing;
    uint256 nMinimumChainWork;
    uint256 defaultAssumeValid;

    /** Auxpow parameters */
    int32_t nAuxpowChainId;

    /** Consensus rule interface.  */
    std::unique_ptr<ConsensusRules> rules;
};
} // namespace Consensus

#endif // BITCOIN_CONSENSUS_PARAMS_H
