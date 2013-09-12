#pragma  once
#include <d3dx9.h>
#include <d3d9.h>
#include <string>
#include <vector>

class TextItem
{
public:
	friend class DXText;
	TextItem();
	~TextItem();

	void SetColor(float* col);
	void SetPos(int x,int y);
	void SetText(const char* str);
	const char* GetTextString();
private:
	std::string m_text;
	int m_x,m_y;
	float r,g,b;
};

class DXText
{
public:
	DXText(IDirect3DDevice9* pDevice);
	~DXText();

	void PushText(int x,int y,float* color,const char* szString...);
	void RenderText();

	void OnLostDevice();
	void OnResetDecive();

	void ClearTextCon();
private:
	IDirect3DDevice9* m_pDevice;
	ID3DXFont* m_pFont;
	std::vector<TextItem*> m_TextCon;

};