#include "Encode.h"

// Adepted from: https://stackoverflow.com/a/36488003 and encode_video example.

extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
  #include <libavutil/opt.h>
  #include <libswscale/swscale.h>
}

#pragma warning(disable : 4996) //deprected

#pragma comment (lib, "D:/Programming/Libraries/Lib/FFmpeg/avcodec.lib")
#pragma comment (lib, "D:/Programming/Libraries/Lib/FFmpeg/avutil.lib")

class Raw_Encoder_Data {
  shared_ptr<Filter_Resource_Output> _global;
  bool _ready = false;

  EncodeData _encoding;

  const AVCodec* codec;
  AVCodecContext* c = NULL;
  int i, ret, x, y;
  FILE* f;
  AVFrame* frame;
  AVPacket* pkt;
  uint8_t endcode[4] = { 0, 0, 1, 0xb7 };
  struct SwsContext* sws_context = NULL;
  uint8_t* rgb = NULL;
  GLubyte* pixels = NULL;
  int64_t nframes;

  void ffmpeg_encoder_set_frame_yuv_from_rgb(uint8_t* rgb) {
    const int in_linesize[1] = { 4 * c->width };
    sws_context = sws_getCachedContext(sws_context,
      c->width, c->height, AV_PIX_FMT_RGB32,
      c->width, c->height, AV_PIX_FMT_YUV420P,
      0, NULL, NULL, NULL);
    sws_scale(sws_context, (const uint8_t * const*)& rgb, in_linesize, 0,
      c->height, frame->data, frame->linesize);
  }

  void ffmpeg_encoder_glread_rgb(uint8_t** rgb, GLubyte** pixels, unsigned int width, unsigned int height) {
    size_t i, j, k, cur_gl, cur_rgb, nvals;
    const size_t format_nchannels = 4;
    nvals = format_nchannels * width * height;
    *pixels = (GLubyte*)realloc(*pixels, nvals * sizeof(GLubyte));
    *rgb = (GLubyte*)realloc(*rgb, nvals * sizeof(uint8_t));
    // Get RGBA to align to 32 bits instead of just 24 for RGB. May be faster for FFmpeg. 
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, *pixels);
    for (i = 0; i < height; i++) {
      for (j = 0; j < width; j++) {
        cur_gl = format_nchannels * (width * (height - i - 1) + j);
        cur_rgb = format_nchannels * (width * i + j);
        for (k = 0; k < format_nchannels; k++)
          (*rgb)[cur_rgb + k] = (*pixels)[cur_gl + k];
      }
    }
  }

  void encode(AVCodecContext* enc_ctx, AVFrame* frame, AVPacket* pkt,
    FILE* outfile) {
    int ret;

    // send the frame to the encoder
    if (frame) {
      //printf("Send frame %3lld\n", frame->pts);
    }

    ret = avcodec_send_frame(enc_ctx, frame);
    if (ret < 0) {
      fprintf(stderr, "Error sending a frame for encoding\n");
      exit(1);
    }

    while (ret >= 0) {
      ret = avcodec_receive_packet(enc_ctx, pkt);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return;
      }
      else if (ret < 0) {
        fprintf(stderr, "Error during encoding\n");
        exit(1);
      }

      //printf("Write packet %3lld (size=%5d)\n", pkt->pts, pkt->size);
      fwrite(pkt->data, 1, pkt->size, outfile);
      av_packet_unref(pkt);
    }
  }

