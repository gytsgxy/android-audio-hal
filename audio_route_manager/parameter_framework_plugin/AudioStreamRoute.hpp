/*
 * INTEL CONFIDENTIAL
 *
 * Copyright (c) 2013-2014 Intel Corporation All Rights Reserved.
 *
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors.
 *
 * Title to the Material remains with Intel Corporation or its suppliers and
 * licensors. The Material contains trade secrets and proprietary and
 * confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and treaty
 * provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or disclosed
 * in any way without Intel's prior express written permission.
 *
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 */

#pragma once

#include "FormattedSubsystemObject.h"
#include "InstanceConfigurableElement.h"
#include "MappingContext.h"
#include <RouteInterface.hpp>

class RouteSubsystem;

class AudioStreamRoute : public CFormattedSubsystemObject
{
private:
    static const uint32_t mMaxStringSize = 257; /**< max string size (plus zero terminal). */

    /**
     * Mapped control structure - must be packed
     */
    struct Config
    {
        bool requirePreEnable; /**< require pre enable attribute. */
        bool requirePostDisable; /**< require post disable attribute. */
        uint32_t silencePrologInMs; /**< prolog silence to be added when opening the device in ms.*/
        uint32_t channel; /**< pcm channels supported. */
        char channelsPolicy[mMaxStringSize]; /**< channel policy supported. */
        uint32_t rate; /**< pcm rate supported. */
        uint8_t format; /**< pcm format supported. */
        uint32_t periodSize; /**< period size. */
        uint32_t periodCount; /**< period count. */
        uint32_t startThreshold; /**< start threshold. */
        uint32_t stopThreshold; /**< stop threshold. */
        uint32_t silenceThreshold; /**< silence threshold. */
        uint32_t availMin; /** avail min. */
        uint32_t applicabilityMask; /**< applicability mask, either input source or output flags. */
        char effectSupported[mMaxStringSize]; /**< effects supported by the stream route. */
    } __attribute__((packed));

public:
    AudioStreamRoute(const string &mappingValue,
                     CInstanceConfigurableElement *instanceConfigurableElement,
                     const CMappingContext &context);

protected:
    /**
     * Sync from HW.
     * From CSubsystemObject
     *
     * @param[out] error: if return code is false, it contains the description
     *                     of the error, empty string otherwise.
     *
     * @return true if success, false otherwise.
     */
    virtual bool receiveFromHW(string &error);

    /**
     * Sync to HW.
     * From CSubsystemObject
     *
     * @param[out] error: if return code is false, it contains the description
     *                     of the error, empty string otherwise.
     *
     * @return true if success, false otherwise.
     */
    virtual bool sendToHW(string &error);

private:
    /**
     * Parse a concatenated list of channel policy separated by a coma.
     *
     * @param[in] channelPolicy string of concatenated channel policy.
     *
     * @return vector of channel policy.
     */
    std::vector<intel_audio::SampleSpec::ChannelsPolicy>
    parseChannelPolicyString(const std::string &channelPolicy);

    const RouteSubsystem *mRouteSubsystem; /**< Route subsytem plugin. */
    intel_audio::IRouteInterface *mRouteInterface; /**< Interface to communicate with Route Mgr. */

    static const Config mDefaultConfig; /**< default route stream configuration at construction. */
    Config _config; /**< stream route configuration. */

    string mRouteName; /**< stream route name. */
    uint32_t mRouteId;   /**< stream route Id. */
    string mCardName; /**< card name used by the stream route. */
    int32_t mDevice; /**< audio device used by the stream route. */
    bool mIsOut; /**< direction qualifier of the stream route. */
    bool mIsStreamRoute; /**< qualifier of the stream route. */

    static const std::string mOutputDirection; /**< string key to identify output routes. */
    static const std::string mStreamType; /**< key to identify stream route. */
    static const uint32_t mSinglePort = 1;  /**< only one port is mentionned for this route. */
    static const uint32_t mDualPorts = 2; /**< both port are mentionnent for this route. */
    static const std::string mPortDelimiter; /**< Delimiter to parse a list of ports. */
    static const std::string mStringDelimiter; /**< Delimiter to parse strings. */
    static const std::string mChannelPolicyCopy; /**< copy channel policy tag. */
    static const std::string mChannelPolicyIgnore; /**< ignore channel policy tag. */
    static const std::string mChannelPolicyAverage; /**< average channel policy tag. */
};
