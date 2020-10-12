#include "mu2e-artdaq-core/Overlays/TrackerFragment.hh"

#include <vector>
#include "TrackerFragment.hh"

namespace mu2e {
TrackerFragment::TrackerFragment(artdaq::Fragment const& f)
	: ArtFragment(f)
{
	if (block_count() > 0)
	{
		auto dataPtr = dataAtBlockIndex(0);
		auto hdr = dataPtr->GetHeader();
		if (hdr.GetSubsystem() != DTCLib::DTC_Subsystem_Tracker || hdr.GetVersion() > 1)
		{
			TLOG(TLVL_ERROR) << "TrackerFragment CONSTRUCTOR: First block has unsupported type/version " << hdr.GetSubsystem() << "/" << hdr.GetVersion();
		}
	}
}

std::unique_ptr<TrackerFragment::TrackerDataPacket> TrackerFragment::GetTrackerData(size_t blockIndex) const
{
	auto dataPtr = dataAtBlockIndex(blockIndex);
	if (dataPtr == nullptr) return nullptr;

	std::unique_ptr<TrackerDataPacket> output(nullptr);
	switch (dataPtr->GetHeader().GetVersion())
	{
		case 0: {
			auto trackerPacket = reinterpret_cast<TrackerDataPacketV0 const*>(dataPtr->GetData());
			output = Upgrade(trackerPacket);
		}
		break;
		case 1:
			output.reset(new TrackerDataPacket(*reinterpret_cast<TrackerDataPacket const*>(dataPtr->GetData())));
			break;
	}

	return output;
}

std::vector<uint16_t> TrackerFragment::GetWaveform(size_t blockIndex) const
{
	std::vector<uint16_t> output;
	auto dataPtr = dataAtBlockIndex(blockIndex);
	if (dataPtr == nullptr) return output;

	switch (dataPtr->GetHeader().GetVersion())
	{
		case 0: {
			auto trackerPacket = reinterpret_cast<TrackerDataPacketV0 const*>(dataPtr->GetData());
			output.push_back(trackerPacket->ADC00);
			output.push_back(trackerPacket->ADC01());
			output.push_back(trackerPacket->ADC02());
			output.push_back(trackerPacket->ADC03);
			output.push_back(trackerPacket->ADC04);
			output.push_back(trackerPacket->ADC05());
			output.push_back(trackerPacket->ADC06());
			output.push_back(trackerPacket->ADC07);
			output.push_back(trackerPacket->ADC08);
			output.push_back(trackerPacket->ADC09());
			output.push_back(trackerPacket->ADC10());
			output.push_back(trackerPacket->ADC11);
			output.push_back(trackerPacket->ADC12);
			output.push_back(trackerPacket->ADC13());
			output.push_back(trackerPacket->ADC14());
		}
		break;
		case 1: {
			auto trackerHeaderPacket = reinterpret_cast<TrackerDataPacket const*>(dataPtr->GetData());
			output.push_back(trackerHeaderPacket->ADC00);
			output.push_back(trackerHeaderPacket->ADC01());
			output.push_back(trackerHeaderPacket->ADC02);

			auto adcs = trackerHeaderPacket->NumADCPackets;
			auto adcsProcessed = 0;
			auto trackerADCPacket = reinterpret_cast<TrackerADCPacket const*>(trackerHeaderPacket + 1);
			while (adcsProcessed < adcs)
			{
				output.push_back(trackerADCPacket->ADC0);
				output.push_back(trackerADCPacket->ADC1());
				output.push_back(trackerADCPacket->ADC2);
				output.push_back(trackerADCPacket->ADC3);
				output.push_back(trackerADCPacket->ADC4());
				output.push_back(trackerADCPacket->ADC5);
				output.push_back(trackerADCPacket->ADC6);
				output.push_back(trackerADCPacket->ADC7());
				output.push_back(trackerADCPacket->ADC8);
				output.push_back(trackerADCPacket->ADC9);
				output.push_back(trackerADCPacket->ADC10());
				output.push_back(trackerADCPacket->ADC11);

				adcsProcessed++;
				if (adcsProcessed < adcs)
					trackerADCPacket += 1;  // Go to the next packet, assuming it's a TrackerADCPacket
			}
		}
		break;
	}

	return output;
}

std::unique_ptr<TrackerFragment::TrackerDataPacket> TrackerFragment::Upgrade(const TrackerFragment::TrackerDataPacketV0* input)
{
	if (input == nullptr) return nullptr;
	auto output = std::make_unique<TrackerDataPacket>();
	output->StrawIndex = input->StrawIndex;

	output->TDC0A = input->TDC0;

	output->TDC0B = 0;
	output->TOT0 = input->TOT0 & 0xF;
	output->EWMCounter = 0;

	output->TDC1A = input->TDC1;

	output->TDC1B = 0;
	output->TOT1 = input->TOT1 & 0xF;
	output->ErrorFlags = input->PreprocessingFlags & 0xF;  // Note that we're dropping 4 bits here

	output->NumADCPackets = 1;
	output->PMP = 0;

	return output;
}
}  // namespace mu2e