public:

  Raw_Encoder_Data(shared_ptr<Filter_Resource_Output> global, EncodeData encoding) {
    cout << "Starting encoder" << endl;

    _global = global;
    shared_ptr<EditorContext> ed_ctx = _global->_filter.lock()->_ctx.lock();
    shared_ptr<Filter_Resource_RenderBuffer> colbuf = _global->_res->castTo<Filter_Resource_RenderBuffer>();
    assert(colbuf->_col_att_ids[0] == 0);
    _encoding = encoding;

    avcodec_register_all();

    // Enumerate the codecs
    /*AVCodec* codec = av_codec_next(NULL);
    while (codec != NULL) {
      fprintf(stderr, "%s : %s\n", codec->name, codec->long_name);
      codec = av_codec_next(codec);
    }*/

    // find the mpeg1video encoder 
    codec = avcodec_find_encoder_by_name(_encoding.encoder.c_str());
    //codec = avcodec_find_encoder(AVCodecID::AV_CODEC_ID_H264);
    if (!codec) {
      fprintf(stderr, "Codec '%s' not found\n", _encoding.encoder.c_str());
      exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
      fprintf(stderr, "Could not allocate video codec context\n");
      exit(1);
    }

    pkt = av_packet_alloc();
    if (!pkt)
      exit(1);

    // put sample parameters 
    c->bit_rate = _encoding.bitrate;
    // resolution must be a multiple of two 
    c->width = ed_ctx->_stream_desc._resolution.x;
    c->height = ed_ctx->_stream_desc._resolution.y;
    // frames per second 
    c->time_base = AVRational{ 1, ed_ctx->_stream_desc._frameRate };
    c->framerate = AVRational{ ed_ctx->_stream_desc._frameRate, 1 };

    // emit one intra frame every ten frames
    // check frame pict_type before passing frame
    // to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
    // then gop_size is ignored and the output of encoder
    // will always be I frame irrespective to gop_size
    //
    c->gop_size = 10;
    c->max_b_frames = 1;
    c->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec->id == AV_CODEC_ID_H264)
      av_opt_set(c->priv_data, "preset", "slow", 0);

    // open it
    ret = avcodec_open2(c, codec, NULL);
    if (ret < 0) {
      char errcode[AV_ERROR_MAX_STRING_SIZE]{0};
      av_make_error_string(errcode, AV_ERROR_MAX_STRING_SIZE, ret);
      fprintf(stderr, "Could not open codec: %s\n", errcode);
      exit(1);
    }

    fopen_s(&f, _encoding.filename.c_str(), "wb");
    if (!f) {
      fprintf(stderr, "Could not open %s\n", _encoding.filename.c_str());
      exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
      fprintf(stderr, "Could not allocate video frame\n");
      exit(1);
    }
    frame->format = c->pix_fmt;
    frame->width = c->width;
    frame->height = c->height;

    ret = av_frame_get_buffer(frame, 32);
    if (ret < 0) {
      fprintf(stderr, "Could not allocate the video frame data\n");
      exit(1);
    }

    nframes = 0;

    _ready = true;
  }

  void addFrame() {
    shared_ptr<EditorContext> ed_ctx = _global->_filter.lock()->_ctx.lock();
    shared_ptr<Filter_Resource_RenderBuffer> colbuf = _global->_res->castTo<Filter_Resource_RenderBuffer>();
    assert(_ready);

    glBindFramebuffer(GL_FRAMEBUFFER, colbuf->_source->_id);

    ffmpeg_encoder_glread_rgb(&rgb, &pixels, ed_ctx->_stream_desc._resolution.x, ed_ctx->_stream_desc._resolution.y);
    ffmpeg_encoder_set_frame_yuv_from_rgb(rgb);
    frame->pts = nframes;
    ++nframes;
    // encode the image
    encode(c, frame, pkt, f);
  }

  void finish() {
    _ready = false;


    // flush the encoder
    encode(c, NULL, pkt, f);

    // add sequence end code to have a real MPEG file
    fwrite(endcode, 1, sizeof(endcode), f);
    fclose(f);

    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    cout << "Closing video output" << endl;
  }
};

VideoEncoder::VideoEncoder(shared_ptr<Filter_Resource_Output> data, EncodeData encoding) {
  _encoder = make_shared<Raw_Encoder_Data>(data, encoding);
  //_encoder->init(data->_filename, AVCodecID::AV_CODEC_ID_H264, data->_frameRate, data->_resolution.x, data->_resolution.y);

}

void VideoEncoder::addFrame() {
  _encoder->addFrame();
}

void VideoEncoder::finish() {
  _encoder->finish();
}


