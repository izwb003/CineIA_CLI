# CineIA_CLI

将IMF IAB编码为（全景声兼容）的DCP IAB的命令行工具。

## 简介

CineIA是一个重新编码工具，旨在制作包含遵循[IAB应用配置文件1 (SMPTE RDD57:2021)](https://doi.org/10.5594/SMPTE.RDD57.2021)的沉浸式声音比特流的DCP资源，以允许支持（或兼容）该约束的沉浸式声音电影院（例如杜比全景声®电影院）以完整的沉浸式体验播放音频。

## 使用说明

### 帮助说明
CineIA_CLI是一个命令行工具。参数帮助如下（```cineia -h```）：

```
用法: cineia [-n] [-f] [-c <number>] [-o <number>] <input file path> <output file path> [-l] [-h]

<input file path>: 要转换的IMF IAB文件的完整或相对路径。
<output file path>: 输出的DCP IAB文件的完整或相对路径。


参数简介:

-n, --no-copy-preamble       不要从比特流中复制“PreambleValue部分”。
                             如果输出的比特流不能正常工作，尝试添加此选项重新编码。
-f, --force-dolby-constraint 调整比特流，使其强制符合杜比约束。
                             如果输出比特流会导致错误，尝试添加此选项重新编码。
                             但它可能导致比特流工作不正常。
-c, --set-channels <number>  设置MXF AtmosDescriptor记录的音床声道数。
                             除非遇到问题，否则请勿修改。
-o, --set-objects <number>   设置MXF AtmosDescriptor记录的声音对象数。
                             除非遇到问题，否则请勿修改。
-l,     --show-licenses      显示开放源代码许可并退出。
-h,     --help               显示此帮助信息并退出。
```

### 使用方法
该工具接受可互操作母版格式（IMF）包的IAB资源作为输入。输入内容需要符合[杜比IMF IAB指导约束](https://professionalsupport.dolby.com/s/article/Dolby-Atmos-IMF-IAB-interoperability-guidelines?language=en_US)。大多数杜比全景声®渲染器可以生成此类文件。

*注意：请确保已经调整正确您的输入内容的响度等参数。[DCP-o-matic 2](https://dcpomatic.com/)等工具不能直接调整全景声资源的增益。*

*提示：若您持有其它格式的杜比全景声®母版文件（如.atmos，ADM BWF等），请使用免费工具[Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/)将其转换为IMF IAB格式。*

*提示：如果您持有的IMF IAB文件会导致问题，请尝试添加选项```-f```或优先尝试使用[Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/)重新转换文件为IMF IAB。该工具创建的IMF IAB文件较为符合规范。*

获取到IMF IAB ```*.mxf```文件后，使用CineIA_CLI转换其为DCP IAB。运行：

```sh
cineia "IMF IAB文件名或文件路径.mxf" "DCP IAB文件名或文件路径.mxf"
```

程序将自动校验、显示信息并完成转换。

输出的DCP IAB文件可以直接被大多数支持沉浸式声音比特流或全景声资源的DCP制作工具识别并正确封装。以[DCP-o-matic 2](https://dcpomatic.com/)为例：

- 在“内容”选项卡下点选“添加文件...”，选择您转换成功的DCP IAB```.mxf```文件。
- 在“DCP”选项卡下的“音频”选项卡中设置“通道”为14。
- 此外，您同样可以添加普通的立体声/5.1/7.1音频文件，并按正常操作设置。它们将在解码器无法播放DCP IAB时被播放，例如不支持或发生错误。
- 添加您的其它内容构建完整DCP，并渲染导出。

接下来，在您需要播放内容的影厅中，按照对待IAB或全景声拷贝的方式加载DCP并播放即可。

## 技术信息

### 构建指南

CineIA_CLI是一个CMake项目。按CMake使用方法对待即可。

项目依赖[asdcplib](https://github.com/cinecert/asdcplib)需要[Xerces-C++](https://xerces.apache.org/xerces-c/)及[OpenSSL](https://www.openssl.org/)作为依赖。开始构建前请正确配置这些依赖。

*提示：在Windows下处理依赖若遇到困难，您可以直接指定以下CMake宏为对应的库的```.lib```文件位置。*

```sh
-Wno-dev
-DXercescppLib_PATH="path/to/xerces-c_3.lib"
-DOpenSSLLib_PATH="path/to/libcrypto.lib"
-DXercescppLib_include_DIR="path/to/xerces-c-3.2.4/src"
-DXercescppLib_Debug_PATH="path/to/xerces-c_3D.lib"
```

### 为什么使用IMF IAB格式？
IMF IAB，定义于[ST 2067-201](https://doi.org/10.5594/SMPTE.ST2067-201.2019)，是一种基于沉浸式声音比特流规范[ST 2098-2](https://doi.org/10.5594/SMPTE.ST2098-2.2019)的扩展。除部分约束不同外，其同DCP IAB（或影院杜比全景声®比特流）的规范一致。通过直接调整其约束，可以很快捷地将其变更为DCP IAB。

此外，比特流需要一段名为“前序”（```Preamble```）（在早期25CSS讨论中也被称为```IABPCMSubFrame```）的部分。其包含一段一般为1603字节（随帧率变化）的数据用于比特流解码操作。这一部分的定义及规范尚未公开。故此，必须使用IMF IAB格式，以将编码IMF IAB时由编码器生成的该部分复制到生成的DCP IAB中。否则，生成的DCP IAB在播放时可能发生异常。

*提示：您可以添加选项```-n```来跳过这个操作，但是强烈不建议这样做。*

## 开放源代码许可与致谢
CineIA的诞生离不开[asdcplib](https://github.com/cinecert/asdcplib)库和[iab-renderer](https://github.com/DTSProAudio/iab-renderer)库。这些库在CineIA中完成了大部分的实现。请确保参阅这些库的开源协议。运行```cineia -l```获取更多信息。

那个充满艺术感的进度条是通过MIT许可证开源的库[indicators](https://github.com/p-ranav/indicators)实现的。

CineIA的开发与调试离不开[@筱理_Rize](https://space.bilibili.com/3848521/)和[@神奇的红毛丹](https://space.bilibili.com/364856318)的鼓励和支持，及[@冷小鸢aque](https://space.bilibili.com/27063907)不辞辛劳地联系协助进行实地测试。

CineIA_CLI是在[MIT许可证](https://opensource.org/license/mit/)下开源的开源项目。

## 声明
**CineIA是一个[IAB应用配置文件1](https://doi.org/10.5594/SMPTE.RDD57.2021) DCP IAB生成器。它不能取代Dolby Atmos® Cinema MXF生成器的工作，尽管它们是兼容的。因此，该工具的输出不能代表杜比实验室（和/或其它实体）的产品质量。该工具仅可用于UGC内容制作应用，不可用于专业/商业发行工作。针对专业/商业发行需求，请与专业内容发行机构联系。**

**本软件是AS IS的，不提供任何保证，不管是显式的还是隐式的，包括但不限于适销性保证、适用性保证、非侵权性保证。在任何情况下，对于任何的权益追索、损失赔偿或者任何追责，作者或者版权所有人都不会负责。无论这些追责产生自合同、侵权，还是直接或间接来自于本软件以及与本软件使用或经营有关的情形。**

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.**