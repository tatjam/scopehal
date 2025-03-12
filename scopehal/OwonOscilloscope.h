/***********************************************************************************************************************
*                                                                                                                      *
* libscopehal v0.1                                                                                                     *
*                                                                                                                      *
* Copyright (c) 2012-2023 Andrew D. Zonenberg and contributors                                                         *
* All rights reserved.                                                                                                 *
*                                                                                                                      *
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the     *
* following conditions are met:                                                                                        *
*                                                                                                                      *
*    * Redistributions of source code must retain the above copyright notice, this list of conditions, and the         *
*      following disclaimer.                                                                                           *
*                                                                                                                      *
*    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the       *
*      following disclaimer in the documentation and/or other materials provided with the distribution.                *
*                                                                                                                      *
*    * Neither the name of the author nor the names of any contributors may be used to endorse or promote products     *
*      derived from this software without specific prior written permission.                                           *
*                                                                                                                      *
* THIS SOFTWARE IS PROVIDED BY THE AUTHORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED   *
* TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL *
* THE AUTHORS BE HELD LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES        *
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR       *
* BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT *
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE       *
* POSSIBILITY OF SUCH DAMAGE.                                                                                          *
*                                                                                                                      *
***********************************************************************************************************************/

#ifndef OwonOscilloscope_h
#define OwonOscilloscope_h

class EdgeTrigger;

#include "RemoteBridgeOscilloscope.h"

/**
	@brief OwonOscilloscope - driver for talking to the scopehal-pico-bridge daemons
 */
class OwonOscilloscope 	: public virtual RemoteBridgeOscilloscope
{
public:
	explicit OwonOscilloscope(SCPITransport* transport);
	~OwonOscilloscope() override;

	//not copyable or assignable
	OwonOscilloscope(const OwonOscilloscope& rhs) =delete;
	OwonOscilloscope& operator=(const OwonOscilloscope& rhs) =delete;

public:

	//Device information
	virtual unsigned int GetInstrumentTypes() const override;
	virtual uint32_t GetInstrumentTypesForChannel(size_t i) const override;

	virtual void FlushConfigCache() override;

	//Channel configuration
	virtual bool IsChannelEnabled(size_t i) override;
	virtual void EnableChannel(size_t i) override;
	virtual void DisableChannel(size_t i) override;
	virtual std::vector<OscilloscopeChannel::CouplingType> GetAvailableCouplings(size_t i) override;
	virtual double GetChannelAttenuation(size_t i) override;
	virtual void SetChannelAttenuation(size_t i, double atten) override;
	virtual unsigned int GetChannelBandwidthLimit(size_t i) override;
	virtual void SetChannelBandwidthLimit(size_t i, unsigned int limit_mhz) override;
	virtual OscilloscopeChannel* GetExternalTrigger() override;
	virtual bool CanEnableChannel(size_t i) override;

	//Triggering
	virtual Oscilloscope::TriggerMode PollTrigger() override;
	virtual bool AcquireData() override;
	virtual bool IsTriggerArmed() override;
	virtual void PushTrigger() override;

	//Timebase
	virtual bool CanInterleave() override;
	virtual std::vector<uint64_t> GetSampleRatesNonInterleaved() override;
	virtual std::vector<uint64_t> GetSampleRatesInterleaved() override;
	virtual std::set<InterleaveConflict> GetInterleaveConflicts() override;
	virtual std::vector<uint64_t> GetSampleDepthsNonInterleaved() override;
	virtual std::vector<uint64_t> GetSampleDepthsInterleaved() override;
	virtual uint64_t GetSampleRate() override;
	virtual uint64_t GetSampleDepth() override;
	virtual void SetSampleDepth(uint64_t depth) override;
	virtual void SetSampleRate(uint64_t rate) override;
	virtual void SetTriggerOffset(int64_t offset) override;
	virtual int64_t GetTriggerOffset() override;
	virtual bool IsInterleaving() override;
	virtual bool SetInterleaving(bool combine) override;

	//ADC configuration
	virtual std::vector<AnalogBank> GetAnalogBanks() override;
	virtual AnalogBank GetAnalogBank(size_t channel) override;
	virtual bool IsADCModeConfigurable() override;
	virtual std::vector<std::string> GetADCModeNames(size_t channel) override;
	virtual size_t GetADCMode(size_t channel) override;
	virtual void SetADCMode(size_t channel, size_t mode) override;

	enum Series
	{
		SERIES_VDS1022,
		SERIES_VDS2062, // TODO
		SERIES_VDS2064, // TODO
		SERIES_VDS3102, // TODO
		SERIES_VDS3104, // TODO
		SERIES_UNKNOWN	//unknown or invalid model name
	};

protected:
	std::string GetChannelColor(size_t i);

	//hardware analog channel count
	size_t m_analogChannelCount;

	OscilloscopeChannel* m_extTrigChannel;

	Series m_series;

	///@brief Buffers for storing raw ADC samples before converting to fp32
	std::vector<std::unique_ptr<AcceleratorBuffer<int16_t> > > m_analogRawWaveformBuffers;

	//Vulkan waveform conversion
	std::shared_ptr<QueueHandle> m_queue;
	std::unique_ptr<vk::raii::CommandPool> m_pool;
	std::unique_ptr<vk::raii::CommandBuffer> m_cmdBuf;
	std::unique_ptr<ComputePipeline> m_conversionPipeline;

public:

	static std::string GetDriverNameInternal();
	OSCILLOSCOPE_INITPROC(OwonOscilloscope)
};

#endif
