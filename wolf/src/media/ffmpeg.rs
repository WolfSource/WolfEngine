// use super::ffi::ffmpeg::{w_ffmpeg, w_ffmpeg_fini};

use std::ptr::null_mut;

use super::{
    av_frame::AVFrame,
    ffi::ffmpeg::{w_ffmpeg, w_ffmpeg_fini, w_ffmpeg_init, AVCodeOptions, W_MAX_PATH},
};
use anyhow::{bail, Result};

#[allow(non_camel_case_types)]
#[allow(clippy::upper_case_acronyms)]
#[derive(Clone)]
pub enum AVCodecID {
    NONE,

    /* video codecs */
    MPEG1VIDEO,
    ///< preferred ID for MPEG-1/2 video decoding
    MPEG2VIDEO,
    H261,
    H263,
    RV10,
    RV20,
    MJPEG,
    MJPEGB,
    LJPEG,
    SP5X,
    JPEGLS,
    MPEG4,
    RAWVIDEO,
    MSMPEG4V1,
    MSMPEG4V2,
    MSMPEG4V3,
    WMV1,
    WMV2,
    H263P,
    H263I,
    FLV1,
    SVQ1,
    SVQ3,
    DVVIDEO,
    HUFFYUV,
    CYUV,
    H264,
    INDEO3,
    VP3,
    THEORA,
    ASV1,
    ASV2,
    FFV1,
    _4XM,
    VCR1,
    CLJR,
    MDEC,
    ROQ,
    INTERPLAY_VIDEO,
    XAN_WC3,
    XAN_WC4,
    RPZA,
    CINEPAK,
    WS_VQA,
    MSRLE,
    MSVIDEO1,
    IDCIN,
    _8BPS,
    SMC,
    FLIC,
    TRUEMOTION1,
    VMDVIDEO,
    MSZH,
    ZLIB,
    QTRLE,
    TSCC,
    ULTI,
    QDRAW,
    VIXL,
    QPEG,
    PNG,
    PPM,
    PBM,
    PGM,
    PGMYUV,
    PAM,
    FFVHUFF,
    RV30,
    RV40,
    VC1,
    WMV3,
    LOCO,
    WNV1,
    AASC,
    INDEO2,
    FRAPS,
    TRUEMOTION2,
    BMP,
    CSCD,
    MMVIDEO,
    ZMBV,
    AVS,
    SMACKVIDEO,
    NUV,
    KMVC,
    FLASHSV,
    CAVS,
    JPEG2000,
    VMNC,
    VP5,
    VP6,
    VP6F,
    TARGA,
    DSICINVIDEO,
    TIERTEXSEQVIDEO,
    TIFF,
    GIF,
    DXA,
    DNXHD,
    THP,
    SGI,
    C93,
    BETHSOFTVID,
    PTX,
    TXD,
    VP6A,
    AMV,
    VB,
    PCX,
    SUNRAST,
    INDEO4,
    INDEO5,
    MIMIC,
    RL2,
    ESCAPE124,
    DIRAC,
    BFI,
    CMV,
    MOTIONPIXELS,
    TGV,
    TGQ,
    TQI,
    AURA,
    AURA2,
    V210X,
    TMV,
    V210,
    DPX,
    MAD,
    FRWU,
    FLASHSV2,
    CDGRAPHICS,
    R210,
    ANM,
    BINKVIDEO,
    /// alsow known as IFF_BYTERUN1
    IFF_ILBM,
    KGV1,
    YOP,
    VP8,
    PICTOR,
    ANSI,
    A64_MULTI,
    A64_MULTI5,
    R10K,
    MXPEG,
    LAGARITH,
    PRORES,
    JV,
    DFA,
    WMV3IMAGE,
    VC1IMAGE,
    UTVIDEO,
    BMV_VIDEO,
    VBLE,
    DXTORY,
    V410,
    XWD,
    CDXL,
    XBM,
    ZEROCODEC,
    MSS1,
    MSA1,
    TSCC2,
    MTS2,
    CLLC,
    MSS2,
    VP9,
    AIC,
    ESCAPE130,
    G2M,
    WEBP,
    HNM4_VIDEO,
    /// also known as AV_CODEC_ID_H265
    HEVC,
    FIC,
    ALIAS_PIX,
    BRENDER_PIX,
    PAF_VIDEO,
    EXR,
    VP7,
    SANM,
    SGIRLE,
    MVC1,
    MVC2,
    HQX,
    TDSC,
    HQ_HQA,
    HAP,
    DDS,
    DXV,
    SCREENPRESSO,
    RSCC,
    AVS2,
    PGX,
    AVS3,
    MSP2,
    ///also known as AV_CODEC_ID_H266
    VVC,
    Y41P,
    AVRP,
    _012V,
    AVUI,
    AYUV,
    TARGA_Y216,
    V308,
    V408,
    YUV4,
    AVRN,
    CPIA,
    XFACE,
    SNOW,
    SMVJPEG,
    APNG,
    DAALA,
    CFHD,
    TRUEMOTION2RT,
    M101,
    MAGICYUV,
    SHEERVIDEO,
    YLC,
    PSD,
    PIXLET,
    SPEEDHQ,
    FMVC,
    SCPR,
    CLEARVIDEO,
    XPM,
    AV1,
    BITPACKED,
    MSCC,
    SRGC,
    SVG,
    GDV,
    FITS,
    IMM4,
    PROSUMER,
    MWSC,
    WCMV,
    RASC,
    HYMT,
    ARBC,
    AGM,
    LSCR,
    VP4,
    IMM5,
    MVDV,
    MVHA,
    CDTOONS,
    MV30,
    NOTCHLC,
    PFM,
    MOBICLIP,
    PHOTOCD,
    IPU,
    ARGO,
    CRI,
    SIMBIOSIS_IMX,
    SGA_VIDEO,
    GEM,
    VBN,
    JPEGXL,
    QOI,
    PHM,
    RADIANCE_HDR,

    /* various PCM "codecs" */
    /// FIRST_AUDIO is a dummy id pointing at the start of audio codecs
    PCM_S16LE = 0x10000,
    PCM_S16BE,
    PCM_U16LE,
    PCM_U16BE,
    PCM_S8,
    PCM_U8,
    PCM_MULAW,
    PCM_ALAW,
    PCM_S32LE,
    PCM_S32BE,
    PCM_U32LE,
    PCM_U32BE,
    PCM_S24LE,
    PCM_S24BE,
    PCM_U24LE,
    PCM_U24BE,
    PCM_S24DAUD,
    PCM_ZORK,
    PCM_S16LE_PLANAR,
    PCM_DVD,
    PCM_F32BE,
    PCM_F32LE,
    PCM_F64BE,
    PCM_F64LE,
    PCM_BLURAY,
    PCM_LXF,
    S302M,
    PCM_S8_PLANAR,
    PCM_S24LE_PLANAR,
    PCM_S32LE_PLANAR,
    PCM_S16BE_PLANAR,
    PCM_S64LE,
    PCM_S64BE,
    PCM_F16LE,
    PCM_F24LE,
    PCM_VIDC,
    PCM_SGA,

    /* various ADPCM codecs */
    ADPCM_IMA_QT = 0x11000,
    ADPCM_IMA_WAV,
    ADPCM_IMA_DK3,
    ADPCM_IMA_DK4,
    ADPCM_IMA_WS,
    ADPCM_IMA_SMJPEG,
    ADPCM_MS,
    ADPCM_4XM,
    ADPCM_XA,
    ADPCM_ADX,
    ADPCM_EA,
    ADPCM_G726,
    ADPCM_CT,
    ADPCM_SWF,
    ADPCM_YAMAHA,
    ADPCM_SBPRO_4,
    ADPCM_SBPRO_3,
    ADPCM_SBPRO_2,
    ADPCM_THP,
    ADPCM_IMA_AMV,
    ADPCM_EA_R1,
    ADPCM_EA_R3,
    ADPCM_EA_R2,
    ADPCM_IMA_EA_SEAD,
    ADPCM_IMA_EA_EACS,
    ADPCM_EA_XAS,
    ADPCM_EA_MAXIS_XA,
    ADPCM_IMA_ISS,
    ADPCM_G722,
    ADPCM_IMA_APC,
    ADPCM_VIMA,
    ADPCM_AFC,
    ADPCM_IMA_OKI,
    ADPCM_DTK,
    ADPCM_IMA_RAD,
    ADPCM_G726LE,
    ADPCM_THP_LE,
    ADPCM_PSX,
    ADPCM_AICA,
    ADPCM_IMA_DAT4,
    ADPCM_MTAF,
    ADPCM_AGM,
    ADPCM_ARGO,
    ADPCM_IMA_SSI,
    ADPCM_ZORK,
    ADPCM_IMA_APM,
    ADPCM_IMA_ALP,
    ADPCM_IMA_MTF,
    ADPCM_IMA_CUNNING,
    ADPCM_IMA_MOFLEX,
    ADPCM_IMA_ACORN,

