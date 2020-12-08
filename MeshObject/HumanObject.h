#ifdef HUMANOBJECT_EXPORTS
#define HUMANOBJECT_API __declspec(dllexport)
#else
#define HUMANOBJECT_API __declspec(dllimport)
#endif

#pragma once

#define _USE_MATH_DEFINES
#include <cmath>

#define STB_IMAGE_IMPLEMENTATION   // use of stb functions once and for all
#include "stb_image.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <tuple>
#include <algorithm>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "JointList.h"
#include "BoneList.h"

#include "MeshObject.h"


#define Female 0
#define Male 1

#define MAX(x, y)	(x > y ? x : y)
#define MIN(x, y)	(x < y ? x : y)


class mjVertex;
class mjTexel;
class mjNormal;
class mjEdge;
class mjFace; // Tri face

class mjJoint;
class mjBone;
class mjSkeleton;

class mjLandmark;

class mjTexture;
class mjMaterial;

class mjBoundingBox;

class HumanObject;


class mjVertex {
public: 
	int m_Idx;

	mjPos3 *m_Coord;
	mjTexel *m_Texel;
	mjNormal *m_Normal;

	int m_Segment;
public: 
	mjVertex(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	mjVertex(const mjVertex& cpy);
	~mjVertex();
};

class mjTexel {
public: 
	int m_Idx;
	mjPos2 *m_Coord;

public: 
	mjTexel(float u = 0.0f, float v = 0.0f);
	mjTexel(const mjTexel& cpy);
	~mjTexel();
};


class mjNormal {
public:
	int m_Idx;

	mjVec3 *m_Dir;

public:
	mjNormal(float x = 0.0f, float y = 0.0f, float z = 0.0f);
	mjNormal(mjVec3 *n);
	mjNormal(const mjNormal& cpy);
	~mjNormal();
};


class mjEdge {
public:
	std::vector<mjVertex *> *m_Verts;

	mjEdge *m_Prev;
	mjEdge *m_Next;

	// ��� face�� �����ִ���
	mjFace *m_Face;

public:
	mjEdge(mjVertex *v0, mjVertex *v1);
	~mjEdge();

};


class mjFace {
public:
	//////////////////// Member Variables
	int m_Idx;

	mjMaterial *m_Material;
	std::string m_Group;

	// vertices
	std::vector<mjVertex *> *m_Verts;

	// textures
	std::vector<mjTexel *> *m_Texels;

	// face normals
	std::vector<mjNormal *> *m_Normals;

	// edges
	std::vector<mjEdge *> *m_Edges;


public:
	//////////////////// Constructor & Deconstructor
	mjFace(
		mjVertex *v0, mjVertex *v1, mjVertex *v2,
		mjTexel *t0, mjTexel *t1, mjTexel *t2,
		mjNormal *n0, mjNormal *n1, mjNormal * n2
	);
	mjFace(
		mjVertex *v0, mjVertex *v1, mjVertex *v2,
		mjTexel *t0, mjTexel *t1, mjTexel *t2,
		mjNormal *n0, mjNormal *n1, mjNormal * n2,
		mjMaterial *mtl,
		std::string group
	);
	mjFace(const mjFace &cpy);
	~mjFace();



	//////////////////// Member Variables
	mjVertex* GetVert(int idx);
	mjPos3* GetVertPos(int idx);
	int GetVertIdx(int idx);


	mjTexel* GetTex(int idx);
	mjPos2* GetTexPos(int idx);
	int GetTexIdx(int idx);

	mjNormal *GetNorm(int idx);
	mjVec3* GetNormDir(int idx);
	int GetNormIdx(int idx);
};


class mjJoint {
public:
	mjJoint(float _x = 0.0f, float _y = 0.0f, float _z = 0.0f);
	mjJoint(const mjJoint &cpy);
	~mjJoint();

public:
	int m_Idx;
	mjPos3 *m_Coord;

	HumanObject *m_Human;

	void SetHuman(HumanObject *h);
};

class mjBone {
public:
	mjBone();
	mjBone(mjJoint *upper, mjJoint *lower);
	mjBone(const mjBone &cpy);
	~mjBone();

public:
	HumanObject *m_Human;
	mjSkeleton *m_Skeleton;

	bool isLeaf;

	int m_Idx;
	int m_ChildNum;
	mjLine *m_Bone;
	float m_Length;

	mjJoint *m_UpperJoint = NULL, *m_LowerJoint = NULL;

	mjBone *m_Parent;
	std::vector<mjBone *> *m_Children = NULL;


