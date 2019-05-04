#include "filterfunctions.h"

/*

Linear congruential generator

*/

float lgc(){
    static unsigned xn = 0;
    unsigned r = (1664525*xn+1013904223) % ( (unsigned) 0xFFFFFFFF);
    xn = r;
    float f = ((float) r) / (float) ( (unsigned) 0xFFFFFFFF);
    return f;
}

/*

Hard-copies from one frame to another

@f1     frame to copy from
@f2     frame to copy to
@H      height of the frame
@W      width of the frame

*/

void copy_from_frame_to(unsigned char *** f1, unsigned char *** f2, int H, int W){
    int x, y;
    for(x=0; x<H; x++){
        for(y=0; y<W; y++){
            f2[x][y][0] = f1[x][y][0];
            f2[x][y][1] = f1[x][y][1];
            f2[x][y][2] = f1[x][y][2];
        }
    }
}

/*

Fills a frame with input buffer

@f1     frame to copy to
@H      height of the frame
@W      width of the frame
@pipein input std

*/

int read_frame_into(unsigned char *** f1, int H, int W, FILE * pipein){
    
    unsigned char buffer[H][W][3];
    memset(buffer, 0, H*W*3);

    int count = fread(buffer, 1, H*W*3, pipein);

    if (count != H*W*3) return -1;

    int x, y;
    for(x=0; x<H; x++){
        for(y=0; y<W; y++){
            f1[x][y][0] = buffer[x][y][0];
            f1[x][y][1] = buffer[x][y][1];
            f1[x][y][2] = buffer[x][y][2];
        }
    }

    return 0;    
}

/*

Writes a frame to the output

@f1     frame to copy to
@H      height of the frame
@W      width of the frame
@pipeout    file going out

*/

int write_frame_to(unsigned char *** f1, int H, int W, FILE * pipeout){
    
    unsigned char buffer[H][W][3];
    memset(buffer, 0, H*W*3);

    int x, y;
    for(x=0; x<H; x++){
        for(y=0; y<W; y++){
            buffer[x][y][0] = f1[x][y][0];
            buffer[x][y][1] = f1[x][y][1];
            buffer[x][y][2] = f1[x][y][2];
        }
    }

    fwrite(buffer, 1, H*W*3, pipeout);

    return 0;    
}

/*

Allocates a new frame, zero initialize it and return it

@H      height of the frame
@W      width of the frame

*/

unsigned char *** allocate_frame(int H, int W){

    int x, y;

    unsigned char *** frame = (unsigned char ***) malloc(H*sizeof(unsigned char **));
    if(frame == NULL) { fprintf(stderr, "Could not allocate frame - EXITING\n"); exit(-1); }
    for(x=0; x<H; x++){
        frame[x] = (unsigned char **) malloc(W*sizeof(unsigned char *));
        if(frame[x] == NULL) { fprintf(stderr, "Could not allocate frame X - EXITING\n"); exit(-1); }
        for(y=0; y<W; y++){
            frame[x][y] = (unsigned char *) malloc(3*sizeof(unsigned char));
            if(frame[x][y] == NULL) { fprintf(stderr, "Could not allocate frame X,Y - EXITING\n"); exit(-1); }
            frame[x][y][0] = 0;
            frame[x][y][0] = 1;
            frame[x][y][0] = 2;
        }
    }
    
    return frame;
}

/*

Moves by an x and y offset all pixels in a frame

@frame     frame to edit
@write_f    frame to save results
@xoffset    the offset in the x coord [y][x]
@yoffset    the offset in the y coord [y][x]
@H      height of the frame
@W      width of the frame

*/

void displace_frame_at_once(unsigned char *** frame, unsigned char *** write_frame, int xoffset, int yoffset, int H, int W){
    
    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        if(y+yoffset < H && y+yoffset >= 0 && x+xoffset < W && x+xoffset >= 0)
        {
            write_frame[y+yoffset][x+xoffset][0] = frame[y][x][0];
            write_frame[y+yoffset][x+xoffset][1] = frame[y][x][1];
            write_frame[y+yoffset][x+xoffset][2] = frame[y][x][2];
        }
    }
}

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

void displace_color_of_frame(unsigned char *** frame, unsigned char *** write_frame, int xoffset, int yoffset, int color, int H, int W){

    if(color < 0 || color > 2) { fprintf(stderr, "BAD COLOR - EXITING\n"); exit(-1); }

    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        if(y+yoffset < H && y+yoffset >= 0 && x+xoffset < W && x+xoffset >= 0)
        {
            write_frame[y+yoffset][x+xoffset][color] = frame[y][x][color];
        }
    }
}

