#!/usr/bin/env python

import numpy as np
import os
from PIL import Image

class ColorBlindConverter(object):

    def __init__(self, inputimage):
        self.powGammaLookup = np.power(np.linspace(0,256,256)/256, 2.2)
        self.conversion_coeffs = {
            'p': {'cpu': 0.753, 'cpv':  0.265, 'am': 1.273463, 'ayi': -0.073894},
            'd': {'cpu': 1.140, 'cpv': -0.140, 'am': 0.968437, 'ayi':  0.003331},
            't': {'cpu': 0.171, 'cpv': -0.003, 'am': 0.062921, 'ayi':  0.292119}}
        self.image = Image.open(inputimage)
        self.image_orig = self.image
        self.infilename = inputimage
        self.prefix = os.path.splitext(inputimage)[0]
        self.suffix = ''
        self.extension = os.path.splitext(inputimage)[1]

        self.width, self.height = self.image.size

    def _inversePow(self, x):
        return int(255.0 * float(0 if x <= 0 else (1 if x >= 1 else np.power(x, 1/2.2))))

    def convert(self, cb_type):
        self.cb_type = cb_type
        self.image = self.image_orig
        print('Converting {:} to vision type: {:}.'.format(self.infilename, self.cb_type))
        if(   self.cb_type == 'Normal'):
            self._convert_normal()
        if(   self.cb_type == 'Protanopia' or
              self.cb_type == 'Deuteranopia' or
              self.cb_type == 'Tritanopia'):
            self._convert_colorblind()
        elif( self.cb_type == 'Protanomaly' or
              self.cb_type == 'Deuteranomaly' or
              self.cb_type == 'Tritanomaly'):
            self._convert_colorblind()
            self._convert_anomylize()
        elif( self.cb_type == 'Monochromacy'):
            self._convert_monochrome()
        self.suffix = self.cb_type
        return

    def _convert_normal(self):
        self.image = self.image
        return

    def _convert_colorblind(self):
        gamma = 2.2;
        wx = 0.312713;
        wy = 0.329016;
        wz = 0.358271;

        cpu = self.conversion_coeffs[self.cb_type[0].lower()]['cpu']
        cpv = self.conversion_coeffs[self.cb_type[0].lower()]['cpv']
        am  = self.conversion_coeffs[self.cb_type[0].lower()]['am']
        ayi = self.conversion_coeffs[self.cb_type[0].lower()]['ayi']

        image_new = Image.new("RGB", (self.width, self.height), "white")
        p_new = image_new.load()
        for i in range(self.width):
            for j in range(self.height):
                p_old = self.image.getpixel((i,j))
                r = p_old[0]
                g = p_old[1]
                b = p_old[2]

                cr = self.powGammaLookup[r]
                cg = self.powGammaLookup[g]
                cb = self.powGammaLookup[b]

                # rgb -> xyz
                cx = (0.430574 * cr + 0.341550 * cg + 0.178325 * cb)
                cy = (0.222015 * cr + 0.706655 * cg + 0.071330 * cb)
                cz = (0.020183 * cr + 0.129553 * cg + 0.939180 * cb)

                sum_xyz = cx + cy + cz
                cu = 0
                cv = 0

                if(sum_xyz != 0):
                    cu = cx / sum_xyz
                    cv = cy / sum_xyz

                nx = wx * cy / wy
                nz = wz * cy / wy
                clm = 0
                dy = 0

                if(cu < cpu):
                    clm = (cpv - cv) / (cpu - cu)
                else:
                    clm = (cv - cpv) / (cu - cpu)

                clyi = cv - cu * clm
                du = (ayi - clyi) / (clm - am)
                dv = (clm * du) + clyi

                sx = du * cy / dv
                sy = cy
                sz = (1 - (du + dv)) * cy / dv

                # xyz->rgb
                sr =  (3.063218 * sx - 1.393325 * sy - 0.475802 * sz)
                sg = (-0.969243 * sx + 1.875966 * sy + 0.041555 * sz)
                sb =  (0.067871 * sx - 0.228834 * sy + 1.069251 * sz)

                dx = nx - sx
                dz = nz - sz

                # xyz->rgb

                dr =  (3.063218 * dx - 1.393325 * dy - 0.475802 * dz)
                dg = (-0.969243 * dx + 1.875966 * dy + 0.041555 * dz)
                db =  (0.067871 * dx - 0.228834 * dy + 1.069251 * dz)

                adjr = ((0 if sr < 0 else 1) - sr) / dr if dr > 0 else 0
                adjg = ((0 if sg < 0 else 1) - sg) / dg if dg > 0 else 0
                adjb = ((0 if sb < 0 else 1) - sb) / db if db > 0 else 0

                adjust = max([
                    0 if (adjr > 1 or adjr < 0) else adjr,
                    0 if (adjg > 1 or adjg < 0) else adjg,
                    0 if (adjb > 1 or adjb < 0) else adjb])

                sr = sr + (adjust * dr)
                sg = sg + (adjust * dg)
                sb = sb + (adjust * db)

                p_new[i,j] = (self._inversePow(sr),
                              self._inversePow(sg),
                              self._inversePow(sb))

        self.image = image_new

        return self.image

    def _convert_anomylize(self):
        v = 1.75
        d = v + 1
        image_new = Image.new("RGB", (self.width, self.height), "white")
        p_new = image_new.load()
        for i in range(self.width):
            for j in range(self.height):
                p_orig = self.image_orig.getpixel((i,j))
                r_orig = p_orig[0]
                g_orig = p_orig[1]
                b_orig = p_orig[2]
                p_cb = self.image.getpixel((i,j))
                r_cb = p_cb[0]
                g_cb = p_cb[1]
                b_cb = p_cb[2]

                r_new = (v * r_cb + r_orig) / d
                g_new = (v * g_cb + g_orig) / d
                b_new = (v * b_cb + b_orig) / d

                p_new[i,j] = (int(r_new), int(g_new), int(b_new))

        self.image = image_new

        return self.image

    def _convert_monochrome(self):
        image_new = Image.new("RGB", (self.width, self.height), "white")
        p_new = image_new.load()
        for i in range(self.width):
            for j in range(self.height):
                p_old = self.image.getpixel((i,j))
                r_old = p_old[0]
                g_old = p_old[1]
                b_old = p_old[2]
                g_new = (r_old * 0.299) + (g_old * 0.587) + (b_old * 0.114)
                p_new[i,j] = (int(g_new), int(g_new), int(g_new))
        self.image = image_new
        return

    def writeImage(self):
        outfilename = '{:}_{:}{:}'.format(self.prefix, self.suffix, self.extension)
        self.image.save(outfilename)
        return

