//
// Video processing example using FFmpeg
// Written by Ted Burke - last updated 12-2-2017
//
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "filterfunctions.h"
 
// Video resolution
//#define W 1280
//#define H 720

// Video resolution
int W;
int H;
int fps;

 
// Allocate a buffer to store one frame
//unsigned char frame[H][W][3] = {0};

 
int main(int ac, char ** av)
{
    int x, y, count;

    if(ac < 8)
    {
        fprintf(stderr, "Use: retro-video <video-name> <x> <y> <fps> <out-name> <audio-format> <date in ms>\n");
        exit(-1);
    }

    // Get parameters

    W = atoi(av[2]);
    H = atoi(av[3]);

    fps = atoi(av[4]);

    char name[MAX_NAME];
    name[0] = '\0'; strcpy(name, av[1]);

    char outname[MAX_NAME];
    outname[0] = '\0'; strcpy(outname, av[5]);

    char outname_temp[MAX_NAME];
    outname_temp[0] = '\0'; strcpy(outname_temp, "TEMP-"); strcat(outname_temp, av[5]); 

    char audio_format[MAX_NAME];
    audio_format[0] = '\0'; strcpy(audio_format, av[6]);

    int date = atoi(av[7]);

    fprintf(stdout, "INFO: V-name: %s, wide: %d, height: %d, fps: %d, O-name: %s, A-codec: %s\n", name, W, H, fps, outname, audio_format);

    // Build commands

    char comm_pipe_in[MAX_COMMAND]; comm_pipe_in[0] = '\0';
    char comm_pipe_out[MAX_COMMAND]; comm_pipe_out[0] = '\0';
    char comm_text[MAX_COMMAND]; comm_text[0] = '\0';
    char comm_get_audio[MAX_COMMAND]; comm_get_audio[0] = '\0';
    char comm_write_audio[MAX_COMMAND]; comm_write_audio[0] = '\0';
    char rm[MAX_COMMAND]; rm[0] = '\0';

    sprintf(comm_pipe_in, "ffmpeg -i %s -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -", name);
    sprintf(comm_pipe_out, "ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s %dx%d -r %d -i - -f mp4 -q:v 8 -vcodec mpeg4 %s.temp", W, H, fps, outname);
    sprintf(comm_text, "ffmpeg -y -i %s.temp -vf drawtext=\"fontfile=VCR_OSD_MONO_1.001.ttf: text='%%{pts\\:gmtime\\:%d}': fontcolor=white: fontsize=36: box=1: boxcolor=black@0.0: boxborderw=5: x=50: y=h-100\" -codec:a copy %s", outname, date, outname_temp);
    sprintf(comm_get_audio, "ffmpeg -y -i %s -vn -acodec copy %s.%s", name, name, audio_format);
    sprintf(comm_write_audio, "ffmpeg -y -i %s -i %s.%s -shortest -c:v copy -c:a aac -b:a 256k %s", outname_temp, name, audio_format, outname);

    
    // Allocate frame

    unsigned char *** frame = allocate_frame(H, W);
    unsigned char *** write_frame = allocate_frame(H, W);


    //unsigned char frame[H][W][3] = {0};
    //unsigned char write_frame[H][W][3] = {0};
    
     
    // Open an input pipe from ffmpeg and an output pipe to a second instance of ffmpeg
    //FILE *pipein = popen("ffmpeg -i video.mp4 -f image2pipe -vcodec rawvideo -pix_fmt rgb24 -", "r");
    //FILE *pipeout = popen("ffmpeg -y -f rawvideo -vcodec rawvideo -pix_fmt rgb24 -s 1280x720 -r 25 -i - -f mp4 -q:v 8 -vcodec mpeg4 output.mp4", "w");

    FILE *pipein = popen(comm_pipe_in, "r");
    FILE *pipeout = popen(comm_pipe_out, "w");

    // Returned errors
    int ret;
     
    // Process video frames
    while(1)
    {
        // Read a frame from the input pipe into the buffer
        //count = fread(frame, 1, H*W*3, pipein);

        ret = read_frame_into(frame, H, W, pipein);
        
         
        // If we didn't get a frame of video, we're probably at the end
        if (ret != 0) break;
         
        // Process this frame
        /*
        for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
        {
            // Invert each colour component in every pixel
            frame[y][x][0] = 255 - frame[y][x][0]; // red
            frame[y][x][1] = 255 - frame[y][x][1]; // green
            frame[y][x][2] = 255 - frame[y][x][2]; // blue
        }
        */
        


         
        // Write this frame to the output pipe
        //fwrite(frame, 1, H*W*3, pipeout);

        copy_from_frame_to(frame, write_frame, H, W);

        // This adds the effect of distorted colors
        displace_color_of_frame(frame, write_frame, 4, 4, 0, H, W);

        // This reduces contrast by a factor f
        reduce_contrast_of_frame(write_frame, frame, 0.5, H, W);

        // This adds noise to the frame from uniform *random* numbers
        add_uniform_noise_to_frame(frame, write_frame, H, W, 25);

        // This adds a white noise line
        add_horizontal_noise_line(frame, write_frame, H, W, 0.2, 10, 0.4);
        
        // Write frame to output pipe
        write_frame_to(write_frame, H, W, pipeout);

    }
     
    // Flush and close input and output pipes
    fflush(pipein);
    pclose(pipein);
    fflush(pipeout);
    pclose(pipeout);

    //system("ffmpeg -y -i output.mp4 -vf drawtext=\"fontfile=VCR_OSD_MONO_1.001.ttf: text='JUN 14 2001': fontcolor=white: fontsize=36: box=1: boxcolor=black@0.0: boxborderw=5: x=50: y=h-100\" -codec:a copy output2.mp4");
    system(comm_text);
    
    //system("ffmpeg -y -i video.mp4 -vn -acodec copy audio.aac");
    system(comm_get_audio);
    
    //system("ffmpeg -y -i output.mp4 -i audio.aac -shortest -c:v copy -c:a aac -b:a 256k final.mp4");
    system(comm_write_audio);

    // Delete temporary files
    sprintf(rm, "rm %s.temp", outname);
    system(rm);
    sprintf(rm, "rm %s.%s", name, audio_format);
    system(rm);
    sprintf(rm, "rm %s", outname_temp);
    system(rm);


    /*
    for(x=0; x<H; x++){
        for(y=0; y<W; y++){
            free(frame[x][y]);
        }
        free(frame[x]);
    }
    free(frame);
    */

    return 0;
}