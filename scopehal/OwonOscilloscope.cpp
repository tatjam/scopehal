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

uint32_t OwonOscilloscope::GetInstrumentTypesForChannel(size_t i) const
{
	return Instrument::INST_OSCILLOSCOPE;
}

vector<OscilloscopeChannel::CouplingType> OwonOscilloscope::GetAvailableCouplings(size_t i)
{
	vector<OscilloscopeChannel::CouplingType> ret;
	ret.push_back(OscilloscopeChannel::COUPLE_DC_1M);
	ret.push_back(OscilloscopeChannel::COUPLE_AC_1M);

	return ret;
}

unsigned int OwonOscilloscope::GetChannelBandwidthLimit(size_t i)
{
	return 0;
}

void OwonOscilloscope::SetChannelBandwidthLimit(size_t i, unsigned int limit_mhz)
{
}

bool OwonOscilloscope::CanInterleave()
{
	return false;
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

bool OwonOscilloscope::IsInterleaving()
{
	return false;
}

bool OwonOscilloscope::SetInterleaving(bool combine)
{
	return false;
}

std::vector<Oscilloscope::AnalogBank> OwonOscilloscope::GetAnalogBanks()
{
	vector<AnalogBank> banks;
	banks.push_back(GetAnalogBank(0));
	return banks;
}

Oscilloscope::AnalogBank OwonOscilloscope::GetAnalogBank(size_t channel)
{
	AnalogBank bank;
	return bank;
}

bool OwonOscilloscope::IsADCModeConfigurable()
{
	return false;
}

std::vector<std::string> OwonOscilloscope::GetADCModeNames(size_t channel)
{
	vector<string> ret;
	ret.push_back("8 Bit");

	return ret;
}
size_t OwonOscilloscope::GetADCMode(size_t channel)
{
	return 0;
}

void OwonOscilloscope::SetADCMode(size_t channel, size_t mode)
{
}