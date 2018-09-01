/* roadmap_library.c - a low level module to manage plugins for RoadMap.
 *
 * LICENSE:
 *
 *   Copyright 2002 Pascal F. Martin
 *   Copyright 2008 Ehud Shabtai
 *
 *   This file is part of RoadMap.
 *
 *   RoadMap is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   RoadMap is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with RoadMap; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * SYNOPSYS:
 *
 *   See roadmap_library.h
 */

#include <stdlib.h>
#include <string.h>
#include "roadmap.h"
#include "roadmap_sound.h"
#include "roadmap_lang.h"
#include "roadmap_file.h"
#include "roadmap_path.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <mad.h>
#include <pulse/simple.h>
#include <pulse/error.h>


#define LOSOAUDIO

#ifdef LOSOAUDIO
pa_simple *device = NULL;
int ret = 1;
int error;
struct mad_stream mad_stream;
struct mad_frame mad_frame;
struct mad_synth mad_synth;

void output(struct mad_header const *header, struct mad_pcm *pcm);
#endif // LOSOAUDIO




#define SND_VOLUME_LVLS_COUNT 4
const int SND_VOLUME_LVLS[] = {0, 1, 2, 3};
const char* SND_VOLUME_LVLS_LABELS[SND_VOLUME_LVLS_COUNT];
const char* SND_DEFAULT_VOLUME_LVL = "2";
static const char* get_full_name( const char* name );

RoadMapSoundList roadmap_sound_list_create (int flags) {
   fprintf(stdout, "\nloso roadmap_sound_list_create called\n");
   fflush(stdout);
   RoadMapSoundList list =
            (RoadMapSoundList) calloc (1, sizeof(struct roadmap_sound_list_t));
   list->flags = flags;

   return list;
}


int roadmap_sound_list_add (RoadMapSoundList list, const char *name) {

   const char* full_name;
   fprintf(stdout, "\nloso roadmap_sound_list_add called\n");
   fflush(stdout);

   if (list->count == MAX_SOUND_LIST) return SND_LIST_ERR_LIST_FULL;

   full_name = get_full_name( name );

   if ( !roadmap_file_exists( full_name, NULL ) )
   {
      roadmap_log( ROADMAP_ERROR, "File %s doesn't exist! Cannot add to the list.", full_name );
      return SND_LIST_ERR_NO_FILE;
   }

   strncpy_safe (list->list[list->count], name, sizeof(list->list[0]));
   list->count++;

   return list->count - 1;
}


static const char* get_full_name( const char* name ) {
   static char full_name[256];
   const char *suffix = "";

   fprintf(stdout, "\nloso get_full_name called\n");
   fflush(stdout);

   if ( !strchr( name, '.' ) )
   {
      suffix = ".mp3";
   }

   if ( roadmap_path_is_full_path( name ) )
   {
      strncpy_safe( full_name, name, sizeof( full_name ) );
   }
   else
   {
      snprintf( full_name, sizeof( full_name ), "%s//%s//%s//%s%s",
            roadmap_path_downloads(), "sound", roadmap_prompts_get_name(), name, suffix );
   }
   return full_name;
}


int roadmap_sound_list_count (const RoadMapSoundList list) {
   fprintf(stdout, "\nloso roadmap_sound_list_count called\n");
   fflush(stdout);

   return list->count;
}


const char *roadmap_sound_list_get (const RoadMapSoundList list, int i) {
   fprintf(stdout, "\nloso roadmap_sound_list_get called\n");
   fflush(stdout);

   if (i >= MAX_SOUND_LIST) return NULL;

   return list->list[i];
}


void roadmap_sound_list_free (RoadMapSoundList list) {
   fprintf(stdout, "\nloso roadmap_sound_list_free called\n");
   fflush(stdout);
   free(list);
}


RoadMapSound roadmap_sound_load (const char *path, const char *file, int *mem) {
   fprintf(stdout, "\nloso roadmap_sound_load called\n");
   fflush(stdout);
   return 0;
}


