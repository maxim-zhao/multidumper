// multidumper.cpp : Defines the entry point for the console application.
//

#include <pfc/pfc.h>

// For reasons, we get issues with blargg_common if we don't put the gme includes before fex
#include <gme/Music_Emu.h>
#include <Data_Reader.h>
#include <Gzip_Extractor.h>
#include <spu.h>
#include <spucore.h>

#include "json.hpp"

using json = nlohmann::json;

typedef struct gme_job
{
	unsigned int solo_voice;
} gme_job;

int track_number = 0;

size_t size;
void * song_buffer;
int sampling_rate = 44100;
int play_length = -1;
int loop_count = 2;
int fade_length = 8000;
int gap_length = 1000;

bool show_progress = true;

pfc::string8 base_name;

t_size max_thread_count = 1;

volatile LONG thread_count = 0;

struct WriteRegLogOld
{
	uint32_t Tick;
	uint32_t reg;
	uint32_t val;
};

typedef struct
{
	int32_t	y0, y1;
} ADPCM_Decode_t;

typedef struct
{
	int32_t				freq;
	int32_t				nbits;
	int32_t				stereo;
	int32_t				nsamples;
	ADPCM_Decode_t	left, right;
	int16_t			pcm[16384];
} xa_decode_t;

struct WriteRegLog
{
	enum
	{
		type_reg_write = 0,
		type_reg_read,
		type_dma_write,
		type_dma_read,
		type_play_adpcm,
		type_play_cdda
	};

	uint32_t Tick;
	uint8_t type;
	uint32_t reg;
	uint16_t val;
	pfc::array_t<xa_decode_t> xa;
	pfc::array_t<uint16_t> data;

	const WriteRegLog & operator= (const WriteRegLog & src)
	{
		Tick = src.Tick;
		type = src.type;

		switch (type)
		{
		case type_reg_write:
			reg = src.reg;
			val = src.val;
			break;
		case type_reg_read:
			reg = src.reg;
			break;
		case type_dma_write:
			reg = src.reg;
			data = src.data;
			break;
		case type_dma_read:
			reg = src.reg;
			break;
		case type_play_adpcm:
			xa = src.xa;
			break;
		case type_play_cdda:
			reg = src.reg;
			data = src.data;
			break;
		}
		return *this;
	}
};

pfc::array_t<WriteRegLog> spu_log_data;
uint32_t spu_TicksPerSecond;
uint32_t spu_LogCount;
uint32_t spu_length;

static inline uint16_t get_le16(const void *_p)
{
	const uint8_t *p = (const uint8_t *)_p;
	return p[0] | (p[1] << 8);
}

