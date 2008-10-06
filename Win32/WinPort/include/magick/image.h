/*
  Copyright (C) 2003 GraphicsMagick Group
  Copyright (C) 2002 ImageMagick Studio
  Copyright 1991-1999 E. I. du Pont de Nemours and Company
 
  This program is covered by multiple licenses, which are described in
  Copyright.txt. You should have received a copy of Copyright.txt with this
  package; otherwise see http://www.graphicsmagick.org/www/Copyright.html.
 
  GraphicsMagick Image Methods.
*/
#ifndef _MAGICK_IMAGE_H
#define _MAGICK_IMAGE_H

#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif

/*
  Include declarations.
*/
#include "magick/error.h"
#include "magick/timer.h"

/*
  Define declarations.
*/
#if !defined(QuantumDepth)
#define QuantumDepth  16
#define QuantumLeap
#endif

#if (QuantumDepth == 8)
#define MaxColormapSize  256UL
#define MaxMap  255UL
#define MaxRGB  255UL
#define ScaleCharToMap(value)        ((unsigned char) (value))
#define ScaleCharToQuantum(value)    ((Quantum) (value))
#define ScaleLongToQuantum(value)    ((Quantum) ((value)/16843009UL))
#define ScaleMapToChar(value)        ((unsigned int) (value))
#define ScaleMapToQuantum(value)     ((Quantum) (value))
#define ScaleQuantum(quantum)        ((unsigned long) (quantum))
#define ScaleQuantumToChar(quantum)  ((unsigned char) (quantum))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (16843009UL*(quantum)))
#define ScaleQuantumToMap(quantum)   ((unsigned char) (quantum))
#define ScaleQuantumToShort(quantum) ((unsigned short) (257UL*(quantum)))
#define ScaleShortToQuantum(value)   ((Quantum) ((value)/257UL))
#define ScaleToQuantum(value)        ((unsigned long) (value))
#define ScaleQuantumToIndex(value)   ((unsigned char) (value))
/*
  intensity=0.299*red+0.587*green+0.114*blue.
  Premultiply by 1024 to obtain integral values, and then divide
  result by 1024 by shifting to the right by 10 bits.
*/
#define PixelIntensity(pixel) ((unsigned int) \
   (((unsigned int)306U*(pixel)->red+ \
     (unsigned int)601U*(pixel)->green+ \
     (unsigned int)117U*(pixel)->blue) \
    >> 10U))
typedef unsigned char Quantum;
#elif (QuantumDepth == 16)
#define MaxColormapSize  65536UL
#define MaxMap 65535UL
#define MaxRGB  65535UL
#define ScaleCharToMap(value)        ((unsigned short) (257UL*(value)))
#define ScaleCharToQuantum(value)    ((Quantum) (257UL*(value)))
#define ScaleLongToQuantum(value)    ((Quantum) ((value)/65537UL))
#define ScaleMapToChar(value)        ((unsigned int) ((value)/257UL))
#define ScaleMapToQuantum(value)     ((Quantum) (value))
#define ScaleQuantum(quantum)        ((unsigned long) ((quantum)/257UL))
#define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/257UL))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (65537UL*(quantum)))
#define ScaleQuantumToMap(quantum)   ((unsigned short) (quantum))
#define ScaleQuantumToShort(quantum) ((unsigned short) (quantum))
#define ScaleShortToQuantum(value)   ((Quantum) (value))
#define ScaleToQuantum(value)        ((unsigned long) (257UL*(value)))
#define ScaleQuantumToIndex(value)   ((unsigned short) (value))
/*
  intensity=0.299*red+0.587*green+0.114*blue.
  Premultiply by 1024 to obtain integral values, and then divide
  result by 1024 by shifting to the right by 10 bits.
*/
#define PixelIntensity(pixel) ((unsigned int) \
   (((unsigned int)306U*(pixel)->red+ \
     (unsigned int)601U*(pixel)->green+ \
     (unsigned int)117U*(pixel)->blue) \
    >> 10U))
typedef unsigned short Quantum;
#elif (QuantumDepth == 32)
#define MaxColormapSize  65536UL
#define MaxMap 65535UL
#define MaxRGB  4294967295UL
#define ScaleCharToMap(value)        ((unsigned short) (257UL*(value)))
#define ScaleCharToQuantum(value)    ((Quantum) (16843009UL*(value)))
#define ScaleLongToQuantum(value)    ((Quantum) ((value)))
#define ScaleMapToChar(value)        ((unsigned int) ((value)/257UL))
#define ScaleMapToQuantum(value)     ((Quantum) (65537UL*(value)))
#define ScaleQuantum(quantum)        ((unsigned long) ((quantum)/16843009UL))
#define ScaleQuantumToChar(quantum)  ((unsigned char) ((quantum)/16843009UL))
#define ScaleQuantumToLong(quantum)  ((unsigned long) (quantum))
#define ScaleQuantumToMap(quantum)   ((unsigned short) ((quantum)/65537UL))
#define ScaleQuantumToShort(quantum) ((unsigned short) ((quantum)/65537UL))
#define ScaleShortToQuantum(value)   ((Quantum) (65537UL*(value)))
#define ScaleToQuantum(value)        ((unsigned long) (16843009UL*(value)))
#define ScaleQuantumToIndex(value)   ((unsigned short) ((value)/65537UL))
/*
  intensity=0.299*red+0.587*green+0.114*blue.
  Premultiply by 1024 to obtain integral values, and then divide
  result by 1024.
*/
#define PixelIntensity(pixel) ((unsigned int) \
   (((double)306.0*(pixel)->red+ \
     (double)601.0*(pixel)->green+ \
     (double)117.0*(pixel)->blue) \
    / 1024.0))
typedef unsigned int Quantum;
#else
# error "Specified value of QuantumDepth is not supported"
#endif

#define ColorMatch(p,q) (((p)->red == (q)->red) && \
  ((p)->green == (q)->green) && ((p)->blue == (q)->blue))
#define OpaqueOpacity  0UL
#define TransparentOpacity  MaxRGB
#define RoundSignedToQuantum(value) ((Quantum) (value < 0 ? 0 : \
  (value > MaxRGB) ? MaxRGB : value + 0.5))
#define RoundToQuantum(value) ((Quantum) (value > MaxRGB ? MaxRGB : \
  value + 0.5))
#define ConstrainToRange(min,max,value) (value < min ? min : \
  (value > max) ? max : value)
#define ConstrainToQuantum(value) ConstrainToRange(0,MaxRGB,value)
#define ScaleAnyToQuantum(x,max_value) \
  ((Quantum) (((double) MaxRGB*x)/max_value+0.5))

#define PixelIntensityToDouble(pixel) ((double)PixelIntensity(pixel))
#define PixelIntensityToQuantum(pixel) ((Quantum)PixelIntensity(pixel))

/*
  Maximum RGB value which fits in the specified bits
*/
#define MaxRGBGivenBits(bits) ((0x01U << (bits-1)) +((0x01U << (bits-1))-1))

/*
  Tests for colorspace classification.
*/
#define IsCMYKColorspace(colorspace) \
  ( \
    (colorspace == CMYKColorspace) \
  )
#define IsGrayColorspace(colorspace) \
  ( \
    (colorspace == GRAYColorspace) \
  )
#define IsRGBColorspace(colorspace) \
  ( \
    (IsGrayColorspace(colorspace)) || \
    (colorspace == RGBColorspace) || \
    (colorspace == TransparentColorspace) \
  )
#define IsYCbCrColorspace(colorspace) \
  ( \
    (colorspace == YCbCrColorspace) \
  )

/*
  Deprecated defines.
*/
#define Downscale(quantum)  ScaleQuantumToChar(quantum)
#define Intensity(color)  PixelIntensityToQuantum(color)
#define RunlengthEncodedCompression RLECompression
#define Upscale(value)  ScaleCharToQuantum(value)
#define XDownscale(value)  ScaleShortToQuantum(value)
#define XUpscale(quantum)  ScaleQuantumToShort(quantum)

/*
  Enum declarations.
*/
typedef enum
{
  UndefinedChannel,
  RedChannel,
  CyanChannel,
  GreenChannel,
  MagentaChannel,
  BlueChannel,
  YellowChannel,
  OpacityChannel,
  BlackChannel,
  MatteChannel,
  AllChannels
} ChannelType;

typedef enum
{
  UndefinedClass,
  DirectClass,
  PseudoClass
} ClassType;

typedef enum
{
  UndefinedColorspace,
  RGBColorspace,
  GRAYColorspace,
  TransparentColorspace,
  OHTAColorspace,
  XYZColorspace,
  YCbCrColorspace,
  YCCColorspace,
  YIQColorspace,
  YPbPrColorspace,
  YUVColorspace,
  CMYKColorspace,
  sRGBColorspace,
  HSLColorspace,
  HWBColorspace,
  LABColorspace
} ColorspaceType;

typedef enum
{
  UndefinedCompliance = 0x0000,
  NoCompliance = 0x0000,
  SVGCompliance = 0x0001,
  X11Compliance = 0x0002,
  XPMCompliance = 0x0004,
  AllCompliance = 0xffff
} ComplianceType;

typedef enum
{
  UndefinedCompositeOp = 0,
  OverCompositeOp,
  InCompositeOp,
  OutCompositeOp,
  AtopCompositeOp,
  XorCompositeOp,
  PlusCompositeOp,
  MinusCompositeOp,
  AddCompositeOp,
  SubtractCompositeOp,
  DifferenceCompositeOp,
  MultiplyCompositeOp,
  BumpmapCompositeOp,
  CopyCompositeOp,
  CopyRedCompositeOp,
  CopyGreenCompositeOp,
  CopyBlueCompositeOp,
  CopyOpacityCompositeOp,
  ClearCompositeOp,
  DissolveCompositeOp,
  DisplaceCompositeOp,
  ModulateCompositeOp,
  ThresholdCompositeOp,
  NoCompositeOp,
  DarkenCompositeOp,
  LightenCompositeOp,
  HueCompositeOp,
  SaturateCompositeOp,
  ColorizeCompositeOp,
  LuminizeCompositeOp,
  ScreenCompositeOp,
  OverlayCompositeOp,
  CopyCyanCompositeOp,
  CopyMagentaCompositeOp,
  CopyYellowCompositeOp,
  CopyBlackCompositeOp
} CompositeOperator;

typedef enum
{
  UndefinedCompression,
  NoCompression,
  BZipCompression,
  FaxCompression,
  Group4Compression,
  JPEGCompression,
  LosslessJPEGCompression,
  LZWCompression,
  RLECompression,
  ZipCompression
} CompressionType;

typedef enum
{
  UndefinedDispose,
  NoneDispose,
  BackgroundDispose,
  PreviousDispose
} DisposeType;

typedef enum
{
  UndefinedEndian,
  LSBEndian,
  MSBEndian
} EndianType;

typedef enum
{
  UndefinedFilter,
  PointFilter,
  BoxFilter,
  TriangleFilter,
  HermiteFilter,
  HanningFilter,
  HammingFilter,
  BlackmanFilter,
  GaussianFilter,
  QuadraticFilter,
  CubicFilter,
  CatromFilter,
  MitchellFilter,
  LanczosFilter,
  BesselFilter,
  SincFilter
} FilterTypes;

