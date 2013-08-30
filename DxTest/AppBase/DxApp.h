#pragma  once
#include "DxWindown.h"
#include <functional>
#include <memory>

class DxAppBase
{
public:
	DxAppBase(const char* name,unsigned int w,unsigned int h,unsigned int left,unsigned top);
	~DxAppBase();

	void Init();
	void ShudDown();
	void StartRun();
	DxWindown* GetWnd(){return m_pWnd.get();}

protected:
	virtual	void InitApp(){};
	virtual void ShutDownApp(){};
	virtual void Render(float interval){};
	virtual void Update(float interval){};
protected:
	std::tr1::shared_ptr<DxWindown> m_pWnd;
};