int roadmap_sound_free (RoadMapSound sound) {
   fprintf(stdout, "\nloso roadmap_sound_free called\n");
   fflush(stdout);
   return 0;
}


int roadmap_sound_play      (RoadMapSound sound) {
   fprintf(stdout, "\nloso roadmap_sound_play called\n");
   fflush(stdout);
   return 0;
}


int roadmap_sound_play_file (const char *file_name) {
   fprintf(stdout, "\nloso roadmap_sound_play_file %s called\n", file_name);
   fflush(stdout);

   // Initialize MAD library
   mad_stream_init(&mad_stream);
   mad_synth_init(&mad_synth);
   mad_frame_init(&mad_frame);


   // File pointer
   FILE *fp = fopen(file_name, "r");
   int fd = fileno(fp);

   // Fetch file size, etc
   struct stat metadata;
   if (fstat(fd, &metadata) >= 0) {
       printf("File size %d bytes\n", (int)metadata.st_size);
   } else {
       printf("Failed to stat %s\n", file_name);
       fclose(fp);
       return 254;
   }

   // Let kernel do all the dirty job of buffering etc, map file contents to memory
   char *input_stream = mmap(0, metadata.st_size, PROT_READ, MAP_SHARED, fd, 0);

   // Copy pointer and length to mad_stream struct
   mad_stream_buffer(&mad_stream, input_stream, metadata.st_size);

   // Decode frame and synthesize loop
   while (1) {

       // Decode frame from the stream
       if (mad_frame_decode(&mad_frame, &mad_stream)) {
           if (MAD_RECOVERABLE(mad_stream.error)) {
               continue;
           } else if (mad_stream.error == MAD_ERROR_BUFLEN) {
               break; //continue;
           } else {
               break;
           }
       }
       // Synthesize PCM data of frame
       mad_synth_frame(&mad_synth, &mad_frame);
       output(&mad_frame.header, &mad_synth.pcm);
   }

   // Close
   fclose(fp);

   // Free MAD structs
   mad_synth_finish(&mad_synth);
   mad_frame_finish(&mad_frame);
   mad_stream_finish(&mad_stream);

   return 0;
}

// Some helper functions, to be cleaned up in the future
int scale(mad_fixed_t sample) {
     /* round */
     sample += (1L << (MAD_F_FRACBITS - 16));
     /* clip */
     if (sample >= MAD_F_ONE)
         sample = MAD_F_ONE - 1;
     else if (sample < -MAD_F_ONE)
         sample = -MAD_F_ONE;
     /* quantize */
     return sample >> (MAD_F_FRACBITS + 1 - 16);
}


void output(struct mad_header const *header, struct mad_pcm *pcm) {
    register int nsamples = pcm->length;
    mad_fixed_t const *left_ch = pcm->samples[0], *right_ch = pcm->samples[1];
    static char stream[1152*4];
//    printf("ch = %d\n", pcm->channels);
    if (pcm->channels == 2) {
        while (nsamples--) {
            signed int sample;
            sample = scale(*left_ch++);
            stream[(pcm->length-nsamples)*4 ] = ((sample >> 0) & 0xff);
            stream[(pcm->length-nsamples)*4 +1] = ((sample >> 8) & 0xff);
            sample = scale(*right_ch++);
            stream[(pcm->length-nsamples)*4+2 ] = ((sample >> 0) & 0xff);
            stream[(pcm->length-nsamples)*4 +3] = ((sample >> 8) & 0xff);
        }
        if (pa_simple_write(device, stream, (size_t)1152*4, &error) < 0) {
            fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
            return;
        }
    } else if (pcm->channels == 1){
        while (nsamples--) {
            signed int sample;
            sample = scale(*left_ch++);
            stream[(pcm->length-nsamples)*4 ] = ((sample >> 0) & 0xff);
            stream[(pcm->length-nsamples)*4 +1] = ((sample >> 8) & 0xff);
            //sample = scale(*right_ch++);
            stream[(pcm->length-nsamples)*4+2 ] = ((sample >> 0) & 0xff);
            stream[(pcm->length-nsamples)*4 +3] = ((sample >> 8) & 0xff);
        }
        if (pa_simple_write(device, stream, (size_t)1152*4, &error) < 0) {
            fprintf(stderr, "pa_simple_write() failed: %s\n", pa_strerror(error));
            return;
        }
    } else {
        printf("output ch = %d is not supported!", pcm->channels);
    }
}




