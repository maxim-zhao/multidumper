// multidumper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef struct gme_job
{
	unsigned int solo_voice;
} gme_job;

int track_number;

size_t size;
void * song_buffer;

pfc::string8 base_name;

volatile unsigned long long thread_count = 0;

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

		FILE * fw = _tfopen( pfc::stringcvt::string_os_from_utf8( name ), _T("wb") );

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

		InterlockedDecrement(&thread_count);
	}
};

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Usage: multidumper <path> [subsong]\n");
		return 1;
	}

	FILE * f = _tfopen( argv[1], _T("rb") );
	if (!f)
	{
		fprintf(stderr, "Unable to open file: %s\n", argv[1]);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fseek(f, 0, SEEK_SET);

	song_buffer = malloc(size);
	if (!song_buffer)
	{
		fprintf(stderr, "Out of memory.\n");
		fclose(f);
		return 1;
	}

	fread(song_buffer, 1, size, f);
	fclose(f);

	gme_t * gme;

	gme_err_t err = gme_open_data(song_buffer, size, &gme, 44100);

	if (err)
	{
		fprintf(stderr, "Error opening song: %s\n", err);
		return 1;
	}

	int track_count = gme_track_count(gme);

	if (argc == 2)
	{
		fprintf(stderr, "Song count: %d\n", track_count);
		gme_delete(gme);
		free(song_buffer);
		return 0;
	}

	TCHAR * end;

	track_number = (int) _tcstol( argv[2], &end, 0 );
	if (*end || track_number < 0 || track_number >= track_count)
	{
		_ftprintf(stderr, _T("Invalid track number: %s (Should be 0 - %d)\n"), argv[2], track_count - 1);
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

	base_name = pfc::stringcvt::string_utf8_from_os(argv[1]);
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

	free(song_buffer);

	return 0;
}
