/*
 * Copyright (C) 2014-2015 Intel Corporation
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

#include <gtest/gtest.h>
#include <system/audio.h>
#include <hardware/audio.h>
#include <hardware/hardware.h>
#include <utils/Errors.h>
#include <string>

struct audio_module;

class AudioHalTest : public ::testing::Test
{
public:
    static void SetUpTestCase();

    static void TearDownTestCase();

    virtual void SetUp();

    virtual void TearDown();

    static audio_hw_device_t *getDevice()
    {
        return mDevice;
    }

    static audio_module *getAudioModule()
    {
        return mAudioModule;
    }

    void setConfig(uint32_t rate, audio_channel_mask_t mask, audio_format_t format,
                   audio_config_t &config);

    virtual ~AudioHalTest() {}

    void printHardwareModuleInformation(hw_module_t *module);

private:
    static audio_hw_device_t *openAudioDevice();

    static android::status_t closeAudioDevice(audio_hw_device_t *audio_dev);

    static audio_module *mAudioModule; /**< HW Module handle. */
    static audio_hw_device_t *mDevice; /**< HW Device handle. */
};

class AudioHalAndroidModeTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<audio_mode_t>
{
};

class AudioHalMicMuteTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<bool>
{
};

class AudioHalMasterMuteTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<bool>
{
};

class AudioHalMasterVolumeTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<float>
{
};

class AudioHalValidInputDeviceTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<audio_devices_t>
{
};

class AudioHalInvalidInputDeviceTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<audio_devices_t>
{
};


class AudioHalValidOutputDevicesTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<audio_devices_t>
{
};

class AudioHalInvalidOutputDevicesTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<audio_devices_t>
{
};


class AudioHalOutputStreamSupportedRateTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<std::pair<uint32_t, bool> >
{
};

class AudioHalOutputStreamSupportedFormatTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<std::pair<audio_format_t, bool> >
{
};

class AudioHalOutputStreamSupportedChannelsTest
    : public AudioHalTest,
      public ::testing::WithParamInterface<std::pair<audio_channel_mask_t, bool> >
{
};

class AudioHalValidGlobalParameterTest
    : public ::testing::WithParamInterface<std::pair<std::string, std::string> >,
      public AudioHalTest
{
};

class AudioHalInvalidGlobalParameterTest
    : public ::testing::WithParamInterface<std::pair<std::string, std::string> >,
      public AudioHalTest
{
};
