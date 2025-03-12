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

#ifdef _WIN32
#include <chrono>
#include <thread>
#endif

#include "scopehal.h"
#include "OwonOscilloscope.h"
#include "EdgeTrigger.h"

using namespace std;

OwonOscilloscope::OwonOscilloscope(SCPITransport* transport)
	: SCPIDevice(transport), SCPIInstrument(transport), RemoteBridgeOscilloscope(transport)
{
}

OwonOscilloscope::~OwonOscilloscope()
{
}

unsigned int OwonOscilloscope::GetInstrumentTypes() const
{
	return Instrument::INST_OSCILLOSCOPE;
}

uint32_t OwonOscilloscope::GetInstrumentTypesForChannel(size_t) const
{
	return Instrument::INST_OSCILLOSCOPE;
}
void OwonOscilloscope::FlushConfigCache()
{
}
bool OwonOscilloscope::IsChannelEnabled(size_t i)
{
	if(i == m_extTrigChannel->GetIndex())
		return false;

	lock_guard<recursive_mutex> lock(m_cacheMutex);
	return m_channelsEnabled[i];
}

void OwonOscilloscope::EnableChannel(size_t i)
{
	RemoteBridgeOscilloscope::EnableChannel(i);
}

void OwonOscilloscope::DisableChannel(size_t i)
{
	RemoteBridgeOscilloscope::DisableChannel(i);
}

vector<OscilloscopeChannel::CouplingType> OwonOscilloscope::GetAvailableCouplings(size_t)
{
	vector<OscilloscopeChannel::CouplingType> ret;
	ret.push_back(OscilloscopeChannel::COUPLE_DC_1M);
	ret.push_back(OscilloscopeChannel::COUPLE_AC_1M);

	return ret;
}

double OwonOscilloscope::GetChannelAttenuation(size_t)
{
	// TODO
	return 1.0;
}

void OwonOscilloscope::SetChannelAttenuation(size_t, double)
{
	// TODO
}

unsigned int OwonOscilloscope::GetChannelBandwidthLimit(size_t)
{
	return 0;
}

void OwonOscilloscope::SetChannelBandwidthLimit(size_t, unsigned int)
{
}

OscilloscopeChannel* OwonOscilloscope::GetExternalTrigger()
{
	// TODO
	return NULL;
}

bool OwonOscilloscope::CanEnableChannel(size_t)
{
	// TODO: There may be some limitations on higher end models
	return true;
}

Oscilloscope::TriggerMode OwonOscilloscope::PollTrigger()
{
	// TODO
	return TRIGGER_MODE_TRIGGERED;
}

bool OwonOscilloscope::AcquireData()
{
	// TODO
	return false;
}

bool OwonOscilloscope::IsTriggerArmed()
{
	return m_triggerArmed;
}
void OwonOscilloscope::PushTrigger()
{
	auto et = dynamic_cast<EdgeTrigger*>(m_trigger);
	if(et)
		PushEdgeTrigger(et);

	else
		LogWarning("Unknown trigger type (not an edge)\n");

	ClearPendingWaveforms();
}

bool OwonOscilloscope::CanInterleave()
{
	return false;
}
std::vector<uint64_t> OwonOscilloscope::GetSampleRatesNonInterleaved()
{
	vector<uint64_t> ret;

	string rates;
	{
		lock_guard<recursive_mutex> lock(m_mutex);
		m_transport->SendCommand("RATES?");
		rates = m_transport->ReadReply();
	}

	size_t i=0;
	while(true)
	{
		size_t istart = i;
		i = rates.find(',', i+1);
		if(i == string::npos)
			break;

		auto block = rates.substr(istart, i-istart);
		uint64_t fs = stoull(block);
		auto hz = FS_PER_SECOND / fs;
		ret.push_back(hz);

		//skip the comma
		i++;
	}

	return ret;
}

vector<uint64_t> OwonOscilloscope::GetSampleRatesInterleaved()
{
	vector<uint64_t> ret = {};
	return ret;
}

std::set<Oscilloscope::InterleaveConflict> OwonOscilloscope::GetInterleaveConflicts()
{
	set<Oscilloscope::InterleaveConflict> ret;
	return ret;
}

std::vector<uint64_t> OwonOscilloscope::GetSampleDepthsNonInterleaved()
{
	vector<uint64_t> ret;

	string depths;
	{
		lock_guard<recursive_mutex> lock(m_mutex);
		m_transport->SendCommand("DEPTHS?");
		depths = m_transport->ReadReply();
	}

	size_t i = 0;
	while(true)
	{
		size_t istart = i;
		i = depths.find(',', i + 1);
		if(i == string::npos)
			break;

		uint64_t sampleDepth = stoull(depths.substr(istart, i - istart));
		ret.push_back(sampleDepth);

		//skip the comma
		i++;
	}

	return ret;
}
std::vector<uint64_t> OwonOscilloscope::GetSampleDepthsInterleaved()
{
	vector<uint64_t> ret;
	return ret;
}

uint64_t OwonOscilloscope::GetSampleRate()
{
	return m_srate;
}
uint64_t OwonOscilloscope::GetSampleDepth()
{
	return m_mdepth;
}
void OwonOscilloscope::SetSampleDepth(uint64_t depth)
{
	lock_guard<recursive_mutex> lock(m_mutex);
	//m_transport->SendCommand(string("DEPTH ") + to_string(depth));
	m_mdepth = depth;
}
void OwonOscilloscope::SetSampleRate(uint64_t rate)
{
	m_srate = rate;

	lock_guard<recursive_mutex> lock(m_mutex);
	m_transport->SendCommand( string("RATE ") + to_string(rate));
}

void OwonOscilloscope::SetTriggerOffset(int64_t offset)
{
	lock_guard<recursive_mutex> lock(m_mutex);

	//Don't allow setting trigger offset beyond the end of the capture
	int64_t captureDuration = GetSampleDepth() * FS_PER_SECOND / GetSampleRate();
	m_triggerOffset = min(offset, captureDuration);

	PushTrigger();
}
int64_t OwonOscilloscope::GetTriggerOffset()
{
	return m_triggerOffset;
}

bool OwonOscilloscope::IsInterleaving()
{
	return false;
}

bool OwonOscilloscope::SetInterleaving(bool)
{
	return false;
}

std::vector<Oscilloscope::AnalogBank> OwonOscilloscope::GetAnalogBanks()
{
	vector<AnalogBank> banks;
	banks.push_back(GetAnalogBank(0));
	return banks;
}

Oscilloscope::AnalogBank OwonOscilloscope::GetAnalogBank(size_t)
{
	AnalogBank bank;
	return bank;
}

bool OwonOscilloscope::IsADCModeConfigurable()
{
	return false;
}

std::vector<std::string> OwonOscilloscope::GetADCModeNames(size_t)
{
	vector<string> ret;
	ret.push_back("8 Bit");

	return ret;
}
size_t OwonOscilloscope::GetADCMode(size_t)
{
	return 0;
}

void OwonOscilloscope::SetADCMode(size_t, size_t)
{
}
std::string OwonOscilloscope::GetDriverNameInternal()
{
	return "owon_vds";
}