static inline uint32_t get_le32(const void *_p)
{
	const uint8_t *p = (const uint8_t *)_p;
	return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

void write_le16(FILE *f, unsigned short v)
{
	unsigned char buffer[2];
	buffer[0] = v & 0xFF;
	buffer[1] = (v >> 8) & 0xFF;
	fwrite(buffer, 1, 2, f);
}

void write_le32(FILE *f, unsigned int v)
{
	unsigned char buffer[4];
	buffer[0] = v & 0xFF;
	buffer[1] = (v >> 8) & 0xFF;
	buffer[2] = (v >> 16) & 0xFF;
	buffer[3] = (v >> 24) & 0xFF;
	fwrite(buffer, 1, 4, f);
}

void write_be32(FILE *f, unsigned int v)
{
	unsigned char buffer[4];
	buffer[3] = v & 0xFF;
	buffer[2] = (v >> 8) & 0xFF;
	buffer[1] = (v >> 16) & 0xFF;
	buffer[0] = (v >> 24) & 0xFF;
	fwrite(buffer, 1, 4, f);
}

class gme_thread : public pfc::thread
{
	unsigned int solo_voice;

public:
	void startVoice(unsigned int voice)
	{
		solo_voice = voice;
		//std::cout << voice << "\r\n";
		start();
	}

	void threadProc()
	{
		gme_t * gme;
		gme_info_t * info;
		unsigned int length, fade;
		size_t offset, written = 0;

#define BUFFER_SAMPLE_COUNT 1024
		signed short buffer[BUFFER_SAMPLE_COUNT*2]; // x2 for stereo

		pfc::string8 name = base_name;

		gme_open_data(song_buffer, size, &gme, sampling_rate);

		gme_track_info(gme, &info, track_number);

		if (play_length > 0)
		{
		    length = play_length;
			fade = fade_length;
		}
		else if (info->loop_length > 0)
		{
			length = info->intro_length + info->loop_length * loop_count;
			fade = fade_length;
		}
		else if (info->length > 0)
		{
			length = info->length;
			fade = fade_length;
		}
		else
		{
			length = 180000; // Default 3 minutes
			fade = fade_length;
		}

		unsigned int mute_mask = ~(1 << solo_voice);

		gme_ignore_silence(gme, true);

		gme_mute_voices(gme, mute_mask);

		gme_start_track(gme, track_number);

		gme_set_fade(gme, length, fade);

		name += gme_voice_name(gme, solo_voice);
		name += ".wav";

		FILE * fw = _tfopen(pfc::stringcvt::string_os_from_utf8(name), _T("wb"));

		write_be32(fw, 'RIFF');
		write_le32(fw, 0);
		write_be32(fw, 'WAVE');

		write_be32(fw, 'fmt ');
		write_le32(fw, 16);
		write_le16(fw, 1); // PCM
		write_le16(fw, 2); // stereo
		write_le32(fw, sampling_rate); // sample sampling_rate
		write_le32(fw, sampling_rate * 2 * 2); // times stereo, 16 bits
		write_le16(fw, 2 * 2); // stereo, 16 bits
		write_le16(fw, 16); // bits

		write_be32(fw, 'data');
		offset = ftell(fw);
		write_le32(fw, 0);

		uint64_t totlen = length;
		if (info->loop_length > 0)
		{
			// Loops fade
			totlen += fade;
		}
		else
		{
			// Non-looped have a bit of silence
			totlen += gap_length;
		}
		uint64_t remaining = totlen * 44100 / 1000; // sample count

		while (thread_count >= max_thread_count)
		{
			if (show_progress)
			{
				std::stringstream progress;
				progress << solo_voice << "|" << 0 << "|" << totlen << "\r\n";

				std::cout << progress.str();
			}
			Sleep(100);
		}

		InterlockedIncrement(&thread_count);

		while (remaining > 0) // was (!gme_track_ended(gme))
		{
			if (show_progress)
			{
				int mspass = gme_tell(gme);

				//double percent = mspass / totlen;

				std::stringstream progress;
				progress << solo_voice << "|" << mspass << "|" << totlen << "\r\n";

				std::cout << progress.str();
			}

			// Render a full buffer or part thereof if near the end
			int count = (int)std::min((uint64_t)BUFFER_SAMPLE_COUNT, remaining);

			gme_err_t err = gme_play(gme, count*2, buffer);
			if (err) break;
			fwrite(buffer, sizeof(signed short), count*2, fw); // *2 for stereo
			written += count * sizeof(signed short) * 2;
			remaining -= count;
		}

		if (show_progress)
		{

			std::stringstream progress;
			progress << solo_voice << "|" << totlen << "|" << totlen << "\r\n";

			std::cout << progress.str();
		}

		gme_delete(gme);

		fseek(fw, 4, SEEK_SET);
		write_le32(fw, written + offset - 4);
		fseek(fw, offset, SEEK_SET);
		write_le32(fw, written);

		fclose(fw);
		//std::cout << solo_voice << "!\r\n";

		InterlockedDecrement(&thread_count);
	}
};

class spu_thread : public pfc::thread
{
	unsigned int solo_voice;

public:
	void startVoice(unsigned int voice)
	{
		solo_voice = voice;
		//std::cout << voice << "\r\n";
		start();
	}

	void threadProc()
	{
		void * spu = malloc(spu_get_state_size(1));
		if (!spu) return;

		spu_clear_state(spu, 1);
		spu_enable_main(spu, 1);
		spu_enable_reverb(spu, 0);

		uint32_t voice;
		for (voice = 0; voice < solo_voice; ++voice)
			spu_enable_mute(spu, voice, 1);
		for (voice = solo_voice + 1; voice < 24; ++voice)
			spu_enable_mute(spu, voice, 1);
		spu_enable_mute(spu, solo_voice, 0);

		/*
		** DMA Transfer the SPU RAM
		*/
		uint32_t addr;
		spu_sh(spu, 0x1f801da6, 0);
		for (addr = 0; addr < 0x80000; addr += 2)
			spu_sh(spu, 0x1f801da8, get_le16((const uint8_t *)song_buffer + addr));

		/*
		** Now the initial registers
		*/
		for (addr = 0x80000; addr < 0x80200; addr += 2)
		{
			if ((addr - 0x80000 + 0x1f801c00) == 0x1f801da8) continue;
			spu_sh(spu, addr - 0x80000 + 0x1f801c00, get_le16((const uint8_t *)song_buffer + addr));
		}

		uint32_t CurrentLog = 0;
		uint32_t remainder = 0;
		uint32_t CurrentTick = 0;
		uint32_t rendered, remain, samples;
		uint32_t wanted;

		size_t offset, written = 0;

		int16_t buffer[1024];

		pfc::string8 name = base_name;

		name += "ch #";
		name += pfc::format_int(solo_voice);
		name += ".wav";

		FILE * fw = _tfopen(pfc::stringcvt::string_os_from_utf8(name), _T("wb"));

		write_be32(fw, 'RIFF');
		write_le32(fw, 0);
		write_be32(fw, 'WAVE');

		write_be32(fw, 'fmt ');
		write_le32(fw, 16);
		write_le16(fw, 1); // PCM
		write_le16(fw, 2); // stereo
		write_le32(fw, sampling_rate); // sample sampling_rate
		write_le32(fw, sampling_rate * 2 * 2); // times stereo, 16 bits
		write_le16(fw, 2 * 2); // stereo, 16 bits
		write_le16(fw, 16); // bits

		write_be32(fw, 'data');
		offset = ftell(fw);
		write_le32(fw, 0);

		wanted = spu_length << 2;

		while (thread_count >= max_thread_count)
		{
			if (show_progress)
			{
				std::stringstream progress;
				progress << solo_voice << "|" << 0 << "|" << wanted << "\r\n";

				std::cout << progress.str();
			}
			Sleep(100);
		}

		InterlockedIncrement(&thread_count);

		while (written < wanted)
		{
			rendered = 0;

			int16_t * p = buffer;

			do
			{
				if (remainder)
				{
					if (remainder > 512)
					{
						spu_render(spu, p, 512);
						remainder -= 512;
						rendered = 512;
						break;
					}
					spu_render(spu, p, remainder);
					p += remainder << 1;
					rendered = remainder;
					remainder = 0;
					if (CurrentLog == spu_LogCount)
					{
						break;
					}
				}

				remain = 0;

				while (CurrentLog < spu_LogCount)
				{
					while ((CurrentLog < spu_LogCount) && (spu_log_data[CurrentLog].Tick <= CurrentTick))
					{
						//SPUwriteRegister(pLog[CurrentLog].reg, pLog[CurrentLog].val);
						WriteRegLog & entry = spu_log_data[CurrentLog];
						switch (entry.type)
						{
						case WriteRegLog::type_reg_write:
							spu_sh(spu, entry.reg, entry.val);
							break;
						case WriteRegLog::type_dma_write:
							spu_dma(spu, 0, entry.data.get_ptr(), 0, ~0, entry.reg * 2, 1);
							break;
						case WriteRegLog::type_dma_read:
							t_uint32 temp;
							spu_dma(spu, 0, &temp, 0, 0, entry.reg * 2, 0);
							break;
						}
						CurrentLog++;
					}
					if (CurrentLog == spu_LogCount)
					{
						samples = spu_length - spu_log_data[spu_LogCount - 1].Tick;
					}
					else
					{
						if (spu_TicksPerSecond != sampling_rate)
						{
							samples = (spu_log_data[CurrentLog].Tick - CurrentTick) * sampling_rate + remain;
							remain = samples % spu_TicksPerSecond;
							samples = samples / spu_TicksPerSecond;
						}
						else
							samples = spu_log_data[CurrentLog].Tick - CurrentTick;
						CurrentTick = spu_log_data[CurrentLog].Tick;
					}
					if (samples > (512 - rendered))
					{
						remainder = samples - (512 - rendered);
						samples = 512 - rendered;
						rendered = 512;
						spu_render(spu, p, samples);
						break;
					}
					if (samples)
					{
						spu_render(spu, p, samples);
						rendered += samples;
						p += samples << 1;
					}
				}
			} while (0);

			if (rendered)
			{
				fwrite(buffer, 4, rendered, fw);
				written += rendered << 2;
			}
			else break;

			if (show_progress)
			{
				std::stringstream progress;
				progress << solo_voice << "|" << written << "|" << wanted << "\r\n";

				std::cout << progress.str();
			}
		}

		if (show_progress)
		{
			std::stringstream progress;
			progress << solo_voice << "|" << wanted << "|" << wanted << "\r\n";

			std::cout << progress.str();
		}

		free(spu);

		fseek(fw, 4, SEEK_SET);
		write_le32(fw, written + offset - 4);
		fseek(fw, offset, SEEK_SET);
		write_le32(fw, written);

		fclose(fw);

		//std::cout << solo_voice << "!\r\n";


		InterlockedDecrement(&thread_count);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	int argname = -1;
	bool dump_metadata_as_json = false;
	bool show_help = false;

	json j;

	max_thread_count = pfc::getOptimalWorkerThreadCount();

	for (int i = 1; i < argc; ++i)
	{
		if (_tcsicmp(argv[i], _T("--json")) == 0)
		{
			dump_metadata_as_json = true;
		}
		else if (_tcsicmp(argv[i], _T("--no_progress")) == 0)
		{
			show_progress = false;
		}
		else if (_tcsnicmp(argv[i], _T("--sampling_rate="), 16) == 0)
		{
			sampling_rate = std::stoi(argv[i] + 16);
		}
		else if (_tcsnicmp(argv[i], _T("--play_length="), 14) == 0)
		{
			play_length = std::stoi(argv[i] + 14);
		}
		else if (_tcsnicmp(argv[i], _T("--fade_length="), 14) == 0)
		{
			fade_length = std::stoi(argv[i] + 14);
		}
		else if (_tcsnicmp(argv[i], _T("--gap_length="), 13) == 0)
		{
			gap_length = std::stoi(argv[i] + 13);
		}
		else if (_tcsnicmp(argv[i], _T("--loop_count="), 13) == 0)
		{
			loop_count = std::stoi(argv[i] + 13);
		}
		else if (_tcsicmp(argv[i], _T("--help")) == 0 || _tcsicmp(argv[i], _T("-h")) == 0 || _tcsicmp(argv[i], _T("/h")) == 0)
		{
			show_help = true;
		}
		else if (argname == -1)
		{
			argname = i;
		}
		else if (track_number == 0)
		{
			try
			{
				track_number = std::stoi(argv[i]);
			}
			catch (const std::exception&)
			{
				fprintf(stderr, "Error parsing track number \"%ls\"\n", argv[i]);
				show_help = true;
			}
		}
		else
		{
			show_help = true;
		}
	}

    if (show_help || argname == -1)
	{
		fprintf(stderr, "Usage: multidumper <path> <subsong> [--json] [--no_progress] [--sampling_rate=<number>] [--play_length=<ms>] [--fade_length=<ms>] [--gap_length=<ms>] [--loop_count=<number>]\n");
		return 1;
	}

	fprintf(stderr, "File is %ls\n", argv[argname]);
	fprintf(stderr, "Subsong is %d\n", track_number);
	fprintf(stderr, "Sampling rate is %d\n", sampling_rate);


    FILE * f = _tfopen(argv[argname], _T("rb"));
	if (!f)
	{
		
		j["error"] = "Unable to open file.";

		std::cout << j;

		return 1;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	song_buffer = malloc(size);
	if (!song_buffer)
	{
		j["error"] = "Out of memory.";

		std::cout << j;
		fclose(f);
		return 1;
	}

	fread(song_buffer, 1, size, f);
	fclose(f);

	while (size >= 2 && ((uint8_t*)song_buffer)[0] == 0x1F && ((uint8_t*)song_buffer)[1] == 0x8B)
	{
		size_t new_size;
		void * new_song_buffer;

		{
			Mem_File_Reader memreader(song_buffer, size);

			Gzip_Extractor gzex;

			blargg_err_t err = gzex.open(&memreader);
			if (err)
			{
				j["error"] = "Error opening gzipped file.";

				std::cout << j;
				free(song_buffer);
				return 1;
			}

			err = gzex.stat();
			if (err)
			{
				j["error"] = "Error probing stats of gzipped file.";

				std::cout << j;
				free(song_buffer);
				return 1;
			}

			new_size = gzex.size();

			new_song_buffer = malloc(new_size);
			if (!new_song_buffer)
			{
				j["error"] = "Out of memory.";

				std::cout << j;
				free(song_buffer);
				return 1;
			}

			Data_Reader & gzreader = gzex.reader();

			err = gzreader.read(new_song_buffer, new_size);

			if (err)
			{
				j["error"] = "Error reading gzipped song.";

				std::cout << j;
				free(new_song_buffer);
				free(song_buffer);
				return 1;
			}
		}

		free(song_buffer);
		song_buffer = new_song_buffer;
		size = new_size;
	}

	const TCHAR * ext = _tcsrchr(argv[argname], _T('.'));
	if (ext && _tcsicmp(ext + 1, _T("spu")) == 0)
	{
		spu_init();
		spucore_init();

		if (size < 524808 || size >(1 << 30))
		{
			j["error"] = "SPU log not usable size.";

			std::cout << j;
			return 1;
		}

		const int track_count = 1;
		const int voice_count = 24;

		spu_TicksPerSecond = get_le32((const uint8_t *)song_buffer + 0x80200);
		spu_LogCount = get_le32((const uint8_t *)song_buffer + 0x80204);

		if (size == 0x80200 + 8 + spu_LogCount * sizeof(WriteRegLogOld))
		{
			spu_length = get_le32((const uint8_t *)song_buffer + 0x80200 + 8 + sizeof(WriteRegLogOld) * (spu_LogCount - 1));
		}
		else
		{
			spu_length = spu_TicksPerSecond;
			spu_TicksPerSecond = sampling_rate;
			spu_LogCount = 0;
		}

		if (dump_metadata_as_json)
		{
			j["subsongCount"] = 1;
			j["containerinfo"] = {
				{ "system", "Sony PlayStation" },
				{ "game", "" },
				{ "dumper", "" },
				{ "copyright", "" } };

			j["songs"] = { {
				{ "name", "" },
				{ "author", "" },
				{ "comment", "" } } };

			std::vector<std::string> channels;

			for (int i = 0; i < voice_count; ++i)
			{
				std::stringstream channel;
				channel << "SPU #" << i;
				channels.push_back(channel.str());
			}

			j["channels"] = channels;

			std::cout << j;

			return 0;
		}

		if (spu_LogCount)
		{
			spu_log_data.set_size(spu_LogCount);

			uint32_t addr = 0x80200 + 8;

			for (unsigned i = 0; i < spu_LogCount; i++)
			{
				WriteRegLog & val = spu_log_data[i];
				val.Tick = get_le32((const uint8_t *)song_buffer + addr);
				val.reg = get_le32((const uint8_t *)song_buffer + addr + 4);
				val.val = get_le16((const uint8_t *)song_buffer + addr + 8);
				val.type = WriteRegLog::type_reg_write;
				addr += 12;
			}
		}
		else
		{
			uint32_t addr = 0x80200 + 4;

			while (addr < size)
			{
				WriteRegLog data;

				data.Tick = get_le32((const uint8_t *)song_buffer + addr);
				data.type = *((const uint8_t *)song_buffer + addr + 4);
				addr += 5;

				switch (data.type)
				{
				case WriteRegLog::type_reg_write:
					data.reg = get_le32((const uint8_t *)song_buffer + addr);
					data.val = get_le16((const uint8_t *)song_buffer + addr + 4);
					addr += 6;
					break;

				case WriteRegLog::type_reg_read:
					data.reg = get_le32((const uint8_t *)song_buffer + addr);
					addr += 4;
					break;

				case WriteRegLog::type_dma_write:
				{
					data.reg = get_le32((const uint8_t *)song_buffer + addr);
					addr += 4;
					data.data.set_count(data.reg);
					uint16_t * words = data.data.get_ptr();
					for (unsigned i = 0; i < data.reg; i++)
					{
						words[i] = get_le16((const uint8_t *)song_buffer + addr);
						addr += 2;
					}
				}
				break;

				case WriteRegLog::type_dma_read:
					data.reg = get_le32((const uint8_t *)song_buffer + addr);
					addr += 4;
					break;

				case WriteRegLog::type_play_adpcm:
				{
					data.xa.set_count(1);
					xa_decode_t * xa = data.xa.get_ptr();
					xa->freq = get_le32((const uint8_t *)song_buffer + addr);
					xa->nbits = get_le32((const uint8_t *)song_buffer + addr + 4);
					xa->stereo = get_le32((const uint8_t *)song_buffer + addr + 8);
					xa->nsamples = get_le32((const uint8_t *)song_buffer + addr + 12);
					xa->left.y0 = get_le32((const uint8_t *)song_buffer + addr + 16);
					xa->left.y1 = get_le32((const uint8_t *)song_buffer + addr + 20);
					xa->right.y0 = get_le32((const uint8_t *)song_buffer + addr + 24);
					xa->right.y1 = get_le32((const uint8_t *)song_buffer + addr + 28);
					addr += 32;
					for (unsigned i = 0; i < 16384; i++)
					{
						xa->pcm[i] = get_le16((const uint8_t *)song_buffer + addr);
						addr += 2;
					}
				}
				break;

				case WriteRegLog::type_play_cdda:
				{
					data.reg = get_le32((const uint8_t *)song_buffer + addr);
					addr += 4;
					data.data.set_count(data.reg / 2);
					int16_t * samples = (int16_t *)data.data.get_ptr();
					for (unsigned i = 0, j = data.reg / 2; i < j; i++)
					{
						samples[i] = get_le16((const uint8_t *)song_buffer + addr);
						addr += 2;
					}
				}
				break;

				default:
					fprintf(stderr, "Unknown event type in SPU Log.\n");
					return 1;
				}

				spu_log_data.append_single(data);
				spu_LogCount++;
			}
		}

		base_name = pfc::stringcvt::string_utf8_from_os(argv[argname]);
		base_name.truncate(base_name.find_last('.'));
		base_name += " - ";

		pfc::array_t<spu_thread*> threads;

		for (int i = 0; i < voice_count; ++i)
		{
			threads.append_single(new spu_thread);
		}
		for (int i = 0; i < voice_count; ++i)
		{
			threads[i]->startVoice(i);
		}
		while (thread_count)
		{
			Sleep(100);
		}
		for (int i = 0; i < voice_count; ++i)
		{
			threads[i]->waitTillDone();
		}
		for (int i = 0; i < voice_count; ++i)
		{
			delete threads[i];
		}
	}
	else
	{
		gme_t * gme;

		gme_err_t err = gme_open_data(song_buffer, size, &gme, sampling_rate);


		if (err)
		{
			j["error"] = "Error opening song.";
			
			std::cout << j;

			return 1;
		}

		gme_info_t * gmeinfo_first;

		gme_err_t inferr = gme_track_info(gme, &gmeinfo_first, 0);

		int track_count = gme_track_count(gme);

		if (dump_metadata_as_json)
		{
			/*fprintf(stdout, "{\"subsongCount\": %d, \"system\": \"%s\", \"channels\": [", track_count, gmeinfo->system);
			for (int i = 0; i < voice_count; ++i) fprintf(stdout, "\"%s\"%s", gme_voice_name(gme, i), i < voice_count-1 ? ", " : "");
			fprintf(stdout, "]}");*/

			int voice_count = gme_voice_count(gme);


			j["subsongCount"] = track_count;
			j["containerinfo"] = { 
				{ "system", gmeinfo_first->system },
				{ "game", gmeinfo_first->game },
				{ "dumper", gmeinfo_first->dumper },
				{ "copyright", gmeinfo_first->copyright } };

			json infos = json::array();

			for (int x = 0; x < track_count; ++x)
			{
				gme_info_t * gmeinfo;

				gme_err_t inferrs = gme_track_info(gme, &gmeinfo, x);

				infos.push_back({
					{"name", gmeinfo->song},
					{"author", gmeinfo->author},
					{"comment", gmeinfo->comment},
					{"length", gmeinfo->length},
					{"loop_length", gmeinfo->loop_length},
					{"intro_length", gmeinfo->intro_length},
					{"fade_length", gmeinfo->fade_length},
					{"play_length", gmeinfo->play_length},
					});
				gme_free_info(gmeinfo);
			}

			j["songs"] = infos;

			std::vector<std::string> channels;

			for (int i = 0; i < voice_count; ++i)
			{
				channels.push_back(gme_voice_name(gme, i));
			}

			j["channels"] = channels;
			
			std::cout << j;
			
			return 0;
		}

		if (track_number < 0 || track_number >= track_count)
		{
			_ftprintf(stderr, _T("Invalid track number: %d (Should be 0 - %d)\n"), track_number, track_count - 1);
			gme_delete(gme);
			free(song_buffer);
			return 1;
		}

		err = gme_start_track(gme, track_number);
		if (err)
		{
			fprintf(stderr, "Error starting track number %d: %s\n", track_number, err);
			gme_delete(gme);
			free(song_buffer);
			return 1;
		}

		base_name = pfc::stringcvt::string_utf8_from_os(argv[argname]);
		base_name.truncate(base_name.find_last('.'));
		base_name += " - ";

		pfc::array_t<gme_thread*> threads;

		int voice_count = gme_voice_count(gme);

		gme_delete(gme);

		for (int i = 0; i < voice_count; ++i)
		{
			threads.append_single(new gme_thread);
		}
		for (int i = 0; i < voice_count; ++i)
		{
			threads[i]->startVoice(i);
		}
		while (thread_count)
		{
			Sleep(100);
		}
		for (int i = 0; i < voice_count; ++i)
		{
			threads[i]->waitTillDone();
		}
		for (int i = 0; i < voice_count; ++i)
		{
			delete threads[i];
		}
	}

	free(song_buffer);

	return 0;
}
