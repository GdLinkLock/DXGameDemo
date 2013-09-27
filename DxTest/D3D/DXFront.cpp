#include "DXFront.h"
#include "../Log/ErrorInfo.h"
#include "D3DUtil.h"

DXText::DXText(IDirect3DDevice9* pDevice):m_pDevice(pDevice)
{
	m_pFont=0;
	HRESULT hr;
	hr=D3DXCreateFont(m_pDevice,15,12,FW_BOLD,1,FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,DEFAULT_QUALITY,
						DEFAULT_PITCH|FF_DONTCARE,"The Times NewMan",&m_pFont);
	if (FAILED(hr))
	{
		PopupError("Create Font Error!");
	}
}

void DXText::PushText(int x,int y,float* color,const char* szString...)
{
	char szBuffer[1024];
	va_list va;
	va_start(va,szString);
	vsprintf_s(szBuffer,szString,va);//注意：这是不能使用sprintf_s，但是为什么呢？
	va_end(va);

	TextItem* p=new TextItem;
	p->SetPos(x,y);
	p->SetColor(color);
	p->SetText(szBuffer);
	
	m_TextCon.push_back(p);
}

void DXText::RenderText()
{
	if (!m_pFont)
		return;
	RECT rect;
	std::vector<TextItem*>::iterator itr=m_TextCon.begin();
	while (itr != m_TextCon.end())
	{
		TextItem* p=*itr;
		SetRect(&rect,p->m_x,p->m_y,1024,768);
		m_pFont->DrawTextA(0,p->GetTextString(),-1,&rect,DT_LEFT,D3DXCOLOR(p->r,p->g,p->b,1.0f));
		++itr;
	}
	m_TextCon.clear();
}
void DXText::ClearTextCon()
{
	std::vector<TextItem*>::iterator itr=m_TextCon.begin();
	while (itr != m_TextCon.end())
	{
		TextItem* p=*itr;
		delete p;
		p=0;
		++itr;
	}
	m_TextCon.clear();
}

 DXText::~DXText()
{
	ClearTextCon();
	SAFE_RELEASE(m_pFont);
}

 void DXText::OnLostDevice()
 {
	 if(m_pFont)
		 m_pFont->OnLostDevice();
 }

 void DXText::OnResetDecive()
 {
	 if (m_pFont)
	 {
		 m_pFont->OnResetDevice();
	 }
 }

 TextItem::TextItem()
 {
	 m_text="";
	 m_x=0;
	 m_y=0;
	 r=g=b=0;
 }

 void TextItem::SetColor(float* col)
 {
	 r=col[0];
	 g=col[1];
	 b=col[2];
 }
 void TextItem::SetPos(int x,int y)
 {
	 m_x=x;
	 m_y=y;
 }

 void TextItem::SetText(const char* str)
 {
	 m_text=str;
 }

 const char* TextItem::GetTextString()
 {
	 return m_text.c_str();
 }
  TextItem::~TextItem()
 {

 }