import __main__ as main
if(__name__ == '__main__' and hasattr(main, '__file__')):

    ############################################################
    # Command line parsing.
    ############################################################

    import argparse
    import os
    import textwrap

    cb_types = {
        'Normal': '(normal vision)',
        'Protanopia': '(red-blind)',
        'Deuteranopia': '(green-blind)',
        'Tritanopia': '(blue-blind)',
        'Protanomaly': '(red-weak)',
        'Deuteranomaly': '(green-weak)',
        'Tritanomaly': '(blue-weak)',
        'Monochromacy': '(totally colorblind)'
    }
    description = textwrap.dedent(
    """
    This script is used to convert "normally" colored images to a representation
    similar to what a colorblind viewer might see.   The types of colorblindness
    that can be simulated are:

    """ + '    '.join(['* {:} {:}\n'.format(k,v) for k,v in cb_types.items()]) +

    """
    with the default action to convert to 'All' types of colorblindness (and to
    a normal vision version).  Converting to only a select type of
    colorblindness can be accomplished with the CB parameter described below.

    The conversion processes and coefficients herein are used with permission
    from Colblindor [http://www.color-blindness.com/] and were therein used with
    permission of Matthew Wickline and the Human-Computer Interaction Resource
    Network [http://www.hcirn.com/] for non-commercial purposes.  As such, this
    code may only be used for non-commercial purposes.
    """)

    epilog = textwrap.dedent(
    """
    Typical command line calls might look like:

    > python """ + os.path.basename(__file__) + """ <inputimage>
    """ + u"\u2063")

    parser = argparse.ArgumentParser(
        formatter_class=argparse.RawDescriptionHelpFormatter,
        description=description, epilog=epilog)

    # Add required, positional, arguments.
    parser.add_argument(
        'inputimage', type = str,
        help = 'input image file name')

    # Add optional, named, arguments.
    parser.add_argument(
        '--cb', type = str, default = 'All',
        help = 'type of colorblindness to convert to (default: All)')

    args = parser.parse_args()

    # Basic error checking of command line options.
    assert(os.path.isfile(args.inputimage)), \
        'Input image (' + args.inputimage + ') not found.'

    ############################################################
    # Program execution.
    ############################################################

    image = ColorBlindConverter(args.inputimage)
    if(args.cb == 'All'):
        image.convert('Normal')
        image.writeImage()
        image.convert('Monochromacy')
        image.writeImage()
        image.convert('Protanopia')
        image.writeImage()
        image.convert('Deuteranopia')
        image.writeImage()
        image.convert('Tritanopia')
        image.writeImage()
        image.convert('Protanomaly')
        image.writeImage()
        image.convert('Deuteranomaly')
        image.writeImage()
        image.convert('Tritanomaly')
        image.writeImage()
    else:
        if(args.cb in cb_types.keys()):
            image.convert(args.cb)
            image.writeImage()
        else:
            print('Cannot find colorblindness type: {:}.'.format(args.cb))
            print('For valid conversion types, see --help.')
            print('Exiting.')

