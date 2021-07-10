#include <cassert>
#include <cmath>
#include <iostream>
#include <vector>
#include <string>
#include <Magick++.h>

#include "CB_Converter.hpp"

const float CB_Converter::conversion_coeffs[3][4] = {
  {0.753,  0.265, 1.273463, -0.073894},
  {1.140, -0.140, 0.968437,  0.003331},
  {0.171, -0.003, 0.062921,  0.292119}
};

const float cb_gamma = 2.2;
const float CB_Converter::wx = 0.312713;
const float CB_Converter::wy = 0.329016;
const float CB_Converter::wz = 0.358271;
const float CB_Converter::v = 1.75;
const float CB_Converter::d = v + 1.0;

// Constructor to read the original image and calculate some parameters based on
// it.
CB_Converter::CB_Converter( const std::string & infilename )
  : infilename( infilename ) {

  Magick::InitializeMagick( NULL );

  // Read image and prepare for modification.
  image_orig.read( infilename );
  image_orig.type( Magick::TrueColorAlphaType );

  // Determine range of pixel values (i.e., bit depth).
  range = QuantumRange;

  // Create lookup table to avoid repeated calculations.
  powGammaLookup = create_Gamma_Lookup( );

  return;
}

// Destructor.
CB_Converter::~CB_Converter() {
  return;
}

// Public driver function that takes the user-specified colorblindness type to
// convert the image to, and then calls the appropriate downstream conversion
// function(s).
void CB_Converter::convert( std::string & cbtype ) {

  if( !cbtype.compare( "All" ) ) {
    std::cout << "Performing conversion to ALL representations of colorblindness..." << std::endl;
    std::vector< std::string > convert_list =
      { "Normal", "Monochromacy",
      "Protanopia", "Deuteranopia", "Tritanopia",
      "Protanomaly", "Deuteranomaly", "Tritanomaly" } ;
    for( auto & it: convert_list ){
      convert( it );
    }
  } else {
    std::cout << "  Performing conversion to " << cbtype << "..." << std::endl;
    if( !cbtype.compare( "Normal" ) ) {
      image = image_orig;
    }
    else if( !cbtype.compare( "Protanopia" ) ||
             !cbtype.compare( "Deuteranopia" ) ||
             !cbtype.compare( "Tritanopia" ) ) {
      convert_colorblind( cbtype );
    }
    else if( !cbtype.compare( "Protanomaly" ) ||
             !cbtype.compare( "Deuteranomaly" ) ||
             !cbtype.compare( "Tritanomaly" ) ) {
      convert_colorblind( cbtype );
      convert_anomylize();
    }
    else if( !cbtype.compare( "Monochromacy" ) ) {
      convert_monochrome();
    } else {
      std::cerr << "Unrecognized type of colorblindness: " << cbtype << ", exiting." << std::endl;
      exit(1);
    }

    outfilename = "CB_";
    outfilename.append( cbtype );
    outfilename.append( "_" );
    outfilename.append( infilename );
    std::cout << "    Writing " << outfilename << "..." << std::endl;
    image.write( outfilename );
  }

  return;
}

// Lookup table to avoid repeated calculations.
std::vector<float> CB_Converter::create_Gamma_Lookup( ) {
  std::vector<float> powGammaLookup( range + 1, 0.0 );
  for( size_t i = 0; i <= range; ++i ) {
    powGammaLookup[i] = std::pow( static_cast<float>( i ) / static_cast<float>( range ), 2.2 );
  }
  return powGammaLookup;
}

// Calculate the inverse power of the values modified by gamma.  Used to
// convert back to RGB color space.
int CB_Converter::inversePow( const float & x ) {
  return round( static_cast< float >( range )
    * float( x <= 0 ? 0 : ( x >= 1 ? 1 : pow( x, 1/2.2 ) ) ) );
}

