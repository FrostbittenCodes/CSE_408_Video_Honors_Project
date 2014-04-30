// main.c

// A program that uses libavformat and libavcodec to
// read video from a file and seperates the pixels into either
// foreground or background. Written for an honors contract for
// CSE408.
//
// Use the Makefile to build.
//
// Run using
//
// ./main myvideofile.mpg
//
// Author: Steven Brown



//optomization macros
#define likely(x)       __builtin_expect((x),1)
#define unlikely(x)     __builtin_expect((x),0)


//Parameters
#define FRAMES  1000
#define ADAPT   0.05
#define BOUND   4
#define RECOLOR_AMOUNT 1

//Misc Options:
#define ERODE_DILATE
#define RECOLOR
//#define ADAPTIVE_THRESHOLD


//Background Adaptation Method (uncomment both for #3):
#define ADAPTIVE_BG1
#define ADAPTIVE_BG2


//History/Future options (will choose lowest in the list if multiple uncommented):
#define B1F0      //Default, just 1 backward frame
//#define B1F1      //1 backward frame, 1 forward
//#define B2F0      //2 backward frames, 0 forward
//#define B2F1      //2 backward frames, 1 forward
//#define B2F2      //2 backward frames, 2 forward


//-----------------------------------------------------------------------------------------------
//              Don't Edit below
//-----------------------------------------------------------------------------------------------

#define CIRCULAR_FRAME_BUFFER_COUNT 2
#define DELTA abs((head->pFrame->data[0])[j] - (head->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->pFrame->data[0])[j+2])

#ifdef B1F1
#undef CIRCULAR_FRAME_BUFFER_COUNT
#undef DELTA
#define CIRCULAR_FRAME_BUFFER_COUNT 3
#define DELTA ((abs((head->Prev->pFrame->data[0])[j] - (head->Prev->Prev->pFrame->data[0])[j])+ abs((head->Prev->pFrame->data[0])[j+1] - (head->Prev->Prev->pFrame->data[0])[j+1])+ abs((head->Prev->pFrame->data[0])[j+2] - (head->Prev->Prev->pFrame->data[0])[j+2])) + (abs((head->pFrame->data[0])[j] - (head->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->pFrame->data[0])[j+2])))/2
#endif

#ifdef B2F0
#undef CIRCULAR_FRAME_BUFFER_COUNT
#undef DELTA
#undef ALPHA
#define CIRCULAR_FRAME_BUFFER_COUNT 3
#define ALPHA 0.75
#define DELTA (abs((head->pFrame->data[0])[j] - (head->Prev->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->Prev->pFrame->data[0])[j+2]))*(1-ALPHA) + (abs((head->pFrame->data[0])[j] - (head->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->pFrame->data[0])[j+2]))*(ALPHA)
#endif

#ifdef B2F1
#undef CIRCULAR_FRAME_BUFFER_COUNT
#undef DELTA
#undef ALPHA
#define CIRCULAR_FRAME_BUFFER_COUNT 4
#define ALPHA 0.75
#define DELTA ((abs((head->Prev->pFrame->data[0])[j] - (head->Prev->Prev->Prev->pFrame->data[0])[j])+ abs((head->Prev->pFrame->data[0])[j+1] - (head->Prev->Prev->Prev->pFrame->data[0])[j+1])+ abs((head->Prev->pFrame->data[0])[j+2] - (head->Prev->Prev->Prev->pFrame->data[0])[j+2]))*(1-ALPHA) + (abs((head->Prev->pFrame->data[0])[j] - (head->Prev->Prev->pFrame->data[0])[j])+ abs((head->Prev->pFrame->data[0])[j+1] - (head->Prev->Prev->pFrame->data[0])[j+1])+ abs((head->Prev->pFrame->data[0])[j+2] - (head->Prev->Prev->pFrame->data[0])[j+2]))*(ALPHA) + (abs((head->pFrame->data[0])[j] - (head->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->pFrame->data[0])[j+2])))/2
#endif

//TODO:fix the below (head is the future frame, not the current)
#ifdef B2F2
#undef CIRCULAR_FRAME_BUFFER_COUNT
#undef DELTA
#undef ALPHA
#define CIRCULAR_FRAME_BUFFER_COUNT 5
#define ALPHA 0.75
#define DELTA ((abs((head->pFrame->data[0])[j] - (head->Prev->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->Prev->pFrame->data[0])[j+2]))*(1-ALPHA) + (abs((head->pFrame->data[0])[j] - (head->Prev->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Prev->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Prev->pFrame->data[0])[j+2]))*(ALPHA) + (abs((head->pFrame->data[0])[j] - (head->Next->Next->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Next->Next->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Next->Next->pFrame->data[0])[j+2]))*(1-ALPHA) + (abs((head->pFrame->data[0])[j] - (head->Next->pFrame->data[0])[j])+ abs((head->pFrame->data[0])[j+1] - (head->Next->pFrame->data[0])[j+1])+ abs((head->pFrame->data[0])[j+2] - (head->Next->pFrame->data[0])[j+2]))*(ALPHA))/2
#endif


#ifdef ADAPTIVE_THRESHOLD
#undef CIRCULAR_FRAME_BUFFER_COUNT
#define CIRCULAR_FRAME_BUFFER_COUNT 10
#endif


//-----------------------------------------------------------------------------------------------

//For video stuff
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

//for timestamps
#include <sys/time.h>

#include <stdio.h>


typedef struct Frame Frame;

struct Frame
{
    AVFrame *pFrame;
    Frame *Prev;
    Frame *Next;
};


void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) 
{
    FILE *pFile;
    char szFilename[32];
    int  y;
  
    // Open file
    sprintf(szFilename, "output/frame%d.ppm", iFrame);
    pFile=fopen(szFilename, "wb");
    if(pFile==NULL)
        return;
  
    // Write header
    fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
    // Write pixel data
    for(y=0; y<height; y++)
        fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
    // Close file
    fclose(pFile);
}

