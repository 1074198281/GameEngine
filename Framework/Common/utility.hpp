#pragma once

namespace My {
	template<class T>
	inline void SafeRelease(T **ppInterfaceToRelease)
	{
		if (*ppInterfaceToRelease != nullptr)
		{
			(*ppInterfaceToRelease)->Release();

			(*ppInterfaceToRelease) = nullptr;
		}
	}

#define BIT_MASK(b)      (1 << (b))
#define SET_BIT(word, bit)    ((word) |= BIT_MASK(bit))
#define CLEAR_BIT(word, bit)    ((word) &= ~BIT_MASK(bit))
#define TEST_BIT(word, bit)   (((word) & BIT_MASK(bit)) != 0)
#define ALL_BITS(type)      (~(type)0))


#define BIG_LITTLE_SWAP32(x)        ( (((*(long int *)&x) & 0xff000000) >> 24) | \
                                      (((*(long int *)&x) & 0x00ff0000) >> 8) | \
                                      (((*(long int *)&x) & 0x0000ff00) << 8) | \
                                      (((*(long int *)&x) & 0x000000ff) << 24) )
}
