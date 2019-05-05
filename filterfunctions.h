#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAX(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

#define MAX_NAME 2048
#define MAX_COMMAND 2*4096
/*

Linear congruential generator that returns between zero and one

*/

float lgc();

/*

Hard-copies from one frame to another

@f1     frame to copy from
@f2     frame to copy to
@H      height of the frame
@W      width of the frame

*/

void copy_from_frame_to(unsigned char *** f1, unsigned char *** f2, int H, int W);

/*

Fills a frame with input buffer

@f1     frame to copy to
@H      height of the frame
@W      width of the frame
@pipein input std

*/

int read_frame_into(unsigned char *** f1, int H, int W, FILE * pipein);

/*

Writes a frame to the output

@f1     frame to copy to
@H      height of the frame
@W      width of the frame
@pipeout    file going out

*/

int write_frame_to(unsigned char *** f1, int H, int W, FILE * pipeout);

/*

Allocates a new frame, zero initialize it and return it

@H      height of the frame
@W      width of the frame

*/

unsigned char *** allocate_frame(int H, int W);

/*

Moves by an x and y offset all pixels in a frame

@frame     frame to edit
@write_f    frame to save results
@xoffset    the offset in the x coord [y][x]
@yoffset    the offset in the y coord [y][x]
@H      height of the frame
@W      width of the frame

*/

void displace_frame_at_once(unsigned char *** frame, unsigned char *** write_frame, int xoffset, int yoffset, int H, int W);

/*

Moves by an x and y offset the pixels corresponding to a given color

@frame     frame to edit
@write_f    frame to save results
@xoffset    the offset in the x coord [y][x]
@yoffset    the offset in the y coord [y][x]
@color  which color will be affected [0,1,2]
@H      height of the frame
@W      width of the frame

*/

void displace_color_of_frame(unsigned char *** frame, unsigned char *** write_frame, int xoffset, int yoffset, int color, int H, int W);

/*

Moves by an x and y offset the pixels corresponding to a given color

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame

*/

void reduce_contrast_of_frame(unsigned char *** frame, unsigned char *** write_frame, int H, int W);

/*

Add uniform noise to frame

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame
@high   max value of noise to add per cell

*/

void add_uniform_noise_to_frame(unsigned char *** frame, unsigned char *** write_frame, int H, int W, int high);

/*

Add white horizontal noise line to video

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame
@fade_c the chande of having a white horizontal line [0,1]
@length_f   the base length for the chunks that compose the noise line
@rate   chance of drawing the noise [0,1]

*/

void add_horizontal_noise_line(unsigned char *** frame, unsigned char *** write_frame, int H, int W, float fade_chance, int length_fade, float rate);

/*

Add band noise to video

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame
@row_st starting position in rows
@band_h   height of the band

*/

void add_band_noise_to_frame(unsigned char *** frame, unsigned char *** write_frame, int H, int W, int row_start, int band_height);

/*

Combines all effects in one loop [NOT GOOD RIGHT NOW]

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame
@xoffset    the offset in the x coord [y][x]
@yoffset    the offset in the y coord [y][x]
@color  which color will be affected [0,1,2]
@high   max value of noise to add per cell

*/

void retro_effects_all_in_one(unsigned char *** frame, unsigned char *** write_frame, int H, int W, int xoffset, int yoffset, int color, int high);