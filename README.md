# CineIA_CLI

Command line tool for encoding IMF IAB into (Atmos compatible) DCP IAB.

![GitHub Release](https://img.shields.io/github/v/release/izwb003/CineIA_CLI)
![GitHub last commit](https://img.shields.io/github/last-commit/izwb003/CineIA_CLI)
![GitHub License](https://img.shields.io/github/license/izwb003/CineIA_CLI)


[简体中文](/README.CN.md)

## Introduction

CineIA is a re-encoding tool aims at creating DCP assets containing Immersive Audio Bitstreams that meet [IAB Application Profile 1 (SMPTE RDD57:2021)](https://doi.org/10.5594/SMPTE.RDD57.2021), to available immersive audio cinemas that support (or compatible with) this constraint (e.g. Dolby Atmos® cinemas) to play audio content with a complete immersive experience.

## Using Guide

### Help instructions
CineIA_CLI is a command line tool. Command line help(```cineia -h```):

```
Usage: cineia [-n] [-f] [-c <number>] [-o <number>] <input file path> <output file path> [-l] [-h]

<input file path>: Full or relative path to the IMF IAB file for conversion.
<output file path>: Full or relative path to the DCP IAB file output.


Option Summary:

-n, --no-copy-preamble       Do not copy the "PreambleValue" part from IMF IAB bitstream.
                             Try using this argument if the output bitstream does not work fine.
-f, --force-dolby-constraint Modify the bitstream to ensure that every detail conforms to the Dolby constraint.
                             Try using this argument if the output bitstream causes error,
                             but it may cause the bitstream to not work as expected.
-c, --set-channels <number>  Set the bed channel number in MXF AtmosDescriptor.
                             Do not set unless you meet some problem.
-o, --set-objects <number>   Set the object number in MXF AtmosDescriptor.
                             Do not set unless you meet some problem.
-l, --show-licenses          Show open-source licenses and quit.
-h, --help                   Print this help message and quit.
```

### Usage
This tool accepts IAB resources from Interoperable Master Format (IMF) packages as input. The input content needs to comply with [Dolby Atmos® IMF IAB Interoperability Guidelines](https://professionalsupport.dolby.com/s/article/Dolby-Atmos-IMF-IAB-interoperability-guidelines?language=en_US). Most Dolby Atmos® renderer can generate such files.

*Notice: Please ensure that the parameters such as loudness of your input content have been adjusted correctly. Tools like [DCP-o-matic 2](https://dcpomatic.com/) cannot directly adjust the gain of Atmos assets.*

*Tip: If you have other formats of Dolby Atmos® Master files (such as .atmos, ADM BWF, etc.), please use the free tool [Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/) to convert it to IMF IAB format.*

*Tip: If the IMF IAB files you hold can cause problems, please try to add option ```-f```, or prioritize using the [Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/) to convert the file back to IMF IAB. The IMF IAB files created by this tool are more in line with the specification.*

After obtaining the IMF IAB ```.mxf``` file, use CineIA_CLI to convert it to DCP IAB. Run:

```sh
cineia "IMF IAB file name or path.mxf" "DCP IAB file name or path.mxf"
```

The program will automatically verify, display information, and complete the conversion.

The output DCP IAB file can be directly used by most DCP creators that support IAB/Atmos assets. Using [DCP-o-matic 2](https://dcpomatic.com/) as an example:

- Under the "Content" tab, click "Add File..." and select the DCP IAB ```.mxf``` file that you have successfully converted.
- Set "Channel" to 14 in the "Audio" tab under the "DCP" tab.
- In addition, you can also add regular stereo/5.1/7.1 audio files and set them according to normal operation. They will be played when the decoder is unable to play DCP IAB, such as when it is not supported or there are errors.
- Add your other contents to build a complete DCP and render it for export.

Next, in the cinema where you need to play the content, simply load DCP and play it according to the way you copy IAB/Atmos copies.

## Technical Information

### Building Instructions

CineIA_CLI is a CMake project. Just follow the CMake usage method.

Project dependency [asdcplib](https://github.com/cinecert/asdcplib) needs [Xerces-C++](https://xerces.apache.org/xerces-c/) and [OpenSSL](https://www.openssl.org/) as dependencies. Please configure these dependencies correctly before starting build.

*Hint: If you encounter difficulties in handling dependencies under Windows, you can directly specify the following CMake macros as the corresponding librarys' ```.lib``` file location.*

```sh
-Wno-dev
-DXercescppLib_PATH="path/to/xerces-c_3.lib"
-DOpenSSLLib_PATH="path/to/libcrypto.lib"
-DXercescppLib_include_DIR="path/to/xerces-c-3.2.4/src"
-DXercescppLib_Debug_PATH="path/to/xerces-c_3D.lib"
```

### Why IMF IAB?
IMF IAB, defined in [ST 2067-201](https://doi.org/10.5594/SMPTE.ST2067-201.2019), is a [ST 2098-2](https://doi.org/10.5594/SMPTE.ST2098-2.2019) Expansion. Except for some constraints, it is the same as DCP IAB (or Dolby Atmos® cinema bitstream). The specifications of the bitstream are consistent. By directly adjusting its constraints, it can be quickly changed into DCP IAB.

In addition, the bitstream requires a section called "Preamble" (a.k.a. "IABPCMSubFrame" in early 25CSS discussions). It contains a segment of data typically 1603 bytes (varies by frame rate) for use in bitstream decoding operation. The definition and specifications of this section have not yet been made public. Therefore, it is necessary to use the IMF IAB format to copy the Preamble generated by the IMF IAB encoder when encoding the generated DCP IAB. Otherwise, the generated DCP IAB may malfunction during playback.

*Tip: You may override this operation by using option ```-n```, but it is strongly not recommended to do so.*

## Open Source Licenses and Acknowledgements
The birth of CineIA cannot be separated from [asdcplib](https://github.com/cinecert/asdcplib) library and [iab-renderer](https://github.com/DTSProAudio/iab-renderer) library. Most of the implemention in CineIA was made by these libraries. Please ensure to refer to the open source licenses for these libraries. Run ```cineia -l``` for more information.

The progress bar, full of artistic sense, was made by MIT-licensed library [indicators](https://github.com/p-ranav/indicators).

The development and debugging of CineIA cannot be separated from [@筱理-Rize](https://space.bilibili.com/3848521/) and [@神奇的红毛丹](https://space.bilibili.com/364856318)'s encouragement and support. Also appreciate [@冷小鸢aque](https://space.bilibili.com/27063907) tirelessly contacting and assisting with in-cinema testing.

CineIA_CLI was open-sourced under [MIT License](https://opensource.org/license/mit/).

## Announcement

**CineIA is an [IAB Application Profile 1](https://doi.org/10.5594/SMPTE.RDD57.2021) DCP IAB generator. It CANNOT replace a Dolby Atmos® Cinema MXF generator's work, although they are compatible. Therefore, the output of this program cannot represent the product quality of Dolby Laboratories (and/or other entities). This program can ONLY be used for UGC content production and CANNOT be used for professional/commercial content production work. For professional/commercial content distribution needs, please contact professional content distributers.**

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.**
