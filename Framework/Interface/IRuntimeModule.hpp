#pragma once

#include "Interface.hpp"

namespace My{
	
	Interface IRuntimeModule {
	public:
		virtual ~IRuntimeModule() {}		//当使用基类指针释放派生类实例时，可能只调用了基类的析构函数，会造成memory leak
		
		virtual int Initialize() = 0;		//初始化模块时使用
		virtual void Finalize() = 0;		//模块释放时使用
		virtual void Tick() = 0;			//使驱动模块调用该模块执行，每调用一次，模块进行一个单位的处理
		
	};
}