// Guides the colorblind conversion over the whole image after setting
// appropriate constants.  As pixels are iterated over, they are passed to a
// downstream function.
void CB_Converter::convert_colorblind( std::string & cbtype ) {

  image = image_orig;

  image.modifyImage();

  // Ensure that there is only one reference to underlying image. If this is not
  // done, then image pixels will not be modified.
  Magick::Pixels view( image );

  // Create a "pixel cache" for the entire image.
  Magick::Quantum *pixels = image.getPixels( 0, 0, image.columns(), image.rows() );

  // Set global colorblindness conversion constants.
  if( !cbtype.compare( "Protanopia" ) || !cbtype.compare( "Protanomaly" ) )  {
    cpu = conversion_coeffs[0][0];
    cpv = conversion_coeffs[0][1];
    am  = conversion_coeffs[0][2];
    ayi = conversion_coeffs[0][3];
  }
  else if( !cbtype.compare( "Deuteranopia" ) || !cbtype.compare( "Deuteranomaly" ) ) {
    cpu = conversion_coeffs[1][0];
    cpv = conversion_coeffs[1][1];
    am  = conversion_coeffs[1][2];
    ayi = conversion_coeffs[1][3];
  }
  else if( !cbtype.compare( "Tritanopia" ) || !cbtype.compare( "Tritanomaly" ) ) {
    cpu = conversion_coeffs[2][0];
    cpv = conversion_coeffs[2][1];
    am  = conversion_coeffs[2][2];
    ayi = conversion_coeffs[2][3];
  } else {
    assert( false );
  }

  // Get pixel values.
  size_t o;
  for( size_t row = 0; row < image.rows(); ++row ) {
    for( size_t column = 0; column < image.columns(); ++column ) {
      // Get offset for color channel lookup.
      o = image.channels() * ( image.columns() * row + column );
      // Perform conversion on given pixel.
      convert_pixel_colorblind( pixels[ o + 0 ], pixels[ o + 1 ], pixels[ o + 2 ] );
    }
  }

  // Save changes to image.
  view.sync();

  return;
}

// Performs post-processing on colorblind image to soften the effect to emulate
// non-full colorblindness.
void CB_Converter::convert_anomylize( ) {

  image_orig.modifyImage();
  image.modifyImage();

  // Ensure that there is only one reference to underlying image. If this is not
  // done, then image pixels will not be modified.
  Magick::Pixels view_orig( image_orig );
  Magick::Pixels view( image );

  // Create a "pixel cache" for the entire image.
  Magick::Quantum *pixels_orig = image_orig.getPixels( 0, 0, image.columns(), image.rows() );
  Magick::Quantum *pixels = image.getPixels( 0, 0, image.columns(), image.rows() );

  // Get pixel values.
  size_t o;
  for( size_t row = 0; row < image.rows(); ++row ) {
    for( size_t column = 0; column < image.columns(); ++column ) {
      // Get offset for color channel lookup.
      o = image.channels() * ( image.columns() * row + column );
      pixels[ o + 0 ] = (v * pixels[ o + 0 ] + pixels_orig[ o + 0 ]) / d; // R
      pixels[ o + 1 ] = (v * pixels[ o + 1 ] + pixels_orig[ o + 1 ]) / d; // G
      pixels[ o + 2 ] = (v * pixels[ o + 2 ] + pixels_orig[ o + 2 ]) / d; // B
    }
  }

  // Save changes to image.
  view.sync();

  return;
}

