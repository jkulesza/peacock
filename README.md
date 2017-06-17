This script is used to convert "normally" colored images to a representation
similar to what a colorblind viewer might see.   The types of colorblindness
that can be simulated are:

* Normal (normal vision)
* Protanopia (red-blind)
* Deuteranopia (green-blind)
* Tritanpoia (blue-blind)
* Protanomaly (red-weak)
* Deuteranomaly (green-weak)
* Tritanomaly (blue-weak)
* Achromatopsia (totally colorblind)

with the default action to convert to 'All' types of colorblindness (and to
a normal vision version).  Converting to only a select type of
colorblindness can be accomplished with the CB parameter described below.

The conversion processes and coefficients herein are used with permission
from Colblindor [http://www.color-blindness.com/] and were therein used with
permission of Matthew Wickline and the Human-Computer Interaction Resource
Network [http://www.hcirn.com/] for non-commercial purposes.

```
positional arguments:
  inputimage  input image file name

optional arguments:
  -h, --help  show this help message and exit
  --cb CB     type of colorblindness to convert to (default: All)
```

Typical command line calls might look like:

`python peacock.py <inputimage>`
