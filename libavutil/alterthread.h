#ifndef ALTERTHREAD_H
#define ALTERTHREAD_H

#include "unistd.h"
#include "libavcodec/avcodec.h"
#include "thread.h" 
#include <x264.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 7890

#include <string.h>
double crf_to_bitrate(float crf);
float bitrate_to_crf(double bitrate);
void* change_param(void *param); 
void server_init(void *param);
/*
class alerthread
{
public:
    alerthread();
};
*/
typedef struct X264Context {
    AVClass        *class;
    x264_param_t    params;
    x264_t         *enc;
    x264_picture_t  pic;
    uint8_t        *sei;
    int             sei_size;
    char *preset;
    char *tune;
    char *profile;
    char *level;
    int fastfirstpass;
    char *wpredp;
    char *x264opts;
    float crf;
    float crf_max;
    int cqp;
    int aq_mode;
    float aq_strength;
    char *psy_rd;
    int psy;
    int rc_lookahead;
    int weightp;
    int weightb;
    int ssim;
    int intra_refresh;
    int bluray_compat;
    int b_bias;
    int b_pyramid;
    int mixed_refs;
    int dct8x8;
    int fast_pskip;
    int aud;
    int mbtree;
    char *deblock;
    float cplxblur;
    char *partitions;
    int direct_pred;
    int slice_max_size;
    char *stats;
    int nal_hrd;
    int avcintra_class;
    int motion_est;
    int forced_idr;
    int coder;
    int a53_cc;
    int b_frame_strategy;
    int chroma_offset;
    int scenechange_threshold;
    int noise_reduction;

    char *x264_params;

    int nb_reordered_opaque, next_reordered_opaque;
    int64_t *reordered_opaque;
} X264Context;
#endif // ALTERTHREAD_H
