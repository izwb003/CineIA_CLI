# CineIA_CLI
将IMF IAB编码为全景声兼容的DCP IAB的命令行工具。

## 简介

### 背景
杜比全景声®极大地提升了影院沉浸式体验。然而，杜比全景声®影院内容DCP（全景声拷贝）中的Atmos资源的制作需要使用杜比专用工具或RMU。也就是说，截至目前没有公开访问的工具可以用于制作全景声拷贝。这为包场观影活动等需要播放自制影院内容的情形下发挥全景声影厅的最大能力带来了障碍。

但是，SMPTE®面向新一代影院沉浸式声音内容发行的全新标准——沉浸式声音比特流（Immersive Audio Bitstream, IAB），即[ST 2098](https://doi.org/10.5594/SMPTE.ST2098-2.2019)系列标准从侧面完整披露了杜比全景声®比特流的技术细节。一些注册披露文档如[RDD 29](https://doi.org/10.5594/SMPTE.RDD29.2019), [RDD 57](https://doi.org/10.5594/SMPTE.RDD57.2021)等同样披露了杜比全景声比特流的约束。

CineIA是一个重新编码工具，旨在制作符合这些约束的沉浸式声音比特流，从而间接实现在杜比全景声®电影院以完整的沉浸式体验播放音频内容，发挥杜比全景声®影厅的最大能力。

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

**注意：请确保已经调整正确您的输入内容的响度等参数。[DCP-o-matic 2](https://dcpomatic.com/)等工具不能直接调整全景声资源的增益。**

若您持有其它格式的杜比全景声®母版文件（如.atmos，ADM BWF等），请使用免费工具[Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/)将其转换为IMF IAB格式。

***提示：如果您持有的IMF IAB文件会导致问题，请优先尝试使用[Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/)重新转换文件为IMF IAB。该工具创建的IMF IAB文件较为符合杜比规范。***

获取到IMF IAB ```*.mxf```文件后，使用CineIA_CLI转换其为DCP IAB。运行：

```sh
cineia "IMF IAB文件名或文件路径.mxf" "DCP IAB文件名或文件路径.mxf"
```

**注意：程序会覆盖输出文件。确保您使用了正确的文件名，避免不慎覆盖重要内容。**

程序将自动校验、显示信息并完成转换。

在封入DCP之前，请确保您已经使用[Dolby Atmos Renderer](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-renderer/)等杜比全景声®渲染器检查过输出文件。若您使用的渲染器不支持读写Cinema MXF，请先将输出的文件重新使用[Dolby Atmos Conversion Tool](https://professional.dolby.com/product/dolby-atmos-content-creation/dolby-atmos-conversion-tool/)转换回ADM BWF等常见格式再导入渲染器。务必进行完整播放并检查是否存在问题。

输出的DCP IAB文件可以直接被大多数支持全景声®的DCP制作工具识别并正确封装。以[DCP-o-matic 2](https://dcpomatic.com/)为例：

- 在“内容”选项卡下点选“添加文件...”，选择您转换成功的DCP IAB```.mxf```文件。
- 在“DCP”选项卡下的“音频”选项卡中设置“通道”为14。
- 此外，您同样可以添加普通的立体声/5.1/7.1音频文件，并按正常操作设置。在不支持杜比全景声®的影厅中，声音将以所添加的立体声/5.1/7.1格式播放。在杜比全景声®影厅中，声音将以沉浸式声音效果播放。若您未添加普通的立体声/5.1/7.1音频，在不支持杜比全景声®的影厅中声音将无法播放。
- 添加您的其它内容构建完整DCP，并渲染导出。

接下来，在您需要播放内容的杜比全景声®影厅中，按照对待全景声拷贝的方式加载DCP并播放即可。

## 技术信息

### 构建指南
**注意：程序使用MSVC编写，代码中可能包含若干不标准的实现。尚未在GCC等编译器下调试。**

**注意：截至目前，程序仅在Windows x64下测试并调试。尚未针对其它环境进行调试。**

CineIA_CLI是一个CMake项目。按CMake使用方法对待即可。

项目依赖[asdcplib](https://github.com/cinecert/asdcplib)需要[Xerces-C++](https://xerces.apache.org/xerces-c/)及[OpenSSL](https://www.openssl.org/)作为依赖。开始构建前请正确配置这些依赖。

***提示：在Windows下处理依赖若遇到困难，您可以直接指定以下CMake宏为对应的库的```.lib```文件位置。***

```sh
-Wno-dev
-DXercescppLib_PATH="path/to/xerces-c_3.lib"
-DOpenSSLLib_PATH="path/to/libcrypto.lib"
-DXercescppLib_include_DIR="path/to/xerces-c-3.2.4/src"
-DXercescppLib_Debug_PATH="path/to/xerces-c_3D.lib"
```

### 为什么使用IMF IAB格式？
IMF IAB，定义于[ST 2067-201](https://doi.org/10.5594/SMPTE.ST2067-201.2019)，是一种基于沉浸式声音比特流规范[ST 2098-2](https://doi.org/10.5594/SMPTE.ST2098-2.2019)的扩展。除部分约束不同外，其同DCP IAB（或影院杜比全景声®比特流）的规范一致。通过直接调整其约束，可以很快捷地将其变更为DCP IAB。

此外，比特流需要一段名为“前序”（```Preamble```）（在早期25CSS讨论中也被称为```IABPCMSubFrame```）的部分。其包含一段一般为1603字节的数据用于在杜比全景声®比特流解码运行的过程中实时监视正确进行的渲染。这一部分的定义及规范尚未公开。故此，必须使用IMF IAB格式，以将编码IMF IAB时由编码器（如Dolby Atmos Storage System IDK）生成的该部分复制到生成的DCP IAB中。否则，生成的DCP IAB在播放时可能因为错误的监视导致故障（如声音对象错位等异常）。

## 开放源代码许可与致谢
CineIA的诞生离不开[asdcplib](https://github.com/cinecert/asdcplib)库和[iab-renderer](https://github.com/DTSProAudio/iab-renderer)库。这些库在CineIA中完成了大部分的实现。请确保参阅这些库的开源协议。运行```cineia -l```获取更多信息。

CineIA的开发与调试离不开[@筱理_Rize](https://space.bilibili.com/3848521/)和[@神奇的红毛丹](https://space.bilibili.com/364856318)的鼓励和支持，及[@冷小鸢aque](https://space.bilibili.com/27063907)不辞辛劳地联系协助进行实地测试。

CineIA_CLI是在[MIT许可证](https://opensource.org/license/mit/)下开源的开源项目。

## 声明
**CineIA与杜比实验室无关。该工具的输出不能代表杜比实验室的产品质量。该工具仅可用于UGC内容制作应用，不可用于专业发行工作。针对专业发行需求，请与杜比实验室联系。**

**本软件是AS IS的，不提供任何保证，不管是显式的还是隐式的，包括但不限于适销性保证、适用性保证、非侵权性保证。在任何情况下，对于任何的权益追索、损失赔偿或者任何追责，作者或者版权所有人都不会负责。无论这些追责产生自合同、侵权，还是直接或间接来自于本软件以及与本软件使用或经营有关的情形。**

**THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.**