typedef enum
{
#undef NoValue
  NoValue = 0x0000,
#undef XValue
  XValue = 0x0001,
#undef YValue
  YValue = 0x0002,
#undef WidthValue
  WidthValue = 0x0004,
#undef HeightValue
  HeightValue = 0x0008,
#undef AllValues
  AllValues = 0x000F,
#undef XNegative
  XNegative = 0x0010,
#undef YNegative
  YNegative = 0x0020,
  PercentValue = 0x1000,
  AspectValue = 0x2000,
  LessValue = 0x4000,
  GreaterValue = 0x8000,
  AreaValue = 0x10000
} GeometryFlags;

typedef enum
{
#undef ForgetGravity
  ForgetGravity,
#undef NorthWestGravity
  NorthWestGravity,
#undef NorthGravity
  NorthGravity,
#undef NorthEastGravity
  NorthEastGravity,
#undef WestGravity
  WestGravity,
#undef CenterGravity
  CenterGravity,
#undef EastGravity
  EastGravity,
#undef SouthWestGravity
  SouthWestGravity,
#undef SouthGravity
  SouthGravity,
#undef SouthEastGravity
  SouthEastGravity,
#undef StaticGravity
  StaticGravity
} GravityType;

typedef enum
{
  UndefinedType,
  BilevelType,
  GrayscaleType,
  GrayscaleMatteType,
  PaletteType,
  PaletteMatteType,
  TrueColorType,
  TrueColorMatteType,
  ColorSeparationType,
  ColorSeparationMatteType,
  OptimizeType
} ImageType;

typedef enum
{
  UndefinedInterlace,
  NoInterlace,
  LineInterlace,
  PlaneInterlace,
  PartitionInterlace
} InterlaceType;

typedef enum
{
  UndefinedMode,
  FrameMode,
  UnframeMode,
  ConcatenateMode
} MontageMode;

typedef enum
{
  UniformNoise,
  GaussianNoise,
  MultiplicativeGaussianNoise,
  ImpulseNoise,
  LaplacianNoise,
  PoissonNoise
} NoiseType;

typedef enum
{
  UndefinedPreview = 0,
  RotatePreview,
  ShearPreview,
  RollPreview,
  HuePreview,
  SaturationPreview,
  BrightnessPreview,
  GammaPreview,
  SpiffPreview,
  DullPreview,
  GrayscalePreview,
  QuantizePreview,
  DespecklePreview,
  ReduceNoisePreview,
  AddNoisePreview,
  SharpenPreview,
  BlurPreview,
  ThresholdPreview,
  EdgeDetectPreview,
  SpreadPreview,
  SolarizePreview,
  ShadePreview,
  RaisePreview,
  SegmentPreview,
  SwirlPreview,
  ImplodePreview,
  WavePreview,
  OilPaintPreview,
  CharcoalDrawingPreview,
  JPEGPreview
} PreviewType;

typedef enum
{
  UndefinedQuantumOp = 0,
  AddQuantumOp,
  AndQuantumOp,
  DivideQuantumOp,
  LShiftQuantumOp,
  MultiplyQuantumOp,
  OrQuantumOp,
  RShiftQuantumOp,
  SubtractQuantumOp,
  XorQuantumOp
 } QuantumOperator;

typedef enum
{
  UndefinedIntent,
  SaturationIntent,
  PerceptualIntent,
  AbsoluteIntent,
  RelativeIntent
} RenderingIntent;

typedef enum
{
  UndefinedResolution,
  PixelsPerInchResolution,
  PixelsPerCentimeterResolution
} ResolutionType;

/*
  Typedef declarations.
*/
typedef struct _AffineMatrix
{
  double
    sx,
    rx,
    ry,
    sy,
    tx,
    ty;
} AffineMatrix;

typedef struct _PrimaryInfo
{
  double
    x,
    y,
    z;
} PrimaryInfo;

typedef struct _ChromaticityInfo
{
  PrimaryInfo
    red_primary,
    green_primary,
    blue_primary,
    white_point;
} ChromaticityInfo;

typedef struct _PixelPacket
{
#if defined(WORDS_BIGENDIAN)
  Quantum
    red,
    green,
    blue,
    opacity;
#else
#if defined(macintosh)
  Quantum
    opacity,
    red,
    green,
    blue;
#else
  Quantum
    blue,
    green,
    red,
    opacity;
#endif
#endif
} PixelPacket;

typedef struct _ColorInfo
{
  char
    *path,
    *name;

  ComplianceType
    compliance;

  PixelPacket
    color;

  unsigned int
    stealth;

  unsigned long
    signature;

  struct _ColorInfo
    *previous,
    *next;
} ColorInfo;

typedef struct _DoublePixelPacket
{
  double
    red,
    green,
    blue,
    opacity;
} DoublePixelPacket;

typedef struct _ErrorInfo
{
  double
    mean_error_per_pixel,
    normalized_mean_error,
    normalized_maximum_error;
} ErrorInfo;

typedef struct _FrameInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y,
    inner_bevel,
    outer_bevel;
} FrameInfo;

typedef Quantum IndexPacket;

typedef struct _LongPixelPacket
{
  unsigned long
    red,
    green,
    blue,
    opacity;
} LongPixelPacket;

typedef struct _MontageInfo
{
  char
    *geometry,
    *tile,
    *title,
    *frame,
    *texture,
    *font;

  double
    pointsize;

  unsigned long
    border_width;

  unsigned int
    shadow;

  PixelPacket
    fill,
    stroke,
    background_color,
    border_color,
    matte_color;

  GravityType
    gravity;

  char
    filename[MaxTextExtent];

  unsigned long
    signature;
} MontageInfo;

typedef struct _ProfileInfo
{
  size_t
    length;

  char
    *name;

  unsigned char
    *info;
} ProfileInfo;

typedef struct _RectangleInfo
{
  unsigned long
    width,
    height;

  long
    x,
    y;
} RectangleInfo;

typedef struct _SegmentInfo
{
  double
    x1,
    y1,
    x2,
    y2;
} SegmentInfo;

typedef struct _ImageChannelStatistics
 {
   /* Minimum value observed */
   double maximum;
   /* Maximum value observed */
   double minimum;
   /* Average (mean) value observed */
   double mean;
   /* Standard deviation, sqrt(variance) */
   double standard_deviation;
   /* Variance */
   double variance;
 } ImageChannelStatistics;

typedef struct _ImageStatistics
 {
   ImageChannelStatistics red;
   ImageChannelStatistics green;
   ImageChannelStatistics blue;
   ImageChannelStatistics opacity;
 } ImageStatistics;

typedef struct _Ascii85Info _Ascii85Info_;

typedef struct _BlobInfo _BlobInfo_;

typedef struct _ImageAttribute  _ImageAttribute_;

typedef struct _Image
{
  ClassType
    storage_class;      /* DirectClass (TrueColor) or PseudoClass (colormapped) */

  ColorspaceType
    colorspace;         /* Current image colorspace/model */

  CompressionType
    compression;        /* Compression algorithm to use when encoding image */

  unsigned int
    dither,             /* True if image is to be dithered */
    matte;              /* True if image has an opacity channel */ 

  unsigned long
    columns,            /* Number of image columns */
    rows;               /* Number of image rows */

  unsigned long
    depth,              /* Bits of precision to preserve in color quantum */
    colors;             /* Current number of colors in PseudoClass colormap */

  PixelPacket
    *colormap;          /* Pseudoclass colormap array */

  PixelPacket
    background_color,   /* Background color */
    border_color,       /* Border color */
    matte_color;        /* Matte (transparent) color */

  double
    gamma;              /* Image gamma */

  ChromaticityInfo
    chromaticity;       /* Red, green, blue, and white chromaticity values */

  RenderingIntent
    rendering_intent;   /* Rendering intent */

  ResolutionType
    units;              /* Units of image resolution (density) */

  char
    *montage,           /* Tile size and offset within an image montage */
    *directory,         /* Tile names from within an image montage */
    *geometry;          /* Composite/Crop options */

  long
    offset;             /* Offset to start of image data */

  double
    x_resolution,       /* Horizontal resolution (also see units) */
    y_resolution;       /* Vertical resoution (also see units) */

  RectangleInfo
    page,               /* Offset to apply when placing image */
    tile_info;          /* Subregion tile dimensions and offset */

  double
    blur,               /* Amount of blur to apply when zooming image */
    fuzz;               /* Colors within this distance match target color */

  FilterTypes
    filter;             /* Filter to use when zooming image */

  InterlaceType
    interlace;          /* Interlace pattern to use when writing image */

  EndianType
    endian;             /* Byte order to use when writing image */

  GravityType
    gravity;            /* Image placement gravity */

  CompositeOperator
    compose;            /* Image placement composition */

  DisposeType
    dispose;            /* GIF disposal option */

  unsigned long
    scene,              /* Animation frame scene number */
    delay,              /* Animation frame scene delay */
    iterations,         /* Animation iterations */
    total_colors;       /* Number of unique colors. See GetNumberColors() */

  long
    start_loop;         /* Animation frame number to start looping at */

  ErrorInfo
    error;              /* Computed image comparison or quantization error */

  TimerInfo
    timer;              /* Operation micro-timer */

  void
    *client_data;       /* User specified opaque data pointer */

  /*
    Output file name.

    A colon delimited format identifier may be prepended to the file
    name in order to force a particular output format. Otherwise the
    file extension is used. If no format prefix or file extension is
    present, then the output format is determined by the 'magick'
    field.
  */
  char
    filename[MaxTextExtent];

  /*
    Original file name (name of input image file)
  */
  char
    magick_filename[MaxTextExtent];

  /*
    File format of the input file, and the default output format.

    The precedence when selecting the output format is:
      1) magick prefix to file name (e.g. "jpeg:foo).
      2) file name extension. (e.g. "foo.jpg")
      3) content of this magick field.

  */
  char
    magick[MaxTextExtent];

  /*
    Original image width (before transformations)
  */
  unsigned long
    magick_columns;

  /*
    Original image height (before transformations)
  */
  unsigned long
    magick_rows;

  ExceptionInfo
    exception;          /* Any error associated with this image frame */

  struct _Image
    *previous,          /* Pointer to previous frame */
    *next;              /* Pointer to next frame */

  /*
    Only private members appear past this point
  */

  ProfileInfo
    color_profile,      /* ICC color profile */
    iptc_profile,       /* IPTC newsphoto profile */
    *generic_profile;   /* List of additional profiles */

  unsigned long
    generic_profiles;   /* Number of additional generic profiles */

  unsigned int
    is_monochrome,      /* Private, True if image is known to be monochrome */
    is_grayscale,       /* Private, True if image is known to be grayscale */
    taint;              /* Private, True if image has not been modifed */

  struct _Image
    *clip_mask;         /* Private, Clipping mask to apply when updating pixels */

  void
    *cache;             /* Private, image pixel cache */

  _ImageAttribute_
    *attributes;        /* Private, Image attribute list */

  _Ascii85Info_
    *ascii85;           /* Private, supports huffman encoding */

  _BlobInfo_
    *blob;              /* Private, file I/O object */

  long
    reference_count;    /* Private, Image reference count */

  void
    *semaphore;         /* Private, Per image lock (for reference count) */

  unsigned int
    logging;            /* Private, True if logging is enabled */

  struct _Image
    *list;              /* Private, used only by display */

  unsigned long
    signature;          /* Private, Unique code to validate structure */
} Image;

typedef unsigned int
  (*StreamHandler)(const Image *,const void *,const size_t);