int main(int argc, char *argv[]) 
{
    //For video stuff
    AVFormatContext *pFormatCtx = NULL;
    AVCodecContext  *pCodecCtx = NULL;
    AVCodec         *pCodec = NULL;
    AVFrame         *pFrame = NULL;
    AVFrame         *ptempFrame = NULL;
    AVFrame         *pBackground = NULL;
    AVPacket        packet;
    int             i, j, k, l, videoStream;
    int             frameFinished;
    int             numBytes;
    uint8_t         *buffer3 = NULL; //for temp wip frame
    uint8_t         *buffer4 = NULL; //for background frame
    uint8_t         *scratch = NULL; //misc space
    uint8_t         *buffers[CIRCULAR_FRAME_BUFFER_COUNT]; //for current RGB frame and future/history
    Frame           *head = NULL;
    Frame           *iterator=NULL;

    AVDictionary            *optionsDict = NULL;
    struct SwsContext       *sws_ctx  = NULL;
    struct SwsContext       *sws_ctx2 = NULL;

    //For metrics
    int bytes = sizeof(AVFrame)*5 + sizeof(AVPacket) + sizeof(AVCodec) + sizeof(AVCodecContext) + sizeof(AVFormatContext) + sizeof(sws_ctx)*2 + sizeof(optionsDict)
                + sizeof(long int) + sizeof(int)*9;
    int frames = 0;
    long unsigned int time = 0;



    //For timestamps
    struct timeval t1, t2;
  
  
    if(argc < 2) 
    {
        printf("Please provide an input file\n");
        return -1;
    }

    printf("Initializing\n");

    // Register all formats and codecs
    av_register_all();
  
    // Open video file
    if(avformat_open_input(&pFormatCtx, argv[1], NULL, NULL)!=0)
        return -1; // Couldn't open file
  
    // Retrieve stream information
    if(avformat_find_stream_info(pFormatCtx, NULL)<0)
        return -1; // Couldn't find stream information
  
    // Dump information about file onto standard error
    av_dump_format(pFormatCtx, 0, argv[1], 0);
  
    // Find the first video stream
    videoStream=-1;
    for(i=0; i<pFormatCtx->nb_streams; i++)
    {
        if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO)
        {
            videoStream=i;
            break;
        }
    }

    if(videoStream==-1)
        return -1; // Didn't find a video stream
  
    // Get a pointer to the codec context for the video stream
    pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
    // Find the decoder for the video stream
    pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
    if(pCodec==NULL) 
    {
        fprintf(stderr, "Unsupported codec!\n");
        return -1; // Codec not found
    }

    // Open codec
    if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0)
        return -1; // Could not open codec
  
    // Allocate video frames
    pFrame=av_frame_alloc();
    if(pFrame==NULL)
        return -1;

    pBackground=av_frame_alloc();
    if(pBackground==NULL)
        return -1;

    ptempFrame=av_frame_alloc();
    if(ptempFrame==NULL)
        return -1;

    //alloc CIRCULAR_..._COUNT frames, first pointed to by head, make circular buffer

    head = malloc(sizeof(Frame));
    head->pFrame = NULL;
    head->Prev = head;
    head->Next = head;
    head->pFrame=av_frame_alloc();
    if(head->pFrame==NULL)
        return -1;

    for(i = 1; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        Frame *temp = malloc(sizeof(Frame));
        temp->Next = head;
        temp->Prev = head->Prev;
        head->Prev->Next = temp;
        head->Prev = temp;

        //alloc video frames inside frame structs
        temp->pFrame=av_frame_alloc();
        if(temp->pFrame==NULL)
            return -1;
    }

    //Determine required buffer sizes and allocate buffers
    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
        buffers[i] = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
        bytes += numBytes;
    }

    //For temp/background
    numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    buffer3 = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    bytes += numBytes;

    numBytes = avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    buffer4 = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));
    bytes += numBytes;

    //misc space
    scratch = (uint8_t *)malloc(pCodecCtx->width*pCodecCtx->height*sizeof(uint8_t));
    bytes += pCodecCtx->width*pCodecCtx->height*sizeof(uint8_t);


    //For converting between the video format and RGB24 frames
    sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, 
                             PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

    //For copying between RGB24's frames
    sws_ctx2 = sws_getContext(pCodecCtx->width, pCodecCtx->height, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height, 
                             PIX_FMT_RGB24, SWS_BILINEAR, NULL, NULL, NULL);

  
    // Assign appropriate parts of buffer to image planes in pFrameRGB
    // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
    // of AVPicture
    iterator = head;
    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        avpicture_fill((AVPicture *)iterator->pFrame, buffers[i], PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
        iterator = iterator->Next;
    }

    //For temp/background
    avpicture_fill((AVPicture *)ptempFrame, buffer3, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);
    avpicture_fill((AVPicture *)pBackground, buffer4, PIX_FMT_RGB24, pCodecCtx->width, pCodecCtx->height);


    printf("Initialization Complete\n");

    printf("\nDimensions\t%dx%d\n", pCodecCtx->width, pCodecCtx->height);


    i = 0;
    j = 0;
    k = 0;
    l = 0;

    //Read and process frames
    while(av_read_frame(pFormatCtx, &packet)>=0) 
    {
        if(frames > FRAMES)
            break;
        // Is this a packet from the video stream?
        if(packet.stream_index==videoStream) 
        {
            // Decode video frame
            avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
      
            // Did we get a video frame?
            if(frameFinished) 
            {

                //Save for metrics
                gettimeofday(&t1,NULL);

                
                // Convert the image from its native format to RGB

                sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                           head->pFrame->data, head->pFrame->linesize);


                //Initialize background with first frame
                if(unlikely(frames == 0))
                    sws_scale(sws_ctx2, (uint8_t const * const *)head->pFrame->data, head->pFrame->linesize, 0, pCodecCtx->height,
                           pBackground->data, pBackground->linesize);


                //TODO:Look into adaptive rates
                //filter insignificant changes
                for(j = (head->pFrame->linesize[0])*BOUND; j < (head->pFrame->linesize[0])*(pCodecCtx->height-BOUND); j = j + 3)
                {
                    #ifdef ADAPTIVE_BG1
                    //For adapting background (needed for once-offs, hue changes, camera movement)
                    (pBackground->data[0])[j] = (1.0-ADAPT)*(pBackground->data[0])[j] + (ADAPT)*(head->pFrame->data[0])[j];
                    (pBackground->data[0])[j+1] = (1.0-ADAPT)*(pBackground->data[0])[j+1] + (ADAPT)*(head->pFrame->data[0])[j+1];
                    (pBackground->data[0])[j+2] = (1.0-ADAPT)*(pBackground->data[0])[j+2] + (ADAPT)*(head->pFrame->data[0])[j+2];  
                    #endif

                    //Check if current position is in one of the left/right buffers, if so skip
                    if((j%(head->pFrame->linesize[0]) < BOUND*3) || (j%(head->pFrame->linesize[0]) >= (head->pFrame->linesize[0]-((BOUND+5)*3))))
                        continue;
                    
                    //Check background differences
                    l = abs((head->pFrame->data[0])[j] - (pBackground->data[0])[j]) + abs((head->pFrame->data[0])[j+1] - (pBackground->data[0])[j+1])
                            + abs((head->pFrame->data[0])[j+2] - (pBackground->data[0])[j+2]);


                    #ifndef ADAPTIVE_THRESHOLD
                    if(l < 60) //If low, (likely) background pixel
                    #else
                    iterator = head->Next; //Point iterator at oldest frame
                    k = 0;
                    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT - 2; i++) //cumulate differences between frames
                    {
                        k += abs((iterator->pFrame->data[0])[j] - (iterator->Next->pFrame->data[0])[j]);
                        iterator = iterator->Next;
                    }
                    if(k < 60) //max(60,k)
                        k = 60;

                    if(l < k)
                    #endif
                    {
                        //check to see if it has significant temporal differences
                        l = DELTA;
                        if(l < 90) //if not, set to ignore (black, stationary background)
                        {
                            (ptempFrame->data[0])[j] = 0;
                            (ptempFrame->data[0])[j+1] = 0;
                            (ptempFrame->data[0])[j+2] = 0;
                        }
                        else //moving background (blue)
                        {
                            (ptempFrame->data[0])[j] = 0;
                            (ptempFrame->data[0])[j+1] = 0;
                            (ptempFrame->data[0])[j+2] = 0;

                            #ifdef ADAPTIVE_BG2
                            //For adapting background (needed for once-offs, hue changes, camera movement)
                            (pBackground->data[0])[j] = (1.0-ADAPT)*(pBackground->data[0])[j] + (ADAPT)*(head->pFrame->data[0])[j];
                            (pBackground->data[0])[j+1] = (1.0-ADAPT)*(pBackground->data[0])[j+1] + (ADAPT)*(head->pFrame->data[0])[j+1];
                            (pBackground->data[0])[j+2] = (1.0-ADAPT)*(pBackground->data[0])[j+2] + (ADAPT)*(head->pFrame->data[0])[j+2];  
                            #endif
                        }
                    }
                    else //else, (likely) foreground pixel
                    {
                        //check to see if it has significant temporal differences
                        l = DELTA;
                        if(l < 90) //stationary foreground (green)
                        {
                            (ptempFrame->data[0])[j] = 255;
                            (ptempFrame->data[0])[j+1] = 255;
                            (ptempFrame->data[0])[j+2] = 255;

                            #ifdef ADAPTIVE_BG2
                            //For adapting background (needed for once-offs, hue changes, camera movement)
                            (pBackground->data[0])[j] = (1.0-ADAPT)*(pBackground->data[0])[j] + (ADAPT)*(head->pFrame->data[0])[j];
                            (pBackground->data[0])[j+1] = (1.0-ADAPT)*(pBackground->data[0])[j+1] + (ADAPT)*(head->pFrame->data[0])[j+1];
                            (pBackground->data[0])[j+2] = (1.0-ADAPT)*(pBackground->data[0])[j+2] + (ADAPT)*(head->pFrame->data[0])[j+2];  
                            #endif
                        }
                        else //moving foreground (red)
                        {
                            (ptempFrame->data[0])[j] = 255;
                            (ptempFrame->data[0])[j+1] = 255;
                            (ptempFrame->data[0])[j+2] = 255;
                        }
                    }
                }


                //--------------------------------------------------------------------------------------------------------------------------------

                #ifdef ERODE_DILATE 
                //Erode (for removing noise) using a 3x3 square
                for(j = (head->pFrame->linesize[0]); j < (head->pFrame->linesize[0])*(pCodecCtx->height-1); j = j + 3)
                {
                    //Left/right edges
                    if((j%(head->pFrame->linesize[0]) == 0) || (j%(head->pFrame->linesize[0]) == (head->pFrame->linesize[0])-3))
                    {
                        (ptempFrame->data[0])[j] = 0;
                        (ptempFrame->data[0])[j+1] = 0;
                        (ptempFrame->data[0])[j+2] = 0;
                        scratch[j/3] = 0;
                    }

                    i  = ((ptempFrame->data[0])[j-3-head->pFrame->linesize[0]] & (ptempFrame->data[0])[j-head->pFrame->linesize[0]] & (ptempFrame->data[0])[j+3-head->pFrame->linesize[0]]);
                    i &= ((ptempFrame->data[0])[j-3] & (ptempFrame->data[0])[j] & (ptempFrame->data[0])[j+3]);
                    i &= ((ptempFrame->data[0])[j-3+head->pFrame->linesize[0]] & (ptempFrame->data[0])[j+head->pFrame->linesize[0]] & (ptempFrame->data[0])[j+3+head->pFrame->linesize[0]]);

                    scratch[j/3] = i;
                }

                //Dilate (for removing noise) using a 3x3 square
                for(j = (head->pFrame->linesize[0]); j < (head->pFrame->linesize[0])*(pCodecCtx->height-1); j = j + 3)
                {
                    //Left/right edges
                    if((j%(head->pFrame->linesize[0]) == 0) || (j%(head->pFrame->linesize[0]) == (head->pFrame->linesize[0])-3))
                        continue;

                    i  = (scratch[(j-3-head->pFrame->linesize[0])/3] | scratch[(j-head->pFrame->linesize[0])/3] | scratch[(j+3-head->pFrame->linesize[0])/3]);
                    i |= (scratch[(j-3)/3] | scratch[j/3] | scratch[(j+3)/3]);
                    i |= (scratch[(j-3+head->pFrame->linesize[0])/3] | scratch[(j+head->pFrame->linesize[0])/3] | scratch[(j+3+head->pFrame->linesize[0])/3]);

                    (ptempFrame->data[0])[j] = i;
                    (ptempFrame->data[0])[j+1] = i;
                    (ptempFrame->data[0])[j+2] = i;
                }
                #endif

                //--------------------------------------------------------------------------------------------------------------------------------

                #ifdef RECOLOR
                //Add color data back to make it more human friendly
                for(j = 0; j < (head->pFrame->linesize[0])*(pCodecCtx->height); j++)
                {
                    if((ptempFrame->data[0])[j])
                        (ptempFrame->data[0])[j] = (head->pFrame->data[0])[j];
                    else
                        (ptempFrame->data[0])[j] = (head->pFrame->data[0])[j]/RECOLOR_AMOUNT;
                }
                #endif

                //Store last frame
                head = head->Next;


                // Write output to disk
	            if(frames<=FRAMES)
	                SaveFrame(ptempFrame, pCodecCtx->width, pCodecCtx->height, frames);


                //For performance metrics
                gettimeofday(&t2,NULL);

                time += ((t2.tv_sec-t1.tv_sec)*1000+(t2.tv_usec-t1.tv_usec)/1000);

                frames++;
            }
        }
    
        // Free the packet that was allocated by av_read_frame
        av_free_packet(&packet);
    }
  
    printf("Total time:\t%lums\nFrames:\t%d\n", time, frames);
    printf("Average time per frame: \t%lums\n", (time/frames));
    printf("Approximate memory alloc peak:\n(not including internal allocations by fftw or ffmpeg):\t%d KB\n", (bytes/1024));


    printf("Cleaning up\n");
    iterator = head;
    // Free the RGB image frames
    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        av_free(iterator->pFrame);
        iterator = iterator->Next;
    }
    av_free(ptempFrame);
    av_free(pBackground);
  
    // Free the YUV frame
    av_free(pFrame);

    // Free the buffers
    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        free(buffers[i]);
    }
    av_free(buffer3);
    av_free(buffer4);

    //free scratch space
    free(scratch);

    iterator = head;
    //free the Frame containers from circular buffer
    for(i = 0; i < CIRCULAR_FRAME_BUFFER_COUNT; i++)
    {
        free(iterator->Prev);
        iterator = iterator->Next;
    }
  
    // Close the codec
    avcodec_close(pCodecCtx);
  
    // Close the video file
    avformat_close_input(&pFormatCtx);

    printf("Cleaning complete\n");
  
    return 0;
}