/*

Moves by an x and y offset the pixels corresponding to a given color

@frame     frame to edit
@write_f    frame to save results
@factor strength of contrast reduction from 0 to 1
@H      height of the frame
@W      width of the frame

*/

void reduce_contrast_of_frame(unsigned char *** frame, unsigned char *** write_frame, float factor, int H, int W){

    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        write_frame[y][x][0] = sqrt(frame[y][x][0]) * 16;
        write_frame[y][x][1] = sqrt(frame[y][x][1]) * 16;
        write_frame[y][x][2] = sqrt(frame[y][x][2]) * 16;
    }
}

/*

Add uniform noise to frame

@frame     frame to edit
@write_f    frame to save results
@H      height of the frame
@W      width of the frame
@high   max value of noise to add per cell

*/

void add_uniform_noise_to_frame(unsigned char *** frame, unsigned char *** write_frame, int H, int W, int high){
    
    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        int r = (lgc() * high) - high/2;
        write_frame[y][x][0] = MIN(MAX(frame[y][x][0] + r, 0), 255);
        write_frame[y][x][1] = MIN(MAX(frame[y][x][1] + r, 0), 255);
        write_frame[y][x][2] = MIN(MAX(frame[y][x][2] + r, 0), 255);

    }
}

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

void add_horizontal_noise_line(unsigned char *** frame, unsigned char *** write_frame, int H, int W, float fade_chance, int length_fade, float rate){
    
    int y;

    if(rate < lgc()) return;

    int r = MAX(MIN((lgc()) * H, H-5), 5);
    
    //int l_fade = 10;
    //float fade_chance = 0.2;

    int have_chance = 0;

    for (y=0 ; y<W-5; ++y)
    {
        
        if(have_chance > 0)
        {
            int v = lgc()/4 * 255;

            write_frame[r][y][0] = MIN(frame[r][y][0] + v, 255);
            write_frame[r][y][1] = MIN(frame[r][y][1] + v, 255);
            write_frame[r][y][2] = MIN(frame[r][y][2] + v, 255);

            write_frame[r][y+1][0] = MIN(frame[r][y][0] + v, 255);
            write_frame[r][y+1][1] = MIN(frame[r][y][1] + v, 255);
            write_frame[r][y+1][2] = MIN(frame[r][y][2] + v, 255);

            write_frame[r+1][y+2][0] = MIN(frame[r][y][0] + v, 255);
            write_frame[r+1][y+2][1] = MIN(frame[r][y][1] + v, 255);
            write_frame[r+1][y+2][2] = MIN(frame[r][y][2] + v, 255);

            --have_chance;
        }
        else if(fade_chance > lgc())
        {
            have_chance = lgc() * length_fade;

        }
    }
}

void retro_effects_all_in_one(unsigned char *** frame, unsigned char *** write_frame, int H, int W, int xoffset, int yoffset, int color, float fade_chance, int length_fade, float rate){

    // This adds the effect of distorted colors
    //displace_color_of_frame(frame, write_frame, 4, 4, 0, H, W);

    // This reduces contrast by a factor f
    //reduce_contrast_of_frame(write_frame, frame, 0.5, H, W);

    // This adds noise to the frame from uniform *random* numbers
    //add_uniform_noise_to_frame(frame, write_frame, H, W, 25);

    // This adds a white noise line
    //add_horizontal_noise_line(frame, write_frame, H, W, 0.2, 10, 0.4);

    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        if(y+yoffset < H && y+yoffset >= 0 && x+xoffset < W && x+xoffset >= 0)
        {
            write_frame[y+yoffset][x+xoffset][color] = frame[y][x][color];
        }

        
    }
}


/*
void reduce_contrast_of_frame(unsigned char *** frame, unsigned char *** write_frame, float factor, int H, int W){

    int y, x;
    for (y=0 ; y<H ; ++y) for (x=0 ; x<W ; ++x)
    {
        write_frame[y][x][0] = sqrt(frame[y][x][0]) * 16;
        write_frame[y][x][1] = sqrt(frame[y][x][1]) * 16;
        write_frame[y][x][2] = sqrt(frame[y][x][2]) * 16;
    }

    */