// Converts an image to represent *total* colorblindness (monochromacy).
void CB_Converter::convert_monochrome( ) {

  image = image_orig;

  image.modifyImage();

  // Point to encoder's bit depth
  image.defineValue("png", "bit-depth", "8");
  // Point to encoder's color-space type.
  image.defineValue("png", "color-type", "6");

  // Ensure that there is only one reference to underlying image. If this is not
  // done, then image pixels will not be modified.
  Magick::Pixels view( image );

  // Create a "pixel cache" for the entire image.
  Magick::Quantum *pixels = image.getPixels( 0, 0, image.columns(), image.rows() );

  // Get pixel values.
  size_t o;
  for( size_t row = 0; row < image.rows(); ++row ) {
    for( size_t column = 0; column < image.columns(); ++column ) {
      // Get offset for color channel lookup.
      o = image.channels() * ( image.columns() * row + column );
      unsigned int g = (0.299 * pixels[ o + 0 ]) +
                       (0.587 * pixels[ o + 1 ]) +
                       (0.114 * pixels[ o + 2 ]);
      pixels[ o + 0 ] = g; // R
      pixels[ o + 1 ] = g; // G
      pixels[ o + 2 ] = g; // B
    }
  }

  // Save changes to image.
  view.sync();

  return;
}

// Perform the actual colorblindness calculation on a given pixel given its
// original R, G, and B value.  This is the kernel that could/should be ported
// to hardware.
void CB_Converter::convert_pixel_colorblind( Magick::Quantum & r,
                                             Magick::Quantum & g,
                                             Magick::Quantum & b ) {

  const float cr = powGammaLookup[r];
  const float cg = powGammaLookup[g];
  const float cb = powGammaLookup[b];

  // Convert rgb -> xyz.
  const float cx = (0.430574 * cr + 0.341550 * cg + 0.178325 * cb);
  const float cy = (0.222015 * cr + 0.706655 * cg + 0.071330 * cb);
  const float cz = (0.020183 * cr + 0.129553 * cg + 0.939180 * cb);

  const float sum_xyz = cx + cy + cz;
  float cu = 0.0;
  float cv = 0.0;
  if( sum_xyz != 0.0 ) {
    cu = cx / sum_xyz;
    cv = cy / sum_xyz;
  }

  const float nx = wx * cy / wy;
  const float nz = wz * cy / wy;

  float clm = 0.0;
  const float dy = 0.0;

  if( cu < cpu ) {
    clm = ( cpv - cv ) / ( cpu - cu );
  } else {
    clm = ( cv - cpv ) / ( cu - cpu );
  }

  const float clyi = cv - cu * clm;
  const float du = ( ayi - clyi ) / ( clm - am );
  const float dv = ( clm * du ) + clyi;

  const float sx = du * cy / dv;
  const float sy = cy;
  const float sz = ( 1.0 - ( du + dv ) ) * cy / dv;

  // Convert xyz to rgb.
  float sr =  (3.063218 * sx - 1.393325 * sy - 0.475802 * sz);
  float sg = (-0.969243 * sx + 1.875966 * sy + 0.041555 * sz);
  float sb =  (0.067871 * sx - 0.228834 * sy + 1.069251 * sz);

  const float dx = nx - sx;
  const float dz = nz - sz;

  // Convert xyz to rgb.
  const float dr =  (3.063218 * dx - 1.393325 * dy - 0.475802 * dz);
  const float dg = (-0.969243 * dx + 1.875966 * dy + 0.041555 * dz);
  const float db =  (0.067871 * dx - 0.228834 * dy + 1.069251 * dz);

  const float adjr = dr > 0 ? ( ( sr < 0 ? 0 : 1 ) - sr ) / dr : 0;
  const float adjg = dg > 0 ? ( ( sg < 0 ? 0 : 1 ) - sg ) / dg : 0;
  const float adjb = db > 0 ? ( ( sb < 0 ? 0 : 1 ) - sb ) / db : 0;

  const float list[3] = {
    static_cast< float >( adjr > 1.0 || adjr < 0.0 ? 0.0 : adjr ),
    static_cast< float >( adjg > 1.0 || adjg < 0.0 ? 0.0 : adjg ),
    static_cast< float >( adjb > 1.0 || adjb < 0.0 ? 0.0 : adjb ),
    };

  const float adjust = *std::max_element( list, list + 3 );

  sr = sr + ( adjust * dr );
  sg = sg + ( adjust * dg );
  sb = sb + ( adjust * db );

  r = inversePow( sr );
  g = inversePow( sg );
  b = inversePow( sb );

  return;
}

