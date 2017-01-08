// multidumper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

using json = nlohmann::json;

typedef struct gme_job
{
	unsigned int solo_voice;
} gme_job;

int track_number;

size_t size;
void * song_buffer;

pfc::string8 base_name;

volatile unsigned long long thread_count = 0;

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
		InterlockedIncrement(&thread_count);

		gme_t * gme;
		gme_info_t * info;
		unsigned int length, fade;
		size_t offset, written = 0;

		signed short buffer[1024];

		pfc::string8 name = base_name;

		gme_open_data(song_buffer, size, &gme, 44100);

		gme_track_info(gme, &info, track_number);

		if (info->loop_length > 0)
		{
			length = info->intro_length + info->loop_length * 2;
			fade = 8000;
		}
		else if (info->length > 0)
		{
			length = info->length;
			fade = 0;
		}
		else
		{
			length = 180000;
			fade = 8000;
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
		write_le32(fw, 44100); // sample rate
		write_le32(fw, 44100 * 2 * 2); // times stereo, 16 bits
		write_le16(fw, 2 * 2); // stereo, 16 bits
		write_le16(fw, 16); // bits

		write_be32(fw, 'data');
		offset = ftell(fw);
		write_le32(fw, 0);

		while (!gme_track_ended(gme))
		{
			int mspass = gme_tell(gme);

			int totlen = length + fade;

			//double percent = mspass / totlen;

			std::stringstream progress;
			progress << solo_voice << "|" << mspass << "|" << totlen << "\r\n";

			std::cout << progress.str();
			
			gme_err_t err = gme_play(gme, 1024, buffer);
			if (err) break;
			fwrite(buffer, 2, 1024, fw);
			written += 2048;
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
		InterlockedIncrement(&thread_count);

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
		write_le32(fw, 44100); // sample rate
		write_le32(fw, 44100 * 2 * 2); // times stereo, 16 bits
		write_le16(fw, 2 * 2); // stereo, 16 bits
		write_le16(fw, 16); // bits

		write_be32(fw, 'data');
		offset = ftell(fw);
		write_le32(fw, 0);

		wanted = spu_length << 2;

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
						if (spu_TicksPerSecond != 44100)
						{
							samples = (spu_log_data[CurrentLog].Tick - CurrentTick) * 44100 + remain;
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
	int argjson = -1;
	int argsubsong = -1;

	json j;

	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: multidumper <path> [subsong]\n");
		return 1;
	}

	for (int i = 1; i < argc; ++i)
	{
		if (_tcsicmp(argv[i], _T("--json")) == 0)
		{
			argjson = i;
			break;
		}
	}

	if (argjson == 1) argname = 2;
	else argname = 1;

	if (argjson > argname) argsubsong = argjson + 1;
	else argsubsong = argname + 1;

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

		if (argc == 3)
		{
			j["error"] = "Subsongs not supported by SPU Logs.";

			std::cout << j;
			return 1;
		}

		spu_TicksPerSecond = get_le32((const uint8_t *)song_buffer + 0x80200);
		spu_LogCount = get_le32((const uint8_t *)song_buffer + 0x80204);

		if (size == 0x80200 + 8 + spu_LogCount * sizeof(WriteRegLogOld))
		{
			spu_length = get_le32((const uint8_t *)song_buffer + 0x80200 + 8 + sizeof(WriteRegLogOld) * (spu_LogCount - 1));
		}
		else
		{
			spu_length = spu_TicksPerSecond;
			spu_TicksPerSecond = 44100;
			spu_LogCount = 0;
		}

		if (argjson > 0)
		{
			/*fprintf(stdout, "{\"subsongCount\": 1, \"channels\": [");
			for (int i = 0; i < 24; ++i) fprintf(stdout, "\"SPU ch #%d\"%s", i + 1, i < 24 ? ", " : "");
			fprintf(stdout, "]}");*/

			//j[]


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

		gme_err_t err = gme_open_data(song_buffer, size, &gme, 44100);


		if (err)
		{
			j["error"] = "Error opening song.";
			
			std::cout << j;

			return 1;
		}

		gme_info_t * gmeinfo_first;

		gme_err_t inferr = gme_track_info(gme, &gmeinfo_first, 0);

		int track_count = gme_track_count(gme);

		if (argjson > 0)
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

			std::vector<std::map<std::string, std::string>> infos;

			for (int x = 0; x < track_count; ++x)
			{

				gme_info_t * gmeinfo;

				gme_err_t inferrs = gme_track_info(gme, &gmeinfo, x);


				infos.push_back({ 
					{ "name", gmeinfo->song },
				    { "author", gmeinfo->author },
					{ "comment", gmeinfo->comment } });

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

		if (argc == 2)
		{
			fprintf(stderr, "Song count: %d\n", track_count);
			gme_delete(gme);
			free(song_buffer);
			return 0;
		}

		TCHAR * end;

		track_number = (int)_tcstol(argv[argsubsong], &end, 0);
		if (*end || track_number < 0 || track_number >= track_count)
		{
			_ftprintf(stderr, _T("Invalid track number: %s (Should be 0 - %d)\n"), argv[argsubsong], track_count - 1);
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
