#include "Chart_O2Jam.hpp"
#include "Music.hpp"
#include "Models/NoteInstanceAlgorithm.hpp" // zip

namespace O2Jam {

//!~ OJN Music Parser
Music* openOJN (const std::string& path)
{
	clan::File file;
	if (file.open(path,
				clan::File::OpenMode::open_existing,
				clan::File::AccessFlags::access_read
				) == false)
		return nullptr;

	uint8_t* buffer = new uint8_t[300];
	int read = file.read(buffer, 300);
	if (read != 300) {
		delete[] buffer;
		return nullptr;
	}

	OJN_Header *pHead = (OJN_Header*)buffer;

	// parse header
	Music* music = new Music;
	music->path     = path;
	music->title    = pHead->Title;
	music->artist   = pHead->Artist;
	music->genre    = pHead->oldGenre;

	switch(pHead->newGenreCode) {
		case 0 : music->genre = "Ballad"; break;
		case 1 : music->genre = "Rock"; break;
		case 2 : music->genre = "Dance"; break;
		case 3 : music->genre = "Techno"; break;
		case 4 : music->genre = "Hip-hop"; break;
		case 5 : music->genre = "Soul/R&B"; break;
		case 6 : music->genre = "Jazz"; break;
		case 7 : music->genre = "Funk"; break;
		case 8 : music->genre = "Traditional"; break;
		case 9 : music->genre = "Classical"; break;
		case 10: music->genre = "Others"; break;
	};

	music->charts[0] = std::make_shared<O2JamChart>(path, *pHead, 0);
	music->charts[1] = std::make_shared<O2JamChart>(path, *pHead, 1);
	music->charts[2] = std::make_shared<O2JamChart>(path, *pHead, 2);

	delete[] buffer;
	return music;
} // end openOJN

O2JamChart::O2JamChart(const std::string &path, const OJN_Header &header, uint8_t index) :
	ojn_path   (path),
	ojn_header (header),
	chart_index(index)
{
	if (index > 2)
		throw std::invalid_argument("Invalid chart index.");

	mInfo = ChartInfo {
		.name       = (index == 2 ? "HX" : (index == 1 ? "NX" : "EX")),
			.charter    = header.Charter,
			.level      = header.Level[index],
			.cc_count   = header.numEvents[index],
			.ne_count   = header.numNotes[index],
			.duration   = header.Duration[index],
			.tempo      = header.Tempo
	};

	ojm_path = clan::PathHelp::get_basepath(path) + header.OJMFile;
}



void O2JamChart::load_cover_art()
{
	if (ojn_header.newCoverArtSize == 0)
		return;

	clan::File file;
	if (file.open(ojn_path,
				clan::File::OpenMode::open_existing,
				clan::File::AccessFlags::access_read
				) == false)
		return;

	file.seek(ojn_header.DataOffset[3], clan::File::seek_set);

	uint8_t* buffer = new uint8_t[ojn_header.newCoverArtSize];

	int read = file.read(buffer, ojn_header.newCoverArtSize);

	if (read == static_cast<int>(ojn_header.newCoverArtSize)) {
		try
		{
			clan::DataBuffer        dbuff ( buffer, read );
			clan::IODevice_Memory   memio ( dbuff );

			mCover = clan::PixelBuffer{ memio, "jpg", false };
		} catch (clan::Exception &e) {
			clan::Console::write_line("Failed to load .jpg cover art for %1.", ojn_header.Title);
			clan::Console::write_line(e.get_message_and_stack_trace());
		}
	}

	delete[] buffer;
}

void O2JamChart::load_chart ()
{
	// this->clear();

	TTime       time;

	// LONG note lists
	Measure::NSs HNL, RNL;

	// Counters : Params, Notes, SingleNotes, LongNotes, HoldNotes, ReleaseNotes, PlayNotes, AutoNotes.
	size_t cP = 0, cN = 0, cSN = 0, cLN = 0, cHN = 0, cRN = 0, cPN = 0, cAN = 0;


	for (unsigned n = 0; n <= ojn_header.numMeasures[chart_index]; n++)
		mSequence->emplace_back(TSignature{ 4, 48 });

	clan::File file;

	if( file.open(ojn_path, clan::File::open_existing, clan::File::access_read) == false )
		return;

	int chartSize = file.get_size() - ojn_header.DataOffset[chart_index];

	file.seek(ojn_header.DataOffset[chart_index], clan::File::seek_set);

	uint8_t* buffer = new uint8_t[chartSize];
	int read = file.read(buffer, chartSize);
	if (read != chartSize)
		throw std::runtime_error("Malformed OJN file.");

	const uint8_t* pPtr = buffer;

	// OJN Note Set parse loop
	for (unsigned j = 0; j < ojn_header.numNoteSets[chart_index]; j++)
	{
		OJN_NoteSet_Header* pNoteSet = (OJN_NoteSet_Header*)pPtr;
		pPtr += sizeof(OJN_NoteSet_Header);

		uint32_t iMeasure  = pNoteSet->Measure;
		uint16_t iChannel  = pNoteSet->Channel;
		uint16_t numEvents = pNoteSet->numEvents;

		Measure& pMeasure = mSequence->at(iMeasure);

		ENoteKey nChannel = ENoteKey::AUTO;

		switch (iChannel)
		{
			// Time Signature changes
			case 0:
				pMeasure.setSignature(getTimeSignature(*((float*)pPtr)));

				pPtr += (4 * numEvents);
				break;
				// Tempo changes
			case 1:
				for (unsigned k = 0; k < numEvents; k++)
				{
					if (*((float*)pPtr) != 0.0f)
					{
						uint16_t Tick = k * 192 / numEvents;
						time = TTime(Tick % 48, Tick / 48, iMeasure);

						pMeasure.mCCs.emplace_back(time, EControl::CLOCK_TEMPO, *((float*)pPtr));
						cP++;
					}

					pPtr += 4;
				}
				break;

				// Music Notes
			default:
				switch (iChannel) {
					case 2: nChannel = ENoteKey::P1_1; break;
					case 3: nChannel = ENoteKey::P1_2; break;
					case 4: nChannel = ENoteKey::P1_3; break;
					case 5: nChannel = ENoteKey::P1_4; break;
					case 6: nChannel = ENoteKey::P1_5; break;
					case 7: nChannel = ENoteKey::P1_6; break;
					case 8: nChannel = ENoteKey::P1_7; break;
				}

				for (uint16_t k = 0; k < numEvents; k++)
				{
					uint16_t Tick = k * 192 / numEvents;

					time = TTime(Tick % 48, Tick / 48, iMeasure);

					// read note event
					OJN_Note *pEvent = (OJN_Note*)pPtr;
					pPtr += 4;

					uint16_t SmplID = pEvent->SampleID;
					uint8_t  VolPan = pEvent->VolPan;
					uint8_t  Type   = pEvent->NoteType;

					// skip empty notes; sample IDs always start with 1
					if (SmplID != 0)
					{
						float Vol = (float)((VolPan >> 4) & 0x0F); // higher half
						float Pan = (float) (VolPan & 0x0F);       //  lower half

						Vol = (Vol == 0) ? 1.0f : (Vol / 16.0f);
						Pan = (Pan == 0) ? 0.0f : (Pan -  8.0f) / 8.0f;

						// Note Type safeguard
						if (Type >= 8) {
							fprintf(stderr, "[debug] Fixing invalid note type. \n");
							Type = Type % 8;
						}

						// M### mode
						if (Type >= 4) {
							SmplID += 1000;
							Type -= 4;
						}

						NoteAudio audio = { SmplID, 0, Vol, Pan };

						// set up event
						cN++;

						if (nChannel == ENoteKey::AUTO)
						{
							nChannel = ENoteKey_makeBG(iChannel - 9);
							// Silently remove release notes.
							if (Type == 2) {
								pMeasure.addNoteSingle(nChannel, time, audio);
								cAN++, cSN++;
								fprintf(stderr, "[debug] Parsing to Hold Note in Autoplay Channel as Normal Note. \n");
							} else if (Type == 3) {
								fprintf(stderr, "[debug] Skipping Release Note in Autoplay Channel. \n");
							} else {
								pMeasure.addNoteSingle(nChannel, time, audio);
								cAN++, cSN++;
							}
						} else if (Type == 2) { // Hold Notes
							audio.trackID = 1;
							HNL.emplace_back(nChannel, time, audio);
							cHN++;
						} else if (Type == 3) { // Release Notes
							audio. trackID = 1;
							audio.sampleID = 0; // O2Jam does not have release sound
							RNL.emplace_back(nChannel, time, audio);
							cRN++;
						} else {
							audio.trackID = 1;
							pMeasure.addNoteSingle(nChannel, time, audio);
							cAN++, cPN++;
						}

					} // end-if empty sample
				} // end-for

				break;
		} // end switch

	}

	auto L = zip_toNL(HNL, RNL);
	while(!L.first.empty())
	{
		auto it = L.first.begin();
		mSequence->at(it->t.m).addNoteSingle(*it);
		cSN++;
		L.first.erase(it);
	}
	while(!L.second.empty())
	{
		auto it = L.second.begin();
		mSequence->at(it->t.m).addNoteLong(*it);
		cSN++;
		L.second.erase(it);
	}

	sort_sequence();

	// TODO: Note count
	// this->notes = cPN;

	delete[] buffer;

}

// O2Jam's M30 XORing
// XOR sets of 4 bytes with mask. Remainder bytes are ignored.
static void decrypt_M30XOR (uint8_t *&sData, unsigned int sSize, const uint8_t *sMask)
{
	for ( unsigned int i = 0; i + 3 < sSize; i += 4 )
	{
		sData[i+0] ^= sMask[0];
		sData[i+1] ^= sMask[1];
		sData[i+2] ^= sMask[2];
		sData[i+3] ^= sMask[3];
	}
}

// O2Jam's OMC-WAV XORing
// Doesn't really XOR the data.
static void decrypt_accXOR(std::vector<uint8_t> &sData, bool reset = false)
{
	/**
	 * The key byte is preserved throughout the whole file so it needs
	 * to be defined as a static variable. It has to be reset whenever
	 * a new file is parsed.
	 */
	static uint32_t j = 0; /* byte counter */
	static uint32_t k = OMC_ACCKEY_INIT;

	uint8_t y , z; /* byte reserve */

	for ( unsigned int i = 0; i < sData.size(); i++ )
	{
		z = y = sData[i];

		if (((k << j) & 0x80) != 0)
			z = ~z;

		sData[i] = z;

		j++;
		if (j > 7) {
			j = 0;
			k = y;
		}
	}

	if (reset == true) {
		j = 0;
		k = OMC_ACCKEY_INIT;
	}
}

// O2Jam's OMC-WAV data shuffler
static void decrypt_arrange (std::vector<uint8_t> &sData)
{
	unsigned int sSize = sData.size();

	// rearrangement key
	unsigned int  k = ((sSize % 17) << 4) + (sSize % 17);

	// rearrangement block size
	unsigned int bs = sSize / 17;

	// temporary buffer
	std::vector<uint8_t> sRawData(sData.begin(), sData.end());

	for ( unsigned int b = 0; b < 17; b++ )
	{
		unsigned int se_bOffset = bs * b;                 // offset of encoded block
		unsigned int ed_bOffset = bs * c_Arrangement[k];  // offset of decoded block

		std::copy (sRawData.begin() + se_bOffset, sRawData.begin() + se_bOffset + bs, sData.begin() + ed_bOffset);
		k++;
	}
}

// type M30 parser
void parseM30 (clan::File& file, SampleMap& sample_map)
{
	const int fileSize = file.get_size();
	static const /* constexpr */ int headSize = sizeof(M30_File_Header);   // 32 - 4 (signature) = 28
	static const /* constexpr */ int M30hSize = sizeof(M30_Sample_Header); // 52 bytes

	file.seek(0);

	// create read buffer
	uint8_t* buffer = new uint8_t[headSize];
	int read = file.read(buffer, headSize);
	if (read != headSize)
		throw std::runtime_error("Malformed OJM file.");

	// read header
	M30_File_Header *pFileHeader = (M30_File_Header*)buffer;

	uint32_t smplEncryption = pFileHeader->encryption;
	uint32_t smplCount      = pFileHeader->samples;
	uint32_t smplOffset     = pFileHeader->payload_addr;
	uint32_t packSize       = pFileHeader->payload_size;

	delete[] buffer;

	if (packSize > fileSize - smplOffset) {
		fprintf(stderr, "[debug] Header reports different payload size.\n");
		packSize = fileSize - smplOffset;
	}

	// Jump to payload location
	file.seek(smplOffset);

	for (unsigned int i = 0; i < smplCount; i++)
	{
		// Read M30 sample header
		buffer = new uint8_t[M30hSize];
		read = file.read(buffer, M30hSize);
		if (read != M30hSize) {
			fprintf(stderr, "[debug] Fatal OJM file read error.\n");
			delete[] buffer;
			break;
		}

		M30_Sample_Header *pSmplHeader = (M30_Sample_Header*)buffer;

		std::string smplName = pSmplHeader->name;
		smplName.append(".ogg");

		uint32_t smplSize = pSmplHeader->size;
		uint16_t smplType = pSmplHeader->type;
		uint16_t smplID   = pSmplHeader->id+1;

		delete[] buffer;

		buffer = new uint8_t[smplSize];
		read = file.read(buffer, smplSize);
		if (read != (int)smplSize)
			delete[] buffer;

		uint8_t* pSmplData = buffer;

		// decode sample
		switch (smplEncryption) {
			// unencrypted OGG
			case  0: break;
			         // namiXOR-ed OGG
			case 16: decrypt_M30XOR(pSmplData, smplSize, M30_nami_XORMASK); break;
			         // 0412XOR-ed OGG
			case 32: decrypt_M30XOR(pSmplData, smplSize, M30_0412_XORMASK); break;
		}

		// type M### note
		if (smplType == 0)
			smplID += 1000;


		// pass into OGG stream
		Sample pSample((char*)pSmplData, smplSize, smplName.c_str());

		if (pSample.getSource() == nullptr)
			fprintf(stderr, "[warn] Failed to load M30 sample: %s\n", smplName.c_str());
		else
			sample_map[smplID] = pSample;

		delete[] buffer;
	}

}


// type OMC parser
void parseOMC (clan::File& file, bool isEncrypted, SampleMap& sample_map)
{
	// read headers
	int fileSize = file.get_size();
	static const /* constexpr */ int headSize = sizeof(OMC_File_Header);
	static const /* constexpr */ int WAVhSize = sizeof(OMC_WAV_Header);
	static const /* constexpr */ int OGGhSize = sizeof(OMC_OGG_Header);

	file.seek(0);

	// create read buffer
	uint8_t* buffer = new uint8_t[headSize];
	int read = file.read(buffer, headSize);
	if (read != headSize)
		throw std::runtime_error("Malformed OJM file.");

	// read header
	OMC_File_Header *pFileHeader = (OMC_File_Header*)buffer;

	// Sample ID counter
	uint16_t smplID;

	uint32_t WAV_Offset = pFileHeader->wavs_addr;
	uint32_t OGG_Offset = pFileHeader->oggs_addr;
	uint32_t WAV_PackSize;
	uint32_t OGG_PackSize;

	// Calculate sound archive size. Usually the WAV archive comes first,
	// but just to be sure, we test the offsets.
	if (OGG_Offset > WAV_Offset) {          // WAVs first
		WAV_PackSize = OGG_Offset - WAV_Offset;
		OGG_PackSize =   fileSize - OGG_Offset;
	} else if (OGG_Offset < WAV_Offset){    // OGGs first
		WAV_PackSize =   fileSize - WAV_Offset;
		OGG_PackSize = WAV_Offset - OGG_Offset;
	} else {                                // no WAVs
		WAV_PackSize = 0;
		OGG_PackSize = fileSize - OGG_Offset;
	}

	// clear buffer
	delete[] buffer;


	if (WAV_PackSize > 0)
	{
		// parse WAV files
		// create new read buffer from offset
		file.seek(WAV_Offset);

		buffer = new uint8_t[WAV_PackSize];
		read = file.read(buffer, WAV_PackSize);
		if (read != (int)WAV_PackSize) {
			fprintf(stderr, "[fatal] Fatal OJM file read error.\n");
			delete[] buffer;
		}

		uint8_t* pPtr = buffer;

		smplID = 0; // WAV

		unsigned long i = 0;

		while (i < WAV_PackSize)
		{
			// read WAV header
			OMC_WAV_Header *pWAVHeader = (OMC_WAV_Header*)pPtr;
			pPtr += WAVhSize, i += WAVhSize;
			smplID++;

			std::string SampleName(pWAVHeader->name);
			SampleName.append(".wav");

			// Of all the things, why does it have to be a mangled header?
			uint16_t CodecFormat = pWAVHeader->fmt0_AudioFormat; // WAVE codec used
			uint16_t numChannels = pWAVHeader->fmt0_numChannels; // number of sample channels
			uint32_t SampleRate  = pWAVHeader->fmt0_SmplRate;    // samples per second (8000, 44100, etc.)
			uint32_t ByteRate    = pWAVHeader->fmt0_ByteRate;    //   bytes per second (SampleRate * FrameRate)
			uint16_t FrameRate   = pWAVHeader->fmt0_PlayRate;    //   bytes per sample frame [NumChannels * BitRate / 8]
			uint16_t BitRate     = pWAVHeader->fmt0_BitRate;     //    bits per sample (8, 16 or 32-bit)

			uint32_t SampleSize  = pWAVHeader->data_ChunkSize;   // size of actual sample data

			char const * err = "c";
			/** sanity check
			 *  "c" = All OK
			 *  "s" = Skip this WAV
			 *  "w" = Skip this WAV and print warning
			 *  "e" = Corrupted WAV header. Return immediately.
			 **/ if (SampleSize == 0)
			err = "sZero sample size";
			else if (numChannels == 0)
				err = "sZero channels";
			else if (SampleSize + i > WAV_PackSize)
				err = "eBad WAV data chunk size descriptor";
			else if (SampleRate > 192000)
				err = "wSampling rate is over 192000 Hz";
			else if (FrameRate != numChannels * BitRate / 8)
				err = "wInvalid block alignment";
			else if (ByteRate  != numChannels * BitRate / 8 * SampleRate)
				err = "wInvalid byte rate";

			/****/ if (err[0] == 'c') {
				// Everything is OK
			} else if (err[0] == 's') {
				pPtr += SampleSize, i += SampleSize;
				continue;
			} else if (err[0] == 'w') {
				fprintf(stderr, "[warn] Skipping WAV file (%s)\n", err + 1);
				pPtr += SampleSize, i += SampleSize;
				continue;
			} else if (err[0] == 'e') {
				fprintf(stderr, "[error] Skipping WAV section (%s)\n", err + 1);
				break;
			} else {
				assert(false);
			}

			// rip PCM data
			std::vector<uint8_t> pSmplData;
			pSmplData.insert(pSmplData.end(), &pPtr[0], &pPtr[SampleSize]);

			// decrypt data
			if (isEncrypted) {
				decrypt_arrange(pSmplData);
				decrypt_accXOR (pSmplData);
			}

			// create WAVE file buffer
			std::vector<uint8_t> poSmplData;

			WAV_Header WAVOutHead =
			{ .RIFF_ID   = 0x46464952           // "RIFF"
				, .RIFF_Size = SampleSize + 36
					, .RIFF_fmt0 = 0x45564157           // "WAVE"
					, .fmt0_ID   = 0x20746d66           // "fmt "
					, .fmt0_Size = 16
					, .fmt0_AudioFormat = CodecFormat
					, .fmt0_numChannels = numChannels
					, .fmt0_SmplRate  = SampleRate
					, .fmt0_ByteRate  = ByteRate
					, .fmt0_PlayRate  = FrameRate
					, .fmt0_BitRate   = BitRate
					, .data_ChunkID   = 0x61746164      // "data"
					, .data_ChunkSize = SampleSize
			};
			uint8_t* pWAVOutHead = reinterpret_cast<uint8_t*>(&WAVOutHead);
			poSmplData.insert(poSmplData.end(), pWAVOutHead, pWAVOutHead + sizeof(WAVOutHead));
			poSmplData.insert(poSmplData.end(), pSmplData.begin(), pSmplData.begin() + SampleSize);

			// pass into WAVE stream
			Sample pSample((char*)poSmplData.data(), poSmplData.size(), SampleName.c_str());

			if (pSample.getSource() == nullptr)
				fprintf(stderr, "[warn] Failed to load OMC WAV sample: %s\n", SampleName.c_str());
			else
				sample_map[smplID] = pSample;

			pPtr += SampleSize, i += SampleSize;
		}

		delete[] buffer;

	}

	/* reset accXOR */
	{
		std::vector<uint8_t> zero(0);
		decrypt_accXOR(zero, true);
	}

	if (OGG_PackSize > 0)
	{
		// parse OGG/MP3 files
		// create new read buffer from offset
		file.seek(OGG_Offset);

		buffer = new uint8_t[OGG_PackSize];
		read = file.read(buffer, OGG_PackSize);
		if (read != (int)OGG_PackSize)
			delete[] buffer;

		uint8_t* pPtr = buffer;

		smplID = 1000;

		for(unsigned long i = 0; i < OGG_PackSize; )
		{
			// read header
			OMC_OGG_Header *pOGGHeader = (OMC_OGG_Header*)pPtr;
			pPtr += OGGhSize, i += OGGhSize;

			smplID++;

			std::string SampleName = pOGGHeader->name; // already has extension
			uint32_t    SampleSize = pOGGHeader->size;

			if (SampleSize != 0) {
				uint8_t* pSmplData = pPtr;
				pPtr += SampleSize, i += SampleSize;

				Sample pSample((char*)pSmplData, SampleSize, SampleName.c_str());

				if (pSample.getSource() == nullptr)
					fprintf(stderr, "[warn] Failed to load OMC M sample: %s\n", SampleName.c_str());
				else
					sample_map[smplID] = pSample;

			}

		}
	}
}

void O2JamChart::load_samples()
{
	clan::File file;
	if (file.open(ojm_path,
				clan::File::OpenMode::open_existing,
				clan::File::AccessFlags::access_read
				) == false)
		throw std::invalid_argument("Failed to open OJM file.");

	if (file.get_size() < 4)
		throw std::invalid_argument("Malformed OJM file.");

	// Read file based on signature
	uint32_t signature = file.read_uint32();
	switch (signature)
	{
		case OJM_SIGNATURE: parseOMC(file, false, *mSampleMap); break;
		case OMC_SIGNATURE: parseOMC(file, true , *mSampleMap); break;
		case M30_SIGNATURE: parseM30(file, *mSampleMap); break;
		default: fprintf(stderr, "[warn] Unknown OJM signature. \n");
	}

}



};

