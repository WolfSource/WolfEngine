use super::ffi::av_frame::{
    w_av_frame, w_av_frame_convert, w_av_frame_fini, w_av_frame_init,
    w_av_get_required_buffer_size, W_MAX_PATH,
};
use anyhow::{bail, Result};

#[derive(Clone)]
pub enum AVPixelFormat {
    /// planar YUV 4:2:0, 12bpp, (1 Cr & Cb sample per 2x2 Y samples)
    YUV420P = 0,
    /// packed YUV 4:2:2, 16bpp, Y0 Cb Y1 Cr
    YUYV422,
    /// packed RGB 8:8:8, 24bpp, RGBRGB...
    RGB24,
    /// packed RGB 8:8:8, 24bpp, BGRBGR...   
    BGR24,
    /// planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    YUV422P,
    /// planar YUV 4:4:4, 24bpp, (1 Cr & Cb sample per 1x1 Y samples)
    YUV444P,
    /// planar YUV 4:1:0,  9bpp, (1 Cr & Cb sample per 4x4 Y samples)
    YUV410P,
    /// planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples)
    YUV411P,
    ///        Y        ,  8bpp
    GRAY8,
    ///        Y        ,  1bpp, 0 is white, 1 is black, in each byte pixels are ordered from the msb to the lsb
    MONOWHITE,
    ///        Y        ,  1bpp, 0 is black, 1 is white, in each byte pixels are ordered from the msb to the lsb
    MONOBLACK,
    /// 8 bits with AV_PIX_FMT_RGB32 palette
    PAL8,
    /// planar YUV 4:2:0, 12bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV420P and setting color_range     
    YUVJ420P,
    /// planar YUV 4:2:2, 16bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV422P and setting color_range
    YUVJ422P,
    /// planar YUV 4:4:4, 24bpp, full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV444P and setting color_range
    YUVJ444P,
    /// packed YUV 4:2:2, 16bpp, Cb Y0 Cr Y1
    UYVY422,
    /// packed YUV 4:1:1, 12bpp, Cb Y0 Y1 Cr Y2 Y3
    UYYVYY411,
    /// packed RGB 3:3:2,  8bpp, (msb)2B 3G 3R(lsb)
    BGR8,
    /// packed RGB 1:2:1 bitstream,  4bpp, (msb)1B 2G 1R(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    BGR4,
    /// packed RGB 1:2:1,  8bpp, (msb)1B 2G 1R(lsb)
    BGR4BYTE,
    /// packed RGB 3:3:2,  8bpp, (msb)2R 3G 3B(lsb)
    RGB8,
    /// packed RGB 1:2:1 bitstream,  4bpp, (msb)1R 2G 1B(lsb), a byte contains two pixels, the first pixel in the byte is the one composed by the 4 msb bits
    RGB4,
    /// packed RGB 1:2:1,  8bpp, (msb)1R 2G 1B(lsb)
    RGB4BYTE,
    /// planar YUV 4:2:0, 12bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    NV12,
    /// as above, but U and V bytes are swapped
    NV21,
    /// packed ARGB 8:8:8:8, 32bpp, ARGBARGB...
    ARGB,
    /// packed RGBA 8:8:8:8, 32bpp, RGBARGBA...
    RGBA,
    /// packed ABGR 8:8:8:8, 32bpp, ABGRABGR...
    ABGR,
    /// packed BGRA 8:8:8:8, 32bpp, BGRABGRA...
    BGRA,
    ///        Y        , 16bpp, big-endian
    GRAY16BE,
    ///        Y        , 16bpp, little-endian
    GRAY16LE,
    /// planar YUV 4:4:0 (1 Cr & Cb sample per 1x2 Y samples)
    YUV440P,
    /// planar YUV 4:4:0 full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV440P and setting color_range
    YUVJ440P,
    /// planar YUV 4:2:0, 20bpp, (1 Cr & Cb sample per 2x2 Y & A samples)
    YUVA420P,
    /// packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as big-endian
    RGB48BE,
    /// packed RGB 16:16:16, 48bpp, 16R, 16G, 16B, the 2-byte value for each R/G/B component is stored as little-endian
    RGB48LE,
    /// packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), big-endian
    RGB565BE,
    /// packed RGB 5:6:5, 16bpp, (msb)   5R 6G 5B(lsb), little-endian
    RGB565LE,
    /// packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), big-endian   , X=unused/undefined
    RGB555BE,
    /// packed RGB 5:5:5, 16bpp, (msb)1X 5R 5G 5B(lsb), little-endian, X=unused/undefined
    RGB555LE,
    /// packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), big-endian
    BGR565BE,
    /// packed BGR 5:6:5, 16bpp, (msb)   5B 6G 5R(lsb), little-endian
    BGR565LE,
    /// packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), big-endian   , X=unused/undefined
    BGR555BE,
    /// packed BGR 5:5:5, 16bpp, (msb)1X 5B 5G 5R(lsb), little-endian, X=unused/undefined
    BGR555LE,
    /// Hardware acceleration through VA-API, data[3] contains a VASurfaceID.
    VAAPI,
    /// planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    YUV420P16LE,
    /// planar YUV 4:2:0, 24bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    YUV420P16BE,
    /// planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    YUV422P16LE,
    /// planar YUV 4:2:2, 32bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    YUV422P16BE,
    /// planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    YUV444P16LE,
    /// planar YUV 4:4:4, 48bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    YUV444P16BE,
    /// HW decoding through DXVA2, Picture.data[3] contains a LPDIRECT3DSURFACE9 pointer
    DXVA2VLD,
    /// packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), little-endian, X=unused/undefined
    RGB444LE,
    /// packed RGB 4:4:4, 16bpp, (msb)4X 4R 4G 4B(lsb), big-endian,    X=unused/undefined
    RGB444BE,
    /// packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), little-endian, X=unused/undefined
    BGR444LE,
    /// packed BGR 4:4:4, 16bpp, (msb)4X 4B 4G 4R(lsb), big-endian,    X=unused/undefined
    BGR444BE,
    /// 8 bits gray, 8 bits alpha
    /// Y400A is alias for YA8
    /// GRAY8A is alias for YA8
    YA8,
    /// packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as big-endian
    BGR48BE,
    /// packed RGB 16:16:16, 48bpp, 16B, 16G, 16R, the 2-byte value for each R/G/B component is stored as little-endian
    BGR48LE,

