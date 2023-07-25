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

#define BIT_MASK(a) (1 << a)
#define TEST_BIT(a, bit) (BIT_MASK(bit) & a)
}
