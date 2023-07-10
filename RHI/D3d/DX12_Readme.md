# DX12

## 目录

[TOC]

## 1. 图形管线概述

--计算着色器（CS）
--输入组装器（IA）阶段
--顶点着色器（VS）阶段
--外壳着色器（HS）阶段
--细分器		（TS）阶段
--域着色器	（DS）阶段
--几何着色器（GS）阶段
--流输出		（SO）阶段
--光栅化		（RS）阶段
--像素着色器（PS）阶段
--输出合并	（OM）阶段

![image-20230515151432031](D:\Engine\GameEngineStart\GameEngine\RHI\Readme\image-20230515151432031.png)

## 2. D3D工作原理概述

--device
--PSO
--Command List 
--Bundls 
--Command Queues
--Command Allocators
--Resources
--Descriptors (Resource Views)
--Descriptor Tables
--Descriptor Heaps
--Root Signatures
--Resource Barriers
--Fences and Fence Event
--Overview of Application Flow Control for Direct3D 12
--Multithreading in D3D

## 3. 管线状态对象（PSO）

​		管线状态对象由接口ID3D12PipelineState表示。创建PSO需要填写D3D12_GRAPHICS_PIPELINE_STATE_DESC结构，当管线状态对象被设置时，这个结构将决定管线的状态，大多数管线状态都可以在PSO内设置，但有一些需要由CommandList（命令列表）设置。

可以在PSO中设置的管线状态：

--顶点(vertex)、像素(pixel)、域(domain)、外壳(hull)、几何(Geometry)着色器的字节码(D3D12_SHADER_BYTECODE)
--流输出缓冲区(D3D12_STREAM_OUTPUT_DESC)
--混合状态(D3D12_BLEND_STATE)
--光栅器状态(D3D12_RASTERIZER_DESC)
--深度/模板状态(D3D12_DEPTH_STENCIL_DESC)
--输入布局(D3D12_INPUT_LAYOUT_DESC)
--原始拓扑(D3D12_PRIMITIVE_TOPOLOGY_TYPE)
--RTV数量()
--RTV格式(RTV format  DXGI_FORMAT)
--DSV格式(DSV format  DXGI_FORMAT)
--采样结构(DXGI_SAMPLE_DESC)

通过命令列表设置的状态：

--资源绑定(包括顶点缓冲区(vertex buffer)，索引缓冲区(index buffer)，流输出目标(stream output targets)，渲染目标(render targets)，描述符堆(descriptor heaps)，图形根参数(graphics root parameter))
--视口(viewports)
--裁剪矩形(scissor rectangles)
--混合因子(blend factor)
--深度/模板参考值(Depth/Stencil reference value)
--图元类型和邻近类型(primitive topology order and adjacency type)

PSO还有一些执行多个命令列表时的问题。

Pipeline states that were set by a *Pipeline State Object* are not inherited by *Command Lists* (pipeline states that were set by *Pipeline State Objects* from previous command lists when a *Command Queue* executes more than one *Command List* at a time are not inherited by the next *Command List* in the queue) or *Bundles* (pipeline states set by *Pipeline State Objects* from the calling *Command List* are not inherited by *Bundles*). The initial graphics pipeline state for both *Command Lists* and *Bundles* are set at creation time of the *Command List* or *Bundle*. Pipeline states that were not set by a *Pipeline State Object* also are not inherited by *Command Lists*. *Bundles* on the other hand, inherit all graphics pipeline states that are not set with a *Pipeline State Object*. When a *Bundle* changes the pipeline state through a method call, that state persists back to the *Command List* after the *Bundle* has finished executing. 

The default graphics pipeline states not set by *Pipeline State Objects* for *Command Lists* and *Bundles* are:

--The Primitive Topology is set to [D3D_PRIMITIVE_TOPOLOGY_UNDEFINED](https://msdn.microsoft.com/en-us/library/windows/desktop/ff728726(v=vs.85).aspx#D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) 

--Viewports are set to all zeros 

--Scissor Rectangles are set to all zeros 

--Blend factor is set to zeros 

--Depth/Stencil reference value is set to zeros

--Predication is disabled 

You can set the pipeline state for a *Command List* back to defaults by calling the [ClearState](https://msdn.microsoft.com/en-us/library/windows/desktop/dn903847(v=vs.85).aspx) method. If you call this method on a *Bundle*, the call to the command lists "close()" function will return *E_FAIL*. 

Resource bindings that are set by *Command Lists* are inherited by *Bundles* that the command list executes. Resource bindings that are set by *Bundles* also stay set for the calling command list when the bundle finishes executing.

### 4. 设备接口 Device

device用来创建命令列表、PSO、根签名、命令分配器、命令队列、栅栏、资源、描述符和描述符堆。device是通过寻找GPU来创建的。

### 5. 命令列表 CommandList

​		使用命令列表来分配需要在GPU上执行的命令。命令可能包括设置管线状态、设置资源、转换资源状态（资源屏障）、设置顶点/索引缓冲区、绘制、清除渲染目标、设置渲染目标视图、执行包（命令组）等。

​		命令列表与命令分配器关联，命令分配器将命令存储在GPU上。首次创建命令列表时需要指定命令列表的类型，并提供与列表相关联的命令分配器，命令列表有4种类型：直接(Direct)、捆绑(Bundle)、计算(Compute)、复制(Copy)。

​		直接命令列表是GPU可以直接执行的命令列表，直接命令列表与直接命令分配器相关联。要将命令列表设置为录制状态使用Reset()方法。使用close()将命令列表设置为不记录状态。调用close()后可以使用命令队列来执行命令列表。一旦执行命令列表，就可以重置它，即时GPU没有完成执行(这里应该是CPU和GPU异步)，这时可以重新给命令列表分配内存(在CPU端重新分配，在GPU端命令由Command Allocator存储在内存中)。

### 6. 命令队列 CommandQueue

![QQ截图20230516084826](D:\Engine\GameEngineStart\GameEngine\RHI\Readme\QQ截图20230516084826.png)

​		命令队列用来提交要由GPU执行的命令列表。命令队列还用于更新资源映射。

### 7. 命令分配器 Command Allocator

​		命令分配器代表存储来自命令列表和捆绑包的命令的内存。命令列表执行完成后可以在命令分配器上调用reset()来释放内存。命令列表可以在调用执行后立即Reset()，但是与这个命令列表关联的命令分配器必须在GPU执行完成后才能调用Reset()，否则可能会调用返回失败。这是因为GPU可能正在执行命令分配器所代表的内存空间中存储的命令。所以我们需要Fence来同步CPU和GPU。在调用命令分配器的Reset()，必须使用Fence检查来保证和执行的命令列表相关联的命令分配器已经执行完成。与命令列表关联的命令分配器在任何时候都处于记录状态。这意味着对于多线程时的每个填充命令列表的线程，都需要一个命令列表和一个命令分配器。

### 8. 资源 Resources

​		d3d中资源类型包括：

--Texture1D
--Texture1DArray
--Texture2D
--Texture2DArray
--Texture2DDMS
--Texture2DDMSArray
--Texture3D
--Buffers(ID3D12Resource)

资源视图(View/)类型包括：

--Constant buffer view (CBV)
--Unordered access view (UAV)
--Shader resource view (SRV) 
--Samplers 
--Render Target View (RTV) 
--Depth Stencil View (DSV) 
--Index Buffer View (IBV) 
--Vertex Buffer View (VBV) 
--Stream Output View (SOV)

### 9. 描述符 Descriptors(Resource Views)

![QQ截图20230516102911](D:\Engine\GameEngineStart\GameEngine\RHI\Readme\QQ截图20230516102911.png)

描述符仅能存储于描述符堆上，除了一些根描述符，它们只能是CBV以及原始或结构UAV/SRV缓冲区。复杂类型的SRV如Texture2D不能当做根描述符。

### 10. 描述符表 Descriptor Table

![QQ截图20230516104155](D:\Engine\GameEngineStart\GameEngine\RHI\Readme\QQ截图20230516104155.png)

​		描述符表是描述符堆中的一组描述符，描述符表的全部内容是描述符堆中的偏移量和长度。

​		着色器可以通过根签名的描述符表按索引访问描述符堆中的描述符，因此要访问着色器中的描述符，将索引到根签名描述符表。

​		CBS，UAV，SRV和采样器都存储在描述符堆里，并且可以通过着色器的描述符引用。

​		RTV，DSV，IBV，VBV，SOV不通过描述符表引用，而是直接绑定到管线内。

### 11. 描述符堆 Descriptor Heap

​		描述符堆是一个描述符的列表，用于存储描述符的内存块。

​		采样器(Samplers)不能和资源(Resources)放在同一个描述符堆里。描述符堆只有Shader Visible和Non-Shader Visible两种类型。Shader Visible类型的描述符堆包含了shader可以获取的描述符，这个堆内可以包含包括CBV，UAV，SRV和采样器描述符。Non-Shader Visible的描述符堆存储了shader不可见的描述符，包括RTV，DSV，IBV，VBV，SOV资源类型。

​		在任意给定时间，只有一个shader visible的描述符堆和一个采样器堆被绑定到管线中。

​		...

### 12. 根签名 RootSignature

​		根签名定义着色器访问的数据(资源)。根签名包括根常量、根描述符和描述符表。根参数是根签名中的一个条目，可以是根常量、根描述符或描述符表。应用程序可以更改的根参数的实际数据称为Root Arguments。

​		根签名的最大大小始终为64DWORDS.

​		根常量是一个1DWORD，直接存储在根签名中，这些值是着色器的常量缓冲区，从着色器访问根常量无需定向，所以根常量的访问是最快的。

​		根描述符占用2个DWORD，仅限于CBV和SRV/UAV使用，不能支持Texture2D的SRV这种复杂的类型。从着色器引用根描述符时有一次定向成本。根描述符只是指向资源的指针，不包括数据的大小，这意味着从根描述符访问资源时没有越界检查，而描述符堆中描述符是有大小的，可以进行越界检查的。

