#pragma  once
#include "STLDefine.h"
#include "Model.h"

class SceneManager
{
public:
	typedef std::vector<Model*> ModelCon;
	bool Init();
	Model* LoadModelFromFile(const char* fn);

	static SceneManager* Instance();
	~SceneManager();
private:
	SceneManager(){};
	SceneManager(const SceneManager& sm){};
	static SceneManager* scenemng;
	void ReleaseMng();
protected:
	ModelCon m_ModelList;
};