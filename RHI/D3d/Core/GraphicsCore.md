# D3dGraphicsCore

## 1.Resource/Buffer

GpuResource
GpuBuffer
BufferManager(ignore now)
ColorBuffer
DepthBuffer
PixelBuffer
ShadowBuffer
UploadBuffer
LinearAllocator
BuddyAllocator
Texture
TextureManager

## 2.Command

CommandAllocatorPool
CommandContext
CommandLineArg
CommandListManager
CommandSignature



## 3.Pipeline

PipelineState

### 3.2 RootSignature

针对根签名，其中RootParameter是用于定义根签名中的根参数，每个根参数可以表示一个常量缓冲区、描述符表、根描述符等。

在枚举D3D12_ROOT_PARAMETER_TYPE中可以看到，根参数类型为CBV/SRV/UAV或者描述符表。

描述符表实际上是一个数组，其中每一个元素都是根描述符。所以根描述符可以是CBV/SRV/UAV。

然後对于根描述符表的初始化有两个函数，一个是**`InitAsDescriptorRange`**，另一个是**`InitAsDescriptorTable`**。

其中第一个是将一个描述符范围添加到根参数中，该描述符范围可以包含一个或者多个描述符，这些描述符可以是常量缓冲区（CBV）、着色器资源视图（SRV）或无序访问视图（UAV）等。该方法会将指定的描述符范围类型、寄存器槽索引、描述符数量等信息存储在根参数中，以供后续创建根签名时使用。

第二个则用于初始化一个根参数为描述符表类型。描述符表是一种根参数，允许您在一个根参数中存放多个描述符，这些描述符可以是不同类型的。通过描述符表，您可以在根签名中设置一个参数，用于在着色器中访问多个资源。这个方法初始化描述符表的信息，例如描述符范围的数量和数组等。

