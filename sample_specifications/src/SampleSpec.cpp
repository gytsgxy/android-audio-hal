/*
 * Copyright (C) 2013-2017 Intel Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#define LOG_TAG "SampleSpec"

#include "SampleSpec.hpp"
#include <typeconverter/TypeConverter.hpp>
#include <AudioCommsAssert.hpp>
#include <utilities/Log.hpp>
#include <stdint.h>
#include <errno.h>
#include <limits>
#include <utils/String8.h>

using audio_comms::utilities::Log;
using namespace std;

namespace intel_audio
{

#define SAMPLE_SPEC_ITEM_IS_VALID(sampleSpecItem)                                    \
    AUDIOCOMMS_ASSERT((sampleSpecItem) >= 0 && (sampleSpecItem) < NbSampleSpecItems, \
                      "Invalid Sample Specifications")

SampleSpec::SampleSpec(uint32_t channel,
                       uint32_t format,
                       uint32_t rate,
                       const vector<ChannelsPolicy> &channelsPolicy)
{
    mChannelMask = 0;
    setSampleSpecItem(ChannelCountSampleSpecItem, channel);
    setSampleSpecItem(FormatSampleSpecItem, format);
    setSampleSpecItem(RateSampleSpecItem, rate);
    if (not channelsPolicy.empty()) {
        setChannelsPolicy(channelsPolicy);
    }
}

// Generic Accessor
void SampleSpec::setSampleSpecItem(SampleSpecItem sampleSpecItem, uint32_t value)
{
    SAMPLE_SPEC_ITEM_IS_VALID(sampleSpecItem);

    if (sampleSpecItem == ChannelCountSampleSpecItem) {

        AUDIOCOMMS_ASSERT(value < mMaxChannels, "Max channel number reached");

        // Reset all the channels policy to copy by default
        mChannelsPolicy.clear();
        mChannelsPolicy = std::vector<ChannelsPolicy>(value, Copy);
    }
    mSampleSpec[sampleSpecItem] = value;
}

void SampleSpec::setChannelsPolicy(const vector<ChannelsPolicy> &channelsPolicy)
{
    if (channelsPolicy.size() != mSampleSpec[ChannelCountSampleSpecItem]) {
        Log::Warning() << __FUNCTION__ << ": Cannot set requested channel policy";
        return;
    }
    mChannelsPolicy = channelsPolicy;
}

SampleSpec::ChannelsPolicy SampleSpec::getChannelsPolicy(uint32_t channelIndex) const
{
    AUDIOCOMMS_ASSERT(channelIndex < mChannelsPolicy.size(),
                      "request of channel policy outside channel numbers");
    return mChannelsPolicy[channelIndex];
}

uint32_t SampleSpec::getSampleSpecItem(SampleSpecItem sampleSpecItem) const
{
    SAMPLE_SPEC_ITEM_IS_VALID(sampleSpecItem);
    return mSampleSpec[sampleSpecItem];
}

size_t SampleSpec::getFrameSize() const
{
    return audio_bytes_per_sample(getFormat()) * getChannelCount();
}

size_t SampleSpec::convertBytesToFrames(size_t bytes) const
{
    if (getFrameSize() == 0) {
        Log::Error() << __FUNCTION__ << ": Null frame size";
        return 0;
    }
    return bytes / getFrameSize();
}

size_t SampleSpec::convertFramesToBytes(size_t frames) const
{
    if (getFrameSize() == 0) {
        Log::Error() << __FUNCTION__ << ": Null frame size";
        return 0;
    }
    AUDIOCOMMS_ASSERT(frames <= numeric_limits<size_t>::max() / getFrameSize(),
                      "conversion exceeds limit");
    return frames * getFrameSize();
}

size_t SampleSpec::convertFramesToUsec(uint32_t frames) const
{
    if (getSampleRate() == 0) {
        Log::Error() << __FUNCTION__ << ": Null sample rate";
        return 0;
    }
    AUDIOCOMMS_ASSERT((static_cast<uint64_t>(frames) / getSampleRate()) <=
                      (numeric_limits<size_t>::max() / mUsecPerSec),
                      "conversion exceeds limit");
    return (mUsecPerSec * static_cast<uint64_t>(frames)) / getSampleRate();
}

size_t SampleSpec::convertUsecToframes(uint32_t intervalUsec) const
{
    return static_cast<uint64_t>(intervalUsec) * getSampleRate() / mUsecPerSec;
}

bool SampleSpec::isSampleSpecItemEqual(SampleSpecItem sampleSpecItem,
                                       const SampleSpec &ssSrc,
                                       const SampleSpec &ssDst)
{
    if (ssSrc.getSampleSpecItem(sampleSpecItem) != ssDst.getSampleSpecItem(sampleSpecItem)) {

        return false;
    }

    return (sampleSpecItem != ChannelCountSampleSpecItem) ||
           ssSrc.getChannelsPolicy() == ssDst.getChannelsPolicy();
}

android::status_t SampleSpec::dump(const int fd, bool isOut, int spaces) const
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    android::String8 result;
    snprintf(buffer, SIZE, "%*s- rate %d\n", spaces, "", getSampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "%*s- channels %d\n", spaces, "", getChannelCount());
    result.append(buffer);
    snprintf(buffer, SIZE, "%*s- channel mask %s\n", spaces, "",
             isOut ? OutputChannelConverter::toString(mChannelMask).c_str() :
             InputChannelConverter::toString(mChannelMask).c_str());
    result.append(buffer);
    snprintf(buffer, SIZE, "%*s- format %s\n", spaces, "", FormatConverter::toString(
                 getFormat()).c_str());
    result.append(buffer);

    write(fd, result.string(), result.size());
    return android::OK;
}

}  // namespace intel_audio
