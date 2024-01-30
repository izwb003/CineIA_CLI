# CineIA_CLI
Command line tool for encoding IMF IAB into Atmos compatible DCP IAB.

[简体中文](/README.CN.md)

## Introduction

### Background
Dolby Atmos® greatly enhanced the immersive experience in cinemas. However, The production of Atmos contents in cinema DCP (Atmos copy) requires the use of Dolby specific tools or RMU. That is to say, there are currently no publicly accessible tools available for making Atmos copies. This poses an obstacle to fully utilizing the capabilities of a Atmos theater in situations where it is necessary to play self-made cinema content, such as movie viewing events.

However, SMPTE®'s new standard for the release of immersive audio content in next-generation cinemas - Immersive Audio Bitstream (IAB), also known as [ST 2098](https://doi.org/10.5594/SMPTE.ST2098-2.2019) series standards fully disclose Dolby Atmos® technical details from a lateral perspective. Some registration disclosure documents, such as [RDD 29](https://doi.org/10.5594/SMPTE.RDD29.2019), [RDD 57](https://doi.org/10.5594/SMPTE.RDD57.2021) also disclosed the constraints of Dolby Atmos® bitstream.

CineIA is a re-encoding tool aimed at creating Immersive Audio Bitstreams that meet these constraints, indirectly available Dolby Atmos® Cinemas to play audio content with a complete immersive experience, unleashing Dolby Atmos® Cinemas' maximum capacity.

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

**Notice: Please ensure that the parameters such as loudness of your input content have been adjusted correctly. Tools like [DCP-o-matic 2](https://dcpomatic.com/) cannot directly adjust the gain of Atmos assets.**

If you have other formats of Dolby Atmos® Master files (such as .atmos, ADM BWF, etc.), please use the free tool [Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/) to convert it to IMF IAB format.

***Tip: If the IMF IAB files you hold can cause problems, please prioritize using the [Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/) to convert the file back to IMF IAB. The IMF IAB files created by this tool are more in line with the Dolby specification.***

After obtaining the IMF IAB ```.mxf``` file, use CineIA_CLI to convert it to DCP IAB. Run:

```sh
cineia "IMF IAB file name or path.mxf" "DCP IAB file name or path.mxf"
```

**Notice: The program will overwrite the output file. Make sure you use the correct file name to avoid accidentally overwriting important content.**

The program will automatically verify, display information, and complete the conversion.

Before merging into DCP, please ensure that you have used a Dolby Atmos® renderer such as [Dolby Atmos Renderer](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-renderer/) to check the output file. If the renderer you are using does not support reading and writing Cinema MXF, please reuse [Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/) to convert it back to common formats such as ADM BWF before importing into the renderer. Be sure to perform a complete playback and check for any issues.

The output DCP IAB file can be directly used by most DCP creators that support Atmos assets. Using [DCP-o-matic 2](https://dcpomatic.com/) as an example:

- Under the "Content" tab, click "Add File..." and select the DCP IAB ```.mxf``` file that you have successfully converted.
- Set "Channel" to 14 in the "Audio" tab under the "DCP" tab.
- In addition, you can also add regular stereo/5.1/7.1 audio files and set them according to normal operation. When Dolby Atmos® is not supported in the cinema, the sound will be played in the added stereo/5.1/7.1 format. In Dolby Atmos® Cinema, the sound will be played in the immersive audio effect. If you have not added regular stereo/5.1/7.1 audio in a cinema that does not support Dolby Atmos®, the sound will not be able to play.
- Add your other contents to build a complete DCP and render it for export.

Next, in the Dolby Atmos® Cinema where you need to play the content, simply load DCP and play it according to the way you copy Atmos copies.

## Technical Information

### Building Instructions
**Notice: The program was written in MSVC. The code may contain several non-standard implementations. Not yet debugged under other compilers such as GCC.**

**Notice: As of now, the program has only been tested and debugged under Windows x64. Debugging has not been carried out for other environments yet.**

CineIA_CLI is a CMake project. Just follow the CMake usage method.

Project Dependency [asdcplib](https://github.com/cinecert/asdcplib) need [Xerces-C++](https://xerces.apache.org/xerces-c/) and [OpenSSL](https://www.openssl.org/) as dependencies. Please configure these dependencies correctly before starting build.

**Hint: If you encounter difficulties in handling dependencies under Windows, you can directly specify the following CMake macro as the corresponding librarys' ```.lib``` file location.**

```sh
-Wno-dev
-DXercescppLib_PATH="path/to/xerces-c_3.lib"
-DOpenSSLLib_PATH="path/to/libcrypto.lib"
-DXercescppLib_include_DIR="path/to/xerces-c-3.2.4/src"
-DXercescppLib_Debug_PATH="path/to/xerces-c_3D.lib"
```

### Why IMF IAB?
IMF IAB, defined in [ST 2067-201](https://doi.org/10.5594/SMPTE.ST2067-201.2019), is an [ST 2098-2](https://doi.org/10.5594/SMPTE.ST2098-2.2019) Expansion. Except for some constraints, it is the same as DCP IAB (or Dolby Atmos® Cinema Bitstream). The specifications of Bitstream are consistent. By directly adjusting its constraints, it can be quickly changed to DCP IAB.

In addition, the bitstream requires a section called "Preamble" (also known as "IABPCMSubFrame" in early 25CSS discussions). It contains a segment of data typically 1603 bytes for use in real time monitoring of correct rendering during Dolby Atmos® bitstream decoding operation. The definition and specifications of this section have not yet been made public. Therefore, it is necessary to use the IMF IAB format to copy the Preamble generated by the encoder (such as Dolby Atmos Storage System IDK) when encoding the IMF IAB into the generated DCP IAB. Otherwise, the generated DCP IAB may malfunction during playback due to incorrect monitoring, such as misalignment of sound objects.

## Open Source Licenses and Acknowledgements
The birth of CineIA cannot be separated from [asdcplib](https://github.com/cinecert/asdcplib) library and [iab renderer](https://github.com/DTSProAudio/iab-renderer) library. Most of the implemention in CineIA was made by these libraries. Please ensure to refer to the open source licenses for these libraries. Run ```cineia -l``` for more information.

The development and debugging of CineIA cannot be separated from [@筱理-Rize](https://space.bilibili.com/3848521/) and [@神奇的红毛丹](https://space.bilibili.com/364856318)'s encouragement and support. Also appreciate [@冷小鸢aque](https://space.bilibili.com/27063907) tirelessly contacting and assisting with in-cinema testing.

CineIA_CLI was open-sourced under [MIT License](https://opensource.org/license/mit/).

## Announcement

**CineIA is not related to Dolby Laboratories. The output of this program cannot represent the product quality of Dolby Laboratories. This program can only be used for UGC content production and cannot be used for professional content production work. For professional content distribution needs, please contact Dolby Laboratories.**

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.**