typedef struct _ImageInfo
{
  CompressionType
    compression;             /* Image compression to use while decoding */

  unsigned int
    temporary,               /* Remove file "filename" once it has been read. */
    adjoin,                  /* If True, join multiple frames into one file */
    antialias;               /* If True, antialias while rendering */

  unsigned long
    subimage,                /* Starting image scene ID to select */
    subrange,                /* Span of image scene IDs (from starting scene) to select */
    depth;                   /* Number of quantum bits to preserve while encoding */

  char
    *size,                   /* Desired/known dimensions to use when decoding image */
    *tile,                   /* Deprecated, name of image to tile on background */
    *page;                   /* Output page size & offset */

  InterlaceType
    interlace;               /* Interlace scheme to use when decoding image */

  EndianType
    endian;                  /* Select MSB/LSB endian output for TIFF format */

  ResolutionType
    units;                   /* Units to apply when evaluating the density option */

  unsigned long
    quality;                 /* Compression quality factor (format specific) */

  char
    *sampling_factor,        /* JPEG, MPEG, and YUV chroma downsample factor */
    *server_name,            /* X11 server display specification */
    *font,                   /* Font name to use for text annotations */
    *texture,                /* Name of texture image to use for background fills */
    *density;                /* Image resolution (also see units) */

  double
    pointsize;               /* Font pointsize */

  double
    fuzz;                    /* Colors within this distance are a match */

  PixelPacket
    pen,                     /* Stroke or fill color while drawing */
    background_color,        /* Background color */
    border_color,            /* Border color (color surrounding frame) */
    matte_color;             /* Matte color (frame color) */

  unsigned int
    dither,                  /* If true, dither image while writing */
    monochrome;              /* If true, use monochrome format */

  ColorspaceType
    colorspace;              /* Colorspace representations of image pixels */

  ImageType
    type;                    /* Desired image type (used while writing) */

  long
    group;                   /* X11 window group ID */

  unsigned int
    verbose;                 /* If true, display high-level processing */

  char
    *view,                   /* FlashPIX view specification */
    *authenticate;           /* Password used to decrypt file */

  void
    *client_data;            /* User-specified data to pass to coder */

  StreamHandler
    stream;                  /* Pass in open blob stream handler for read/write */

  FILE
    *file;                   /* If not null, stdio FILE to read image from */

  char
    magick[MaxTextExtent],   /* File format to read. Overrides file extension */
    filename[MaxTextExtent]; /* File name to read */

  /*
    Only private members appear past this point
  */

  void
    *cache;                  /* Private. Used to pass image via open cache */

  void
    *definitions;            /* Private. Map of coder specific options passed by user.
                                Use AddDefinitions, RemoveDefinitions, & AccessDefinition
                                to access and manipulate this data. */

  Image
    *attributes;             /* Private. Image attribute list */

  unsigned int
    ping;                    /* Private, if true, read file header only */

  PreviewType
    preview_type;            /* Private, used by PreviewImage */

  unsigned int
    affirm;                  /* Private, when true do not intuit image format */

  void
    *blob;                   /* Private, used to pass in open blob */

  size_t
    length;                  /* Private, used to pass in open blob length */

  char
    unique[MaxTextExtent],   /* Private, passes temporary filename to TranslateText */
    zero[MaxTextExtent];     /* Private, passes temporary filename to TranslateText */

  unsigned long
    signature;               /* Private, used to validate structure */
} ImageInfo;

/*
  Image const declarations.
*/
extern MagickExport const char
  *BackgroundColor,
  *BorderColor,
  *DefaultTileFrame,
  *DefaultTileGeometry,
  *DefaultTileLabel,
  *ForegroundColor,
  *MatteColor,
  *LoadImageText,
  *LoadImagesText,
  *PSDensityGeometry,
  *PSPageGeometry,
  *SaveImageText,
  *SaveImagesText;

extern MagickExport const unsigned long
  DefaultCompressionQuality;

/*
  Image utilities methods.
*/

extern MagickExport ExceptionType
  CatchImageException(Image *);

extern MagickExport Image
  *AllocateImage(const ImageInfo *),
  *AppendImages(const Image *,const unsigned int,ExceptionInfo *),
  *AverageImages(const Image *,ExceptionInfo *),
  *CloneImage(const Image *,const unsigned long,const unsigned long,
   const unsigned int,ExceptionInfo *),
  *ReferenceImage(Image *);

extern MagickExport ImageInfo
  *CloneImageInfo(const ImageInfo *);

extern MagickExport ImageType
  GetImageType(const Image *,ExceptionInfo *);

extern MagickExport const char
  *AccessDefinition(const ImageInfo *image_info,const char *magick,
   const char *key);

extern MagickExport int
  GetImageGeometry(const Image *,const char *,const unsigned int,
  RectangleInfo *);

extern MagickExport RectangleInfo
  GetImageBoundingBox(const Image *,ExceptionInfo *exception);

extern MagickExport unsigned int
  AddDefinitions(ImageInfo *image_info,const char *options,
    ExceptionInfo *exception),
  AllocateImageColormap(Image *,const unsigned long),
  AnimateImages(const ImageInfo *image_info,Image *image),
  ChannelImage(Image *,const ChannelType),
  ClipImage(Image *),
  ClipPathImage(Image *,const char *,const unsigned int),
  CycleColormapImage(Image *image,const int amount),
  DescribeImage(Image *image,FILE *file,const unsigned int verbose),
  DisplayImages(const ImageInfo *image_info,Image *image),
  GetImageChannelDepth(const Image *image,
    const ChannelType channel, ExceptionInfo *exception),
  GradientImage(Image *,const PixelPacket *,const PixelPacket *),
  IsImagesEqual(Image *,const Image *),
  IsTaintImage(const Image *),
  IsSubimage(const char *,const unsigned int),
  PlasmaImage(Image *,const SegmentInfo *,unsigned long,unsigned long),
  RemoveDefinitions(const ImageInfo *image_info,const char *options),
  RGBTransformImage(Image *,const ColorspaceType),
  SetImageChannelDepth(Image *image,
    const ChannelType channel, const unsigned int depth),
  SetImageClipMask(Image *image,Image *clip_mask),
  SetImageDepth(Image *,const unsigned long),
  SetImageInfo(ImageInfo *,const unsigned int,ExceptionInfo *),
  SetImageType(Image *,const ImageType),
  SortColormapByIntensity(Image *),
  TextureImage(Image *,const Image *),
  TransformColorspace(Image *,const ColorspaceType),
  TransformRGBImage(Image *,const ColorspaceType);

extern MagickExport MagickPassFail
  GetImageStatistics(const Image *image,ImageStatistics *statistics,
    ExceptionInfo *exception),
  QuantumOperatorImage(Image *image,const ChannelType channel,
    const QuantumOperator quantum_operator,const double rvalue,
    ExceptionInfo *exception),
  QuantumOperatorRegionImage(Image *image,const long x,const long y,
    const unsigned long columns,const unsigned long rows,
    const ChannelType channel,const QuantumOperator quantum_operator,
    const double rvalue,ExceptionInfo *exception);

extern MagickExport unsigned long
  GetImageDepth(const Image *,ExceptionInfo *);

extern MagickExport void
  AllocateNextImage(const ImageInfo *,Image *),
  DestroyImage(Image *),
  DestroyImageInfo(ImageInfo *),
  GetImageException(Image *,ExceptionInfo *),
  GetImageInfo(ImageInfo *),
  GrayscalePseudoClassImage(Image *,unsigned int),
  ModifyImage(Image **,ExceptionInfo *),
  SetImage(Image *,const Quantum),
  SetImageOpacity(Image *,const unsigned int),
  SyncImage(Image *);

extern MagickExport const char
  *ColorspaceTypeToString(const ColorspaceType colorspace);

#if defined(__cplusplus) || defined(c_plusplus)
}
#endif

#endif