    /* AMR */
    AMR_NB = 0x12000,
    AMR_WB,

    /* RealAudio codecs*/
    RA_144 = 0x13000,
    RA_288,

    /* various DPCM codecs */
    ROQ_DPCM = 0x14000,
    INTERPLAY_DPCM,
    XAN_DPCM,
    SOL_DPCM,
    SDX2_DPCM,
    GREMLIN_DPCM,
    DERF_DPCM,

    /* audio codecs */
    MP2 = 0x15000,
    /// preferred ID for decoding MPEG audio layer 1, 2 or 3
    MP3,
    AAC,
    AC3,
    DTS,
    VORBIS,
    DVAUDIO,
    WMAV1,
    WMAV2,
    MACE3,
    MACE6,
    VMDAUDIO,
    FLAC,
    MP3ADU,
    MP3ON4,
    SHORTEN,
    ALAC,
    WESTWOOD_SND1,
    /// as in Berlin toast format
    GSM,
    QDM2,
    COOK,
    TRUESPEECH,
    TTA,
    SMACKAUDIO,
    QCELP,
    WAVPACK,
    DSICINAUDIO,
    IMC,
    MUSEPACK7,
    MLP,
    /// as found in WAV
    GSM_MS,
    ATRAC3,
    APE,
    NELLYMOSER,
    MUSEPACK8,
    SPEEX,
    WMAVOICE,
    WMAPRO,
    WMALOSSLESS,
    ATRAC3P,
    EAC3,
    SIPR,
    MP1,
    TWINVQ,
    TRUEHD,
    MP4ALS,
    ATRAC1,
    BINKAUDIO_RDFT,
    BINKAUDIO_DCT,
    AAC_LATM,
    QDMC,
    CELT,
    G723_1,
    G729,
    _8SVX_EXP,
    _8SVX_FIB,
    BMV_AUDIO,
    RALF,
    IAC,
    ILBC,
    OPUS,
    COMFORT_NOISE,
    TAK,
    METASOUND,
    PAF_AUDIO,
    ON2AVC,
    DSS_SP,
    CODEC2,
    FFWAVESYNTH,
    SONIC,
    SONIC_LS,
    EVRC,
    SMV,
    DSD_LSBF,
    DSD_MSBF,
    DSD_LSBF_PLANAR,
    DSD_MSBF_PLANAR,
    _4GV,
    INTERPLAY_ACM,
    XMA1,
    XMA2,
    DST,
    ATRAC3AL,
    ATRAC3PAL,
    DOLBY_E,
    APTX,
    APTX_HD,
    SBC,
    ATRAC9,
    HCOM,
    ACELP_KELVIN,
    MPEGH_3D_AUDIO,
    SIREN,
    HCA,
    FASTAUDIO,
    MSNSIREN,
    DFPWM,

    /* subtitle codecs */
    /// FIRST_SUBTITLE is a dummy ID pointing at the start of subtitle codecs.
    DVD_SUBTITLE = 0x17000,
    DVB_SUBTITLE,
    /// raw UTF-8 text
    TEXT,
    XSUB,
    SSA,
    MOV_TEXT,
    HDMV_PGS_SUBTITLE,
    DVB_TELETEXT,
    SRT,
    MICRODVD,
    EIA_608,
    JACOSUB,
    SAMI,
    REALTEXT,
    STL,
    SUBVIEWER1,
    SUBVIEWER,
    SUBRIP,
    WEBVTT,
    MPL2,
    VPLAYER,
    PJS,
    ASS,
    HDMV_TEXT_SUBTITLE,
    TTML,
    ARIB_CAPTION,

    /* other specific kind of codecs (generally used for attachments) */
    /// FIRST_UNKNOWN is a dummy ID pointing at the start of various fake codecs.
    TTF = 0x18000,

    /// Contain timestamp estimated through PCR of program stream.
    SCTE_35,
    EPG,
    BINTEXT,
    XBIN,
    IDF,
    OTF,
    SMPTE_KLV,
    DVD_NAV,
    TIMED_ID3,
    BIN_DATA,

    /// codec_id is not known (like AV_CODEC_ID_NONE) but lavf should attempt to identify it
    PROBE = 0x19000,
    /// _FAKE_ codec to indicate a raw MPEG-2 TS stream (only used by libavformat)
    MPEG2TS = 0x20000,
    /// _FAKE_ codec to indicate a MPEG-4 Systems stream (only used by libavformat)
    MPEG4SYSTEMS = 0x20001,
    /// Dummy codec for streams containing only metadata information
    FFMETADATA = 0x21000,
    /// Passthrough codec, AVFrames wrapped in AVPacket
    WRAPPED_AVFRAME = 0x21001,
}

#[derive(Clone)]
pub enum FFmpegMode {
    ENCODER = 0,
    DECODER,
}

#[derive(Clone)]
pub struct FFmpeg {
    pub ctx: w_ffmpeg,
    pub mode: FFmpegMode,
    pub codec_id: AVCodecID,
    pub codec_opt: AVCodeOptions,
}

impl Drop for FFmpeg {
    fn drop(&mut self) {
        if !self.ctx.is_null() {
            unsafe { w_ffmpeg_fini(&mut self.ctx) };
        }
    }
}

impl FFmpeg {
    /// # Errors
    ///
    /// TODO: add error description
    pub fn new(
        p_av_frame: &AVFrame,
        p_mode: FFmpegMode,
        p_codec_id: AVCodecID,
        p_codec_opt: AVCodeOptions,
    ) -> Result<Self> {
        let mut obj = Self {
            ctx: std::ptr::null_mut(),
            mode: p_mode,
            codec_id: p_codec_id,
            codec_opt: p_codec_opt,
        };

        // create a buffer for error
        let mut buf = [1i8; W_MAX_PATH as usize];
        let buf_ptr = buf.as_mut_ptr();

        unsafe {
            let ret = w_ffmpeg_init(
                &mut obj.ctx,
                p_av_frame.ctx,
                obj.mode as u32,
                obj.codec_id as u32,
                &mut obj.codec_opt,
                null_mut(),
                0,
                null_mut(),
                0,
                null_mut(),
                0,
                buf_ptr,
            );
            if ret == 0 {
                Ok(obj)
            } else {
                let c_err_str = std::ffi::CStr::from_ptr(buf_ptr);
                let str = c_err_str.to_str().unwrap_or_default();
                bail!(
                    "could not create ffmpeg object because {}",
                    String::from(str)
                )
            }
        }
    }
}

// // /// # Errors
// // ///
// // /// Will return `Err` if `ffmpeg` could not initialize
// // #[allow(clippy::not_unsafe_ptr_arg_deref)]
// // pub fn init(p_ffmpeg: w_ffmpeg) -> Result<()> {
// //     const TRACE: &str = "rist::init";

// //     let ret: i32 = unsafe { w_ffmpeg_init(p_ffmpeg, p_error.as_ptr() as *mut c_char) };
// //     match ret {
// //         0 => Ok(()),
// //         _ => {
// //             bail!("could not initialize ffmpeg")
// //         }
// //     }
// // }

// // /// # Errors
// // ///
// // /// Will return `Err` if `ffmpeg` could encode the frame.
// // #[allow(clippy::not_unsafe_ptr_arg_deref)]
// // pub fn ffmpeg_encode(
// //     p_ffmpeg_opt: w_ffmpeg_opt,
// //     p_data_in: &[u8],
// //     p_encoded_buffer: &mut CBuffer,
// //     p_error: &[u8],
// // ) -> Result<()> {
// //     let ret: i32 = unsafe {
// //         w_ffmpeg_encode(
// //             p_ffmpeg_opt,
// //             p_data_in.as_ptr() as *mut u8,
// //             &mut p_encoded_buffer.ptr,
// //             &mut p_encoded_buffer.len,
// //             p_error.as_ptr() as *mut c_char,
// //         )
// //     };

// //     match ret {
// //         0 => Ok(()),
// //         _ => {
// //             bail!("could not encode the buffer")
// //         }
// //     }
// // }

// // /// # Errors
// // ///
// // /// Will return `Err` if `ffmpeg` failed on decoding the frame.
// // #[allow(clippy::not_unsafe_ptr_arg_deref)]
// // pub fn ffmpeg_decode(
// //     p_ffmpeg_opt: w_ffmpeg_opt,
// //     p_data_in: &[u8],
// //     p_data_size: i32,
// //     p_decoded_buffer: &mut CBuffer,
// //     p_error: &[u8],
// // ) -> Result<()> {
// //     let ret: i32 = unsafe {
// //         w_ffmpeg_decode(
// //             p_ffmpeg_opt,
// //             p_data_in.as_ptr() as *mut u8,
// //             p_data_size,
// //             &mut p_decoded_buffer.ptr,
// //             &mut p_decoded_buffer.len,
// //             p_error.as_ptr() as *mut c_char,
// //         )
// //     };

// //     match ret {
// //         0 => Ok(()),
// //         _ => {
// //             bail!("could not decode the buffer")
// //         }
// //     }
// // }
