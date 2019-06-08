# DeinterlaceBlend

Filter plugin for AviSynth which performs simple blend deinterlacing.

It is a port of the built-in blend deinterlacer in [VirtualDub].
Color formats tested include RGB24/32, YUY2 and YV12.

[![License: GPL](https://img.shields.io/badge/license-GPL-blue.svg)](https://www.gnu.org/licenses/old-licenses/gpl-2.0.en.html)

## Usage
The filter signature is as follows
```
DeinterlaceBlend( clip )
```

In the test folder there is a sample script which shows the effect of using the filter.

## License
Same base license as AviSynth; GNU GPL v2 or later.

## Contribute
Fork it at [github], hack away and send a pull request.

## Release notes
* 0.0.1 Initial release candidate

[github]: http://https://github.com/jojje/DeinterlaceBlend
[VirtualDub]: https://sourceforge.net/projects/vdfiltermod/