    /*
        The following 12 formats have the disadvantage of needing 1 format for each bit depth.
        Notice that each 9/10 bits sample is stored in 16 bits with extra padding.
        If you want to support multiple bit depths, then using YUV420P16* with the bpp stored separately is better.
    */
    /// planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    YUV420P9BE,
    /// planar YUV 4:2:0, 13.5bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    YUV420P9LE,
    /// planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    YUV420P10BE,
    /// planar YUV 4:2:0, 15bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    YUV420P10LE,
    /// planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    YUV422P10BE,
    /// planar YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    YUV422P10LE,
    /// planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    YUV444P9BE,
    /// planar YUV 4:4:4, 27bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    YUV444P9LE,
    /// planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    YUV444P10BE,
    /// planar YUV 4:4:4, 30bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    YUV444P10LE,
    /// planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    YUV422P9BE,
    /// planar YUV 4:2:2, 18bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    YUV422P9LE,
    /// planar GBR 4:4:4 24bpp
    /// GBR24P is alias for GBRP
    GBRP,
    /// planar GBR 4:4:4 27bpp, big-endian
    GBRP9BE,
    /// planar GBR 4:4:4 27bpp, little-endian
    GBRP9LE,
    /// planar GBR 4:4:4 30bpp, big-endian
    GBRP10BE,
    /// planar GBR 4:4:4 30bpp, little-endian
    GBRP10LE,
    /// planar GBR 4:4:4 48bpp, big-endian
    GBRP16BE,
    /// planar GBR 4:4:4 48bpp, little-endian
    GBRP16LE,
    /// planar YUV 4:2:2 24bpp, (1 Cr & Cb sample per 2x1 Y & A samples)
    YUVA422P,
    /// planar YUV 4:4:4 32bpp, (1 Cr & Cb sample per 1x1 Y & A samples)
    YUVA444P,
    /// planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), big-endian
    YUVA420P9BE,
    /// planar YUV 4:2:0 22.5bpp, (1 Cr & Cb sample per 2x2 Y & A samples), little-endian
    YUVA420P9LE,
    /// planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), big-endian
    YUVA422P9BE,
    /// planar YUV 4:2:2 27bpp, (1 Cr & Cb sample per 2x1 Y & A samples), little-endian
    YUVA422P9LE,
    /// planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    YUVA444P9BE,
    /// planar YUV 4:4:4 36bpp, (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    YUVA444P9LE,
    /// planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    YUVA420P10BE,
    /// planar YUV 4:2:0 25bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    YUVA420P10LE,
    /// planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    YUVA422P10BE,
    /// planar YUV 4:2:2 30bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    YUVA422P10LE,
    /// planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    YUVA444P10BE,
    /// planar YUV 4:4:4 40bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    YUVA444P10LE,
    /// planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, big-endian)
    YUVA420P16BE,
    /// planar YUV 4:2:0 40bpp, (1 Cr & Cb sample per 2x2 Y & A samples, little-endian)
    YUVA420P16LE,
    /// planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, big-endian)
    YUVA422P16BE,
    /// planar YUV 4:2:2 48bpp, (1 Cr & Cb sample per 2x1 Y & A samples, little-endian)
    YUVA422P16LE,
    /// planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, big-endian)
    YUVA444P16BE,
    /// planar YUV 4:4:4 64bpp, (1 Cr & Cb sample per 1x1 Y & A samples, little-endian)
    YUVA444P16LE,
    /// HW acceleration through VDPAU, Picture.data[3] contains a VdpVideoSurface
    VDPAU,
    /// packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as little-endian, the 4 lower bits are set to 0
    XYZ12LE,
    /// packed XYZ 4:4:4, 36 bpp, (msb) 12X, 12Y, 12Z (lsb), the 2-byte value for each X/Y/Z is stored as big-endian, the 4 lower bits are set to 0
    XYZ12BE,
    /// interleaved chroma YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
    NV16,
    /// interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    NV20LE,
    /// interleaved chroma YUV 4:2:2, 20bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    NV20BE,
    /// packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    RGBA64BE,
    /// packed RGBA 16:16:16:16, 64bpp, 16R, 16G, 16B, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    RGBA64LE,
    /// packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as big-endian
    BGRA64BE,
    /// packed RGBA 16:16:16:16, 64bpp, 16B, 16G, 16R, 16A, the 2-byte value for each R/G/B/A component is stored as little-endian
    BGRA64LE,
    /// packed YUV 4:2:2, 16bpp, Y0 Cr Y1 Cb
    YVYU422,
    /// 16 bits gray, 16 bits alpha (big-endian)
    YA16BE,
    /// 16 bits gray, 16 bits alpha (little-endian)
    YA16LE,
    /// planar GBRA 4:4:4:4 32bpp
    GBRAP,
    /// planar GBRA 4:4:4:4 64bpp, big-endian
    GBRAP16BE,
    /// planar GBRA 4:4:4:4 64bpp, little-endian
    GBRAP16LE,
    ///HW acceleration through QSV, data[3] contains a pointer to the mfxFrameSurface1 structure.
    QSV,
    ///HW acceleration though MMAL, data[3] contains a pointer to the MMAL_BUFFER_HEADER_T structure.
    MMAL,
    /// HW decoding through Direct3D11 via old API, Picture.data[3] contains a ID3D11VideoDecoderOutputView pointer
    D3D11VAVLD,
    ///HW acceleration through CUDA. data[i] contain CUdeviceptr pointers exactly as for system memory frames.
    CUDA,
    /// packed RGB 8:8:8, 32bpp, XRGBXRGB...   X=unused/undefined
    ZERORGB,
    /// packed RGB 8:8:8, 32bpp, RGBXRGBX...   X=unused/undefined   
    RGB0,
    /// packed BGR 8:8:8, 32bpp, XBGRXBGR...   X=unused/undefined
    ZEROBGR,
    /// packed BGR 8:8:8, 32bpp, BGRXBGRX...   X=unused/undefined   
    BGR0,
    /// planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    YUV420P12BE,
    /// planar YUV 4:2:0,18bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    YUV420P12LE,
    /// planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), big-endian
    YUV420P14BE,
    /// planar YUV 4:2:0,21bpp, (1 Cr & Cb sample per 2x2 Y samples), little-endian
    YUV420P14LE,
    /// planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    YUV422P12BE,
    /// planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    YUV422P12LE,
    /// planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), big-endian
    YUV422P14BE,
    /// planar YUV 4:2:2,28bpp, (1 Cr & Cb sample per 2x1 Y samples), little-endian
    YUV422P14LE,
    /// planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    YUV444P12BE,
    /// planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    YUV444P12LE,
    /// planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), big-endian
    YUV444P14BE,
    /// planar YUV 4:4:4,42bpp, (1 Cr & Cb sample per 1x1 Y samples), little-endian
    YUV444P14LE,
    /// planar GBR 4:4:4 36bpp, big-endian
    GBRP12BE,
    /// planar GBR 4:4:4 36bpp, little-endian
    GBRP12LE,
    /// planar GBR 4:4:4 42bpp, big-endian
    GBRP14BE,
    /// planar GBR 4:4:4 42bpp, little-endian
    GBRP14LE,
    /// planar YUV 4:1:1, 12bpp, (1 Cr & Cb sample per 4x1 Y samples) full scale (JPEG), deprecated in favor of AV_PIX_FMT_YUV411P and setting color_range
    YUVJ411P,
    /// bayer, BGBG..(odd line), GRGR..(even line), 8-bit samples
    BAYERBGGR8,
    /// bayer, RGRG..(odd line), GBGB..(even line), 8-bit samples
    BAYERRGGB8,
    /// bayer, GBGB..(odd line), RGRG..(even line), 8-bit samples
    BAYERGBRG8,
    /// bayer, GRGR..(odd line), BGBG..(even line), 8-bit samples
    BAYERGRBG8,
    /// bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, little-endian
    BAYERBGGR16LE,
    /// bayer, BGBG..(odd line), GRGR..(even line), 16-bit samples, big-endian
    BAYERBGGR16BE,
    /// bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, little-endian
    BAYERRGGB16LE,
    /// bayer, RGRG..(odd line), GBGB..(even line), 16-bit samples, big-endian
    BAYERRGGB16BE,
    /// bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, little-endian
    BAYERGBRG16LE,
    /// bayer, GBGB..(odd line), RGRG..(even line), 16-bit samples, big-endian
    BAYERGBRG16BE,
    /// bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, little-endian
    BAYERGRBG16LE,
    /// bayer, GRGR..(odd line), BGBG..(even line), 16-bit samples, big-endian
    BAYERGRBG16BE,
    /// XVideo Motion Acceleration via common packet passing
    XVMC,
    /// planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    YUV440P10LE,
    /// planar YUV 4:4:0,20bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    YUV440P10BE,
    /// planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), little-endian
    YUV440P12LE,
    /// planar YUV 4:4:0,24bpp, (1 Cr & Cb sample per 1x2 Y samples), big-endian
    YUV440P12BE,
    /// packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), little-endian
    AYUV64LE,
    /// packed AYUV 4:4:4,64bpp (1 Cr & Cb sample per 1x1 Y & A samples), big-endian
    AYUV64BE,
    /// hardware decoding through Videotoolbox
    VIDEOTOOLBOX,
    /// like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, little-endian
    P010LE,
    /// like NV12, with 10bpp per component, data in the high bits, zeros in the low bits, big-endian
    P010BE,
    /// planar GBR 4:4:4:4 48bpp, big-endian
    GBRAP12BE,
    /// planar GBR 4:4:4:4 48bpp, little-endian
    GBRAP12LE,
    /// planar GBR 4:4:4:4 40bpp, big-endian
    GBRAP10BE,
    /// planar GBR 4:4:4:4 40bpp, little-endian
    GBRAP10LE,
    /// hardware decoding through MediaCodec
    MEDIACODEC,
    ///        Y        , 12bpp, big-endian
    GRAY12BE,
    ///        Y        , 12bpp, little-endian
    GRAY12LE,
    ///        Y        , 10bpp, big-endian
    GRAY10BE,
    ///        Y        , 10bpp, little-endian
    GRAY10LE,
    /// like NV12, with 16bpp per component, little-endian
    P016LE,
    /// like NV12, with 16bpp per component, big-endian
    P016BE,
    /// Hardware surfaces for Direct3D11.
    /// This is preferred over the legacy AV_PIX_FMT_D3D11VA_VLD. The new D3D11
    /// hwaccel API and filtering support AV_PIX_FMT_D3D11 only.
    /// data[0] contains a ID3D11Texture2D pointer, and data[1] contains the
    /// texture array index of the frame as intptr_t if the ID3D11Texture2D is
    /// an array texture (or always 0 if it's a normal texture).
    D3D11,
    ///        Y        , 9bpp, big-endian
    GRAY9BE,
    ///        Y        , 9bpp, little-endian
    GRAY9LE,
    /// IEEE-754 single precision planar GBR 4:4:4,     96bpp, big-endian
    GBRPF32BE,
    /// IEEE-754 single precision planar GBR 4:4:4,     96bpp, little-endian
    GBRPF32LE,
    /// IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, big-endian
    GBRAPF32BE,
    /// IEEE-754 single precision planar GBRA 4:4:4:4, 128bpp, little-endian
    GBRAPF32LE,
    /// DRM-managed buffers exposed through PRIME buffer sharing.
    /// data[0] points to an AVDRMFrameDescriptor.
    DRMPRIME,
    /// Hardware surfaces for OpenCL.
    /// data[i] contain 2D image objects (typed in C as cl_mem, used
    /// in OpenCL as image2d_t) for each plane of the surface.
    OPENCL,
    ///        Y        , 14bpp, big-endian
    GRAY14BE,
    ///        Y        , 14bpp, little-endian
    GRAY14LE,
    /// IEEE-754 single precision Y, 32bpp, big-endian
    GRAYF32BE,
    /// IEEE-754 single precision Y, 32bpp, little-endian
    GRAYF32LE,
    /// planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, big-endian
    YUVA422P12BE,
    /// planar YUV 4:2:2,24bpp, (1 Cr & Cb sample per 2x1 Y samples), 12b alpha, little-endian
    YUVA422P12LE,
    /// planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, big-endian
    YUVA444P12BE,
    /// planar YUV 4:4:4,36bpp, (1 Cr & Cb sample per 1x1 Y samples), 12b alpha, little-endian
    YUVA444P12LE,
    /// planar YUV 4:4:4, 24bpp, 1 plane for Y and 1 plane for the UV components, which are interleaved (first byte U and the following byte V)
    NV24,
    /// as above, but U and V bytes are swapped
    NV42,
    /// Vulkan hardware images. data[0] points to an AVVkFrame
    VULKAN,
    /// packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, big-endian
    Y210BE,
    /// packed YUV 4:2:2 like YUYV422, 20bpp, data in the high bits, little-endian
    Y210LE,
    /// packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), little-endian, X=unused/undefined
    X2RGB10LE,
    /// packed RGB 10:10:10, 30bpp, (msb)2X 10R 10G 10B(lsb), big-endian, X=unused/undefined
    X2RGB10BE,
    /// packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), little-endian, X=unused/undefined
    X2BGR10LE,
    /// packed BGR 10:10:10, 30bpp, (msb)2X 10B 10G 10R(lsb), big-endian, X=unused/undefined
    X2BGR10BE,
    /// interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, big-endian
    P210BE,
    /// interleaved chroma YUV 4:2:2, 20bpp, data in the high bits, little-endian
    P210LE,
    /// interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, big-endian
    P410BE,
    /// interleaved chroma YUV 4:4:4, 30bpp, data in the high bits, little-endian
    P410LE,
    /// interleaved chroma YUV 4:2:2, 32bpp, big-endian
    P216BE,
    /// interleaved chroma YUV 4:2:2, 32bpp, little-endian
    P216LE,
    /// interleaved chroma YUV 4:4:4, 48bpp, big-endian
    P416BE,
    /// interleaved chroma YUV 4:4:4, 48bpp, little-endian
    P416LE,
    NB = 206,
}