int roadmap_sound_list_add_buf (RoadMapSoundList list, void* buf, size_t size )
{
      char path[512];
      int file_num = list->count;
      RoadMapFile file;

   fprintf(stdout, "\nloso roadmap_sound_list_add_buf called\n");
   fflush(stdout);

      if (list->count == MAX_SOUND_LIST) return -1;

      list->buf_list[list->count] = buf;
      list->buf_list_sizes[list->count] = size;


      /*
       * Temporary solution - write the buffer to the file for further playing
       * AGA
       */
      sprintf( path, "%s/tmp/%d", roadmap_path_tts(), file_num );
      if ( file_num == 0 )
      {
         roadmap_path_create( roadmap_path_parent( path, NULL ) );
      }

      file = roadmap_file_open( path, "w" );
      roadmap_file_write( file, buf, size );
      roadmap_file_close( file );

      strncpy_safe( list->list[list->count], path, 512 );

      list->count++;

   return list->count - 1;
}

int roadmap_sound_play_list (const RoadMapSoundList list) {

	int i;
	char announce[2000];
	const char *full_name;

   fprintf(stdout, "\nloso roadmap_sound_play_list called\n");
   fflush(stdout);

	announce[0] = '\0';
	for (i = 0; i < list->count; i++) {
		strcat (announce, list->list[i]);
		strcat (announce, " ");

                const char *name = roadmap_sound_list_get ( list, i );
             if ( (list->flags & SOUND_LIST_BUFFERS) == 0 ) {
                 full_name = get_full_name( name );
                 roadmap_sound_play_file( full_name );
             }

	}
	if (*announce) {
          roadmap_log (ROADMAP_DEBUG, "Voice announce: %s\n", announce);
        }
   if (!(list->flags & SOUND_LIST_NO_FREE)) {
      roadmap_sound_list_free  (list);
   }
   return 0;
}


int roadmap_sound_record (const char *file_name, int seconds) {
   return 0;
}

void roadmap_sound_stop_recording (void) {
}


void roadmap_sound_initialize (void) {
   fprintf(stdout, "\nloso roadmap_sound_initialize called\n");
   fflush(stdout);
    // Set up PulseAudio 16-bit 44.1kHz stereo output
    static const pa_sample_spec ss = { .format = PA_SAMPLE_S16LE, .rate = 44100, .channels = 2 };
    if (!(device = pa_simple_new(NULL, "Waze Navigation", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        printf("pa_simple_new() failed\n");
    }

	// Initialize the volume labels for GUI
	SND_VOLUME_LVLS_LABELS[0] = roadmap_lang_get( "Silent" );
	SND_VOLUME_LVLS_LABELS[1] = roadmap_lang_get( "Low" );
	SND_VOLUME_LVLS_LABELS[2] = roadmap_lang_get( "Medium" );
	SND_VOLUME_LVLS_LABELS[3] = roadmap_lang_get( "High" );
}


void roadmap_sound_shutdown (void) {
   fprintf(stdout, "\nloso roadmap_sound_shutdown called\n");
   fflush(stdout);
   // Close PulseAudio output
   if (device) {
       pa_simple_free(device);
   }
}

/***********************************************************
 *      Name    : roadmap_sound_set_volume
 *      Purpose : Sets the user volume setting to the native sound object
 *                with configuration update
 */
void roadmap_sound_set_volume ( int volLvl ) {
   fprintf(stdout, "\nloso roadmap_sound_set_volume called\n");
   fflush(stdout);
}

