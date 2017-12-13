#ifndef CB_Converter_H
#define CB_Converter_H

#include <Magick++.h>

class CB_Converter {

  public:

    CB_Converter( const std::string & infilename );
    ~CB_Converter();

    void convert( std::string & cbtype );

  private:

    std::vector<float> create_Gamma_Lookup( );
    int inversePow( const float & x );

    void convert_colorblind( std::string & cbtype );
    void convert_anomylize( );
    void convert_monochrome( );

    void convert_pixel_colorblind( Magick::Quantum & r,
                                   Magick::Quantum & g,
                                   Magick::Quantum & b );

    // I/O filenames.
    const std::string infilename;
    std::string outfilename;

    // Global compile-time constants.
    static const float conversion_coeffs[3][4];
    static const float cb_gamma;
    static const float wx;
    static const float wy;
    static const float wz;
    static const float v;
    static const float d;

    // Run-time values, depending on colorblindness type.
    unsigned int range;
    float cpu;
    float cpv;
    float am;
    float ayi;

    // Lookup table to avoid repeated function calls.
    std::vector<float> powGammaLookup;

    // Image object (duplicated out of laziness).
    Magick::Image image_orig, image;
};

#endif
