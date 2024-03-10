
// VAG2WAV taken from http://psxsdk.googlecode.com/svn-history/r13/trunk/tools/vag2wav.c

/*
 * vag2wav
 *
 * Converts a PlayStation VAG sound file to a WAV file
 *
 * Based on VAG-Depack 0.1 by bITmASTER
 */

#include "p3t.h"

double f[5][2] = { { 0.0, 0.0 },
                    {   60.0 / 64.0,  0.0 },
                    {  115.0 / 64.0, -52.0 / 64.0 },
                    {   98.0 / 64.0, -55.0 / 64.0 },
                    {  122.0 / 64.0, -60.0 / 64.0 } };

double samples[28];

int vag2wav( const char* vagfile, const char* pcmfile)
{
	FILE *vag, *pcm;
    char vag_name[17];
    int predict_nr, shift_factor, flags;
    int i;
    int d, s;
    static double s_1 = 0.0;
    static double s_2 = 0.0;
    int sz;
//	unsigned int samp_freq;
//	unsigned int data_size;
	int samp_freq;
	int data_size;
    
    vag = fopen( vagfile, "rb" );
    size_t read_size = 0; 
    read_size = fread(vag_name, sizeof(char), 4, vag);
    
    if(vag == NULL)
    {
    	printf("Can't open %s. Aborting.\n", vagfile);
	return -1;
    }
    
    if(strncmp(vag_name, "VAGp", 4))
    {
    	printf("%s is not in VAG format. Aborting.\n", vagfile);
		fclose(vag);
	return -1;
    }
    
    fseek(vag, 4, SEEK_SET);
    i = fgetc(vag) << 24;
    i |= fgetc(vag) << 16;
    i |= fgetc(vag) << 8;
    i |= fgetc(vag);
#ifdef OUTPUT_DEBUG_TEXT
    printf("Version: %x\n", i);
#endif
    
    fseek(vag, 12, SEEK_SET);
    data_size = fgetc(vag) << 24;
    data_size |= fgetc(vag) << 16;
    data_size |= fgetc(vag) << 8;
    data_size |= fgetc(vag);
#ifdef OUTPUT_DEBUG_TEXT
    printf("Data size: %d bytes\n", data_size);   
#endif    
    fseek(vag, 32, SEEK_SET);
    read_size = fread(vag_name, sizeof(char), 16, vag);
    vag_name[16] = 0;
#ifdef OUTPUT_DEBUG_TEXT
    printf("Name: %s\n", vag_name);
#endif   
    fseek(vag, 16, SEEK_SET);
    
    samp_freq = fgetc(vag)<<24;
    samp_freq|=fgetc(vag)<<16;
    samp_freq|=fgetc(vag)<<8;
    samp_freq|=fgetc(vag);
#ifdef OUTPUT_DEBUG_TEXT
    printf("Sampling frequency: %d\n", samp_freq);
#endif 
    fseek( vag, 64, SEEK_SET );

   /* strcpy( fname, argv[1] );
    p = strrchr( fname, '.' );
    p++;
    strcpy( p, "PCM" );*/
    pcm = fopen( pcmfile, "wb" );
    
    if ( pcm == NULL ) {
        printf( "can´t write output file\n" );
        fclose(vag);
        return( -8 );
    }


// Write header chunk	
	fprintf(pcm, "RIFF");
// Skip file size field for now
	fseek(pcm, 4, SEEK_CUR);

	fprintf(pcm, "WAVE");    
// Write fmt chunk
	fprintf(pcm, "fmt ");
// Write chunk 1 size in little endian format	
	fputc(0x10, pcm);
	fputc(0, pcm);
	fputc(0, pcm);
	fputc(0, pcm);
// Write audio format (1 = PCM)	
	fputc(1, pcm);
	fputc(0, pcm);
// Number of channels (1)
	fputc(1, pcm);
	fputc(0, pcm);
// Write sample rate
	fputc((samp_freq & 0xff), pcm);
	fputc((samp_freq >> 8) & 0xff, pcm);
	fputc(0, pcm);
	fputc(0, pcm);
// Write byte rate (SampleRate * NumChannels * BitsPerSample/8)
// That would be 44100*1*(16/8), thus 88200.
	fputc(((samp_freq*2) & 0xff), pcm);
	fputc(((samp_freq*2) >> 8) & 0xff, pcm);
	fputc(((samp_freq*2) >> 16) & 0xff, pcm);
	fputc(((samp_freq*2) >> 24) & 0xff, pcm);
// Write block align (NumChannels * BitsPerSample/8), thus 2
	fputc(2, pcm);
	fputc(0, pcm);
// Write BitsPerSample
	fputc(16, pcm);
	fputc(0, pcm);

// Write data chunk
	fprintf(pcm, "data");

// Skip SubChunk2Size, we will return to it later
	fseek(pcm, 4, SEEK_CUR);
	
// Now write data...

    while( ftell(vag) < (data_size+48)) {
        predict_nr = fgetc( vag );
        shift_factor = predict_nr & 0xf;
        predict_nr >>= 4;
        flags = fgetc( vag );                           // flags
        if ( flags == 7 )
            break;              
        for ( i = 0; i < 28; i += 2 ) {
            d = fgetc( vag );
            s = ( d & 0xf ) << 12;
            if ( s & 0x8000 )
                s |= 0xffff0000;
            samples[i] = (double) ( s >> shift_factor  );
            s = ( d & 0xf0 ) << 8;
            if ( s & 0x8000 )
                s |= 0xffff0000;
            samples[i+1] = (double) ( s >> shift_factor  );
        }
    
        for ( i = 0; i < 28; i++ ) {
            samples[i] = samples[i] + s_1 * f[predict_nr][0] + s_2 * f[predict_nr][1];
            s_2 = s_1;
            s_1 = samples[i];
            d = (int) ( samples[i] + 0.5 );
            fputc( d & 0xff, pcm );
            fputc( d >> 8, pcm );
        }
    }
    
// Get file size    
    sz = ftell(pcm);
    
// Now write ChunkSize
    fseek(pcm, 4, SEEK_SET);
    
    fputc((sz-8) & 0xff, pcm);
    fputc(((sz-8)>>8)&0xff, pcm);
    fputc(((sz-8)>>16)&0xff,pcm);
    fputc(((sz-8)>>24)&0xff,pcm);
    
// Now write Subchunk2Size
    fseek(pcm, 40, SEEK_SET);
    
    fputc((sz-44) & 0xff, pcm);
    fputc(((sz-44)>>8)&0xff, pcm);
    fputc(((sz-44)>>16)&0xff,pcm);
    fputc(((sz-44)>>24)&0xff,pcm);    
    
    fclose( pcm );
    fclose( vag );
    return( 0 );
}

// VAG2WAV END