	// Vertices effected by the bones
	// and its corresponding weights
	std::vector<mjVertex *> *m_VertList;
	std::vector<float>  *m_WeightList;

public:
	void SetParent(mjBone *p);
	void SetChild(mjBone *c);

	void SetHuman(HumanObject *h);
	void SetSkeleton(mjSkeleton *s);

	void AddVertexWeight(mjVertex *v, float w);
};


class mjSkeleton {
public:
	mjSkeleton();
	mjSkeleton(const mjSkeleton& cpy);
	~mjSkeleton();

public:
	HumanObject *m_Human;
	std::vector<mjBone *> *m_Bones;
	std::vector<mjJoint *> *m_Joints;

public:
	void SetHierarchy(int type = 0);
	void AddJoint(int idx, mjJoint *joint);
	void AddBone(int idx, mjBone *bone);
};

#define Length 0
#define Girth 1

class mjLandmark {
public:
	mjLandmark(const char* name = "", int type = 0, float lvl = 0.0, float val = 0.0f);
	mjLandmark(const mjLandmark& cpy);
	~mjLandmark();

public:
	HumanObject *m_Human;

	// tmp construction for now (20. 8. 22)
	int m_Idx;

	std::string m_Name;
	int m_Type;
	float m_Level; 
	float m_Value;

	std::vector<int> m_SegmentIdx; // Bone ������ segment
	// std::vector<int> m_VertIdx;
	std::vector<mjPos3> m_RelatedPos;


public:
	void SetName(char *lname);
	void SetLandmark(mjLandmark *l);
	void SetHuman(HumanObject *h);
	void SetSegment(int idx);

	int GetIndex();
	int GetIndex(char *lname);
	std::string GetName();
	std::string GetName(int idx);
	std::vector<int> GetSegments();
	float GetSize();

	bool HasSegment(int idx);
	float CalcSize();

	void Deform(float nval = 0.0, float upperBound = 0.0, float lowerBound = 0.0);
	void DeformLengthType(float nval = 0.0);
	void DeformGirthType(float nval = 0.0, float upperBound = 0.0, float lowerBound = 0.0);
};


class mjTexture {
public:
	mjTexture(int id, char* fname);
	~mjTexture();

	void LoadTexture();

public:
	std::string m_Filename;

    unsigned int m_Id;
    int m_Width;
    int m_Height;
    int m_Channels;

	unsigned char *m_TextureData;
};


class mjMaterial {
public:
	/*! \brief ������ �ε��� */
	int m_Idx;

	/*! \brief ������ �̸� */
	std::string m_Name;

	/*! \brief ������ �ֺ��� �ݻ� ��� */
	float m_Ambient[4];

	/*! \brief ������ ���ݻ� ��� */
	float m_Diffuse[4];

	/*! \brief ������ ���ݻ� ��� */
	float m_Specular[4];

	/*! \brief ������ ���ݻ� ���� */
	float m_Shiness;

	/*! \brief ������ ����ϴ� �ؽ�ó�� ���� ������(�ؽ�ó�� ���� ������ ���� �� �� ����) */
	mjTexture *m_Texture;

public:
	// ������ �� �Ҹ���
	mjMaterial(std::string name);
	mjMaterial(const mjMaterial &cpy);
	~mjMaterial();


	// ���� ���� �Լ�
	void SetAmbient(float r, float g, float b, float a = 1.0f);
	void SetDiffuse(float r, float g, float b, float a = 1.0f);
	void SetSpecular(float r, float g, float b, float a = 1.0f);
	void SetShiness(float s);
	void SetTexture(mjTexture *pTexture);
	void Enable();
	void Disable();
};

class mjBoundingBox {
public:
	float m_MinX, m_MaxX;
	float m_MinY, m_MaxY;
	float m_MinZ, m_MaxZ;

public:
	mjBoundingBox();
	mjBoundingBox(const mjBoundingBox &cpy);
	~mjBoundingBox();

public:
	void SetBounds(float minX = 0.0, float maxX = 100.0f, float minY = 0.0, float maxY = 100.0f, float minZ = 0.0, float maxZ = 100.0f);
};

#define RENDER_SHADE 0
#define RENDER_TEXTURE 1

class HUMANOBJECT_API HumanObject : public MeshObject {
public:
	/////// Constructor & Deconstructor
	HumanObject();
	HumanObject(const HumanObject &cpy);
	~HumanObject();


public:
	/////// Member variables
	std::string filepath;
	std::string filename;
	std::string mtlFilename;

	int m_Gender;
	int m_RenderType;
	
	float m_MinX, m_MaxX, m_MinY, m_MaxY, m_MinZ, m_MaxZ;

	mjBoundingBox *m_BoundingBox;

	std::vector<mjVertex *> *m_Vertices;
	std::vector<mjTexel *> *m_Texels;
	std::vector<mjNormal *> *m_Normals;

	std::vector<mjEdge *> *m_Edges;
	std::vector<mjFace *> *m_Faces;

	std::vector<mjTexture *> *m_Textures;
	std::vector<mjMaterial *> *m_Materials;

	// buffer for rendering
	// std::vector<int> m_IndexBuf;
	std::map<std::string, std::vector<int>> m_IndexBuf;

	// std::vector<float> m_VertBuf;
	std::map<std::string, std::vector<float>> m_VertBuf;
	std::map<std::string, std::vector<float>> m_TexBuf;
	std::map<std::string, std::vector<float>> m_NormBuf;
	// std::vector<float> m_TexBuf;
	// std::vector<float> m_NormBuf;
	

	mjSkeleton *m_Skeleton;

	std::vector<mjLandmark *> *m_Landmarks;

	// tmp for sizing functions (20. 8. 22)
	// �ش� Bone segment�� ���ϴ� vertex���� ���´�
	std::vector<mjVertex *> m_Segment[Bone_Num];

public:
	/////// Member functions
	bool LoadObj(const char* fname);
	bool LoadObjMtl(const char* fname);
	bool LoadHuman(const char* fname);
	bool LoadJoints(const char* fname);
	bool LoadLandmarks(const char* fname);


	void AddVertex(mjVertex *v);
	void AddTexel(mjTexel *t);
	void AddNormal(mjNormal *n);

	void AddEdge(mjVertex *v0, mjVertex *v1);
	void AddEdge(mjEdge *e0);

	void AddFace(mjFace *f);

	void AddTexture(mjTexture *pTexture);
	void AddMaterial(mjMaterial *pMaterial);

	void AddLandmark(mjLandmark *pLandmark);


	void UpdateJoints();
	void UpdateLandmarks();


	void UpdateIndexBuff();
	void UpdateVertBuff();
	void UpdateTexBuff();
	void UpdateNormBuff();


	bool SetSkeleton();
	bool AssignWeight();
	bool SetSegment();


	bool WriteObj(const char* fname);
	void WriteHuman(const char* fname);

	// void initHuman();

	void SetMale();
	void SetFemale();


	mjMaterial* GetMaterial(std::string fname);
	mjTexture* GetTexture(std::string fname);


	/////// Measure 
	// �����׸� ����
	int GetLandmarkNum();
	// i��° �����׸� �̸�
	void GetLandmarkName(int i, char* buffer);
	// i��° �����׸� ��
	float GetLandmarkValue(int i);
	// lname �����׸� ��
	float GetLandmarkValue(char* lname);
	// sizes ������� ġ�� ����
	void SetSizes(float *sizes);
	// i��° �����׸� ġ�� ����
	void SetSize(int i, float value);
	// lname �����׸� ġ�� ����
	void SetSize(char* lname, float value);

	// i��° �����׸� ������� ����(�� ����)
	// int GetSizePathLength(int i);
	// i��° �����׸��� n��° �� ��ǥ�� coord�� ����
	// void getSizePathPos(int l, int n, float* coord);
	// i��° �����׸� ��ǥ coord�� �� �� �ޱ�
	// void getSizePath(int l, float *coord);


	/////// Geometry
	int GetVertNum();
	int GetFaceNum();
	// i��° ������ ��ǥ ��ȯ
	void GetVert(int i, float *coord);
	// ��ü ���� ��ǥ ��ȯ
	void GetVerts(float *coord);
	// i��° face�� ��ȣ�� ��ȯ
	void GetIndex(int i, int *node);
	// ��� face ��ȣ�� ��ȯ
	void GetIndices(int *node);

	/////// Bounding / Collision
	// ������ �̸��� ��Ʈ��ȣ
	int GetSegmentNum(char* name);
	// i��° ������ ������ ��ǥ
	void GetSegmentOrigin(int i, float* coord);
	// i��° ������ ���� ��ǥ
	void GetSegmentTermination(int i, float* coord);
	// i��° ������ ���� vertex���� indices
	void GetSegmentVertIndices(int i, int* nums);
	// i��° ������ ���� vertex���� ��ǥ��
	void GetSegmentVertPos(int i, float* coord);


	/////// Pose
	void SetTPose(int s);

	/////// Visual
	void Render();
	void RenderTexture();
};

// extern HUMANOBJECT_API int mHumanObject;

// HUMANOBJECT_API int fnHumanObject(void);