#[derive(Clone)]
pub struct AVFrame {
    pub ctx: w_av_frame,
    pub pixel_format: AVPixelFormat,
    pub width: u32,
    pub height: u32,
    pub align: u32,
}

impl Drop for AVFrame {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_av_frame_fini(&mut self.ctx) };
        }
    }
}

impl AVFrame {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new(
        p_pixel_format: AVPixelFormat,
        p_width: u32,
        p_height: u32,
        p_alignment: u32,
        p_data: &mut [u8],
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            pixel_format: p_pixel_format,
            width: p_width,
            height: p_height,
            align: p_alignment,
        };

        // create a buffer for error
        let mut err = [1i8; W_MAX_PATH as usize];
        let err_ptr = err.as_mut_ptr();

        unsafe {
            let ret = w_av_frame_init(
                &mut obj.ctx,
                obj.pixel_format as u32,
                obj.width,
                obj.height,
                obj.align,
                p_data.as_mut_ptr(),
                err_ptr,
            );

            match ret {
                0 => Ok(obj),
                _ => {
                    let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
                    let str = c_err_str.to_str().unwrap_or_default();
                    bail!(
                        "could not create av_frame object because {}. trace: {:?}",
                        String::from(str),
                        std::backtrace::Backtrace::force_capture()
                    )
                }
            }
        }
    }

    pub fn get_required_buffer_size(
        p_pixel_format: AVPixelFormat,
        p_width: u32,
        p_height: u32,
        p_alignment: u32,
    ) -> ::std::os::raw::c_int {
        unsafe {
            w_av_get_required_buffer_size(p_pixel_format as u32, p_width, p_height, p_alignment)
        }
    }

    /// # Errors
    ///
    /// TODO: add error description
    pub fn convert(&self, p_dst: &mut AVFrame) -> Result<()> {
        // create a buffer for error
        let mut err = [1i8; W_MAX_PATH as usize];
        let err_ptr = err.as_mut_ptr();

        unsafe {
            let ret = w_av_frame_convert(self.ctx, &mut p_dst.ctx, err_ptr);
            match ret {
                0 => Ok(()),
                _ => {
                    let c_err_str = std::ffi::CStr::from_ptr(err_ptr);
                    let str = c_err_str.to_str().unwrap_or_default();
                    bail!(
                        "could not create av_frame object because {}. trace: {:?}",
                        String::from(str),
                        std::backtrace::Backtrace::force_capture()
                    )
                }
            }
        }
    }
}
