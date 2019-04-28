#include "Decode.h"

// Adepted from: https://stackoverflow.com/a/36488003 and decode_video example.

/*extern "C"
{
  #include <libavcodec/avcodec.h>
  #include <libavutil/imgutils.h>
  #include <libavutil/opt.h>
  #include <libswscale/swscale.h>
}

#pragma warning(disable : 4996) //deprected

#pragma comment (lib, "D:/Programming/Libraries/Lib/FFmpeg/avcodec.lib")
#pragma comment (lib, "D:/Programming/Libraries/Lib/FFmpeg/avutil.lib")


#define INBUF_SIZE 4096

class Raw_Decoder_Data {
  string _filename;

  //NOTE: This is your output format, you could
//change it to YUV or whatever you want and it
//should still work, I think the definitions are
//in libavcodec.h or libavformat.h
 
  const char* filename, * outfilename;
  const AVCodec* codec;
  AVCodecParserContext* parser;
  AVCodecContext* c = NULL;
  FILE* f;
  AVFrame* frame;
  uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
  uint8_t* data;
  size_t   data_size;
  int ret;
  AVPacket* pkt;


  void init() {
    pkt = av_packet_alloc();
    if (!pkt)
      exit(1);

    // set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams)
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    // find the MPEG-1 video decoder
    codec = avcodec_find_decoder(AV_CODEC_ID_MPEG1VIDEO);
    if (!codec) {
      fprintf(stderr, "Codec not found\n");
      exit(1);
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
      fprintf(stderr, "parser not found\n");
      exit(1);
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
      fprintf(stderr, "Could not allocate video codec context\n");
      exit(1);
    }

    // For some codecs, such as msmpeg4 and mpeg4, width and height
    // MUST be initialized there because this information is not
    // available in the bitstream.

    // open it
    if (avcodec_open2(c, codec, NULL) < 0) {
      fprintf(stderr, "Could not open codec\n");
      exit(1);
    }

    f = fopen(filename, "rb");
    if (!f) {
      fprintf(stderr, "Could not open %s\n", filename);
      exit(1);
    }

    frame = av_frame_alloc();
    if (!frame) {
      fprintf(stderr, "Could not allocate video frame\n");
      exit(1);
    }

  }

  void decode(AVCodecContext* dec_ctx, AVFrame* frame, AVPacket* pkt,
    const char* filename) {
    char buf[1024];
    int ret;

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
      fprintf(stderr, "Error sending a packet for decoding\n");
      exit(1);
    }

    while (ret >= 0) {
      ret = avcodec_receive_frame(dec_ctx, frame);
      if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
        return;
      else if (ret < 0) {
        fprintf(stderr, "Error during decoding\n");
        exit(1);
      }

      printf("saving frame %3d\n", dec_ctx->frame_number);
      fflush(stdout);

      // the picture is allocated by the decoder. no need to
      // free it
      snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
      pgm_save(frame->data[0], frame->linesize[0],
        frame->width, frame->height, buf);
    }
  }

  void pollFrame() {

  }

  void cleanup() {

  }

public:
  iVec2 resolution;

  Raw_Decoder_Data(std::string filename) {
    _filename = filename;
    init();
  }

  ~Raw_Decoder_Data() {
    cleanup();
  }


};

VideoDecoder(std::weak_ptr<EditorContext> ctx, std::string filename) : Filter(ctx) {
  staticInit();

  _col = 0x40ff00ff;

}

void VideoDecoder::init() {
  weak_ptr<Filter> me = weak_from_this();
  _params.insert({ "resolution", make_shared<Filter_Resource_Input>(me, "resolution", "Resolution", "Resolution of output stream", Filter_Resource::Type_Object) });

  _outputs.insert({ "out", make_shared<Filter_Resource_ColorBuffer>(me, "out", "Out", "Output image", nullptr, 0) });
}

void VideoDecoder::configure() {
  shared_ptr<Filter_Resource_Object> res = _params["resolution"]->castTo<Filter_Resource_Object>();

  _img = make_shared<Raw_FrameBuffer>(fVec2(
    res->_elems["x"]->castTo<Filter_Resource_Float>()->_val,
    res->_elems["y"]->castTo<Filter_Resource_Float>()->_val
  ));
  _img->attachColor(0, GL_RGBA, GL_UNSIGNED_BYTE);
  assert(_img->valid());

  _outputs["out"]->castTo<Filter_Resource_ColorBuffer>()->_source = _img;
  _outputs["out"]->castTo<Filter_Resource_ColorBuffer>()->_col_att_id = 0;
}

void VideoDecoder::calculate(float t) {
  
}*/