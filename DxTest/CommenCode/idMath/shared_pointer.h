#ifndef _MINI_INCLUDE_SHARED_POINTER_H_
#define _MINI_INCLUDE_SHARED_POINTER_H_

#include <stdlib.h>
#include <string>

//基于引用计数的职能指针
//使用同一个原生指针来初始化2个智能指针是不正确的
template<typename T>
class SharedPointer
{
public:

	//使用一个原生指针构造一个sp
	//使用explicit禁止掉SharedPointer<A> sp = new A();的隐式转换。
	//使用member template 保证 SharedPointer<Base> sp( new Deriver() );方便多态。
	template<typename Y>
	explicit SharedPointer(Y* p):
		m_pRP(p),
		m_pCounter((unsigned int*)malloc(sizeof(unsigned int*)))
	{
		*m_pCounter = 1;
	}

	//SharedPointer<Base> spb( (SharedPointer<Deriver>)spd );
	template<typename Y>
	SharedPointer(const SharedPointer<Y>& sp):
		m_pRP(static_cast<T*>(sp.getPointer())),
		m_pCounter(sp.getCounter())
	{
		if (m_pCounter)
		{
			++(*m_pCounter);
		}
	}

	//SharedPointer<Base> spb = (SharedPointer<Deriver>)spd;
	template<typename Y>
	SharedPointer& operator = (const SharedPointer<Y>& sp)
	{
		if(m_pRP == sp.getPointer())
			return *this;

		SharedPointer<T> tmp(sp);
		swap(tmp);
		return *this;
	}

	//析构的时候release
	~SharedPointer()
	{
		release();
	}

	//重写默认的构造函数:空指针
	SharedPointer():
		m_pRP(0),
		m_pCounter(0)
	{

	}

	//重写默认复制构造函数:引用计数加1
	SharedPointer(const SharedPointer& sp):
		m_pRP(sp.m_pRP),
		m_pCounter(sp.m_pCounter)
	{
		if(m_pCounter)
			++(*m_pCounter);
	}

	//重写默认赋值运算符:tmp的构造将新ptr计数加1 tmp的析构将原ptr计数减一
	inline SharedPointer& operator = (const SharedPointer& sp)
	{
		if(this->m_pRP == sp.m_pRP)
			return *this;

		SharedPointer<T> tmp(sp);
		swap(tmp);

		return *this;
	}

	template<typename Y>
	Y* convertTo(){return dynamic_cast<Y*>(m_pRP);}

	inline T& operator * () const {return *m_pRP;}
	inline T* operator -> () const {return m_pRP;} 

	inline bool isNull(void) const {return m_pRP == 0;}
	inline void setNull(void){if(m_pRP){release(); m_pRP = 0; m_pCounter = 0;}}

	inline T* getPointer() const {return m_pRP;}
	inline unsigned int* getCounter() const {return m_pCounter;}
	inline unsigned int usedCount()const {return *m_pCounter;}

protected:

	//计数减一 如果减到0就释放ptr
	inline void release()
	{
		if(m_pCounter)
		{
			if(--(*m_pCounter) == 0)
				destory();
		}
	}

	//释放ptr
	inline void destory()
	{
		delete m_pRP;
		m_pRP = 0;
		free(m_pCounter);
		m_pCounter = 0;
	}

	template<typename Y>
	inline void swap(SharedPointer<Y>& sp)
	{
		std::swap(m_pRP,sp.m_pRP);
		std::swap(m_pCounter,sp.m_pCounter);
	}

	unsigned int* m_pCounter;
	T* m_pRP;
};

#ifndef pointer
#define pointer SharedPointer 
#endif

#endif
