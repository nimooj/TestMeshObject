#include "pch.h"
#include "HumanObject.h"

#define Height 0
#define Bust 1
#define Waist 2
#define Hip 3
#define ArmLengthR 4
#define ArmLengthL 5
#define LegLengthR 6
#define LegLengthL 7
#define ShoulderLength 8

float dist(mjPos3 &a, mjPos3 &b) {
	return sqrt(pow(a.x - b.x , 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

float dist(mjPos3 &a, mjVec3 &b) {
	return sqrt(pow(a.x - b.x , 2) + pow(a.y - b.y, 2) + pow(a.z - b.z, 2));
}

float dist(mjVec3 &v, mjVec3 &w) {
	return sqrt(pow(v.x - w.x , 2) + pow(v.y - w.y, 2) + pow(v.z - w.z, 2));
}

// @param[in] p : 선분 밖의 한 점
// @param[in] v : 선분의 시작점
// @param[in] w : 선분의 끝점
float distToLineSegment(mjPos3 &p, mjPos3 &v, mjPos3 &w) {
	mjVec3 vw = w - v;
	float l2 = pow(vw.length(), 2);
	// v == w일 경우
	if (l2 == 0) 
		return dist(v, p);

	mjVec3 dotProduct = (p - v) * vw;
	float t = MAX(0, MIN(1, (dotProduct.x + dotProduct.y + dotProduct.z) / l2));
	mjVec3 projection = v + t * vw;
	return dist(p, projection);
}

mjPos3* get_intersection(mjPlane &pln, mjPos3 &p, mjPos3 &q) {
		mjLine l = mjLine(p, q);

		mjVec3 n_p_vec = pln.m_Normal * mjVec3(p);
		float n_p = n_p_vec.x + n_p_vec.y + n_p_vec.z;
		
		mjVec3 n_l_vec = pln.m_Normal * l.m_Dir;
		float n_l = n_l_vec.x + n_l_vec.y + n_l_vec.z;

		mjVec3 d_vec = pln.m_Normal.normalize() * q;
		float d = d_vec.x + d_vec.y + d_vec.z;

		return new mjPos3(p + mjPos3(((d - n_p) / n_l) * l.m_Dir));
}

// @param[in] pln
// @param[in] p0, p1, p2
// @param[out] intersections
bool intersect_plane_mesh(mjPlane &pln, mjPos3 &p0, mjPos3 &p1, mjPos3 &p2, std::vector<mjPos3> &intersections) {
	if (pln.IsAbove(p0) && pln.IsAbove(p1) && pln.IsAbove(p2))
		return false;
	if (pln.IsBelow(p0) && pln.IsBelow(p1) && pln.IsBelow(p2))
		return false;


	// p0 ~ p1
	if ((pln.IsAbove(p0) && pln.IsBelow(p1)) || 
		(pln.IsBelow(p0) && pln.IsAbove(p1))) {
		intersections.push_back(*get_intersection(pln, p0, p1));
	}

	// p1 ~ p2
	if ((pln.IsAbove(p1) && pln.IsBelow(p2)) || 
		(pln.IsBelow(p1) && pln.IsAbove(p2))) {
		intersections.push_back(*get_intersection(pln, p1, p2));
	}

	// p2 ~ p0
	if ((pln.IsAbove(p2) && pln.IsBelow(p0)) || 
		(pln.IsBelow(p2) && pln.IsAbove(p0))) {
		intersections.push_back(*get_intersection(pln, p0, p2));
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjVertex									// 
//////////////////////////////////////////////////////////////////////////////////

mjVertex::mjVertex(float x, float y, float z) {
	m_Coord = new mjPos3();

	m_Coord->x = x;
	m_Coord->y = y;
	m_Coord->z = z;

	m_Texel = new mjTexel();
	m_Normal = new mjNormal();
}

mjVertex::mjVertex(const mjVertex& cpy) {
	m_Idx = cpy.m_Idx;
	m_Coord = cpy.m_Coord;
	m_Texel = cpy.m_Texel;
	m_Normal = cpy.m_Normal;
}

mjVertex::~mjVertex() {

}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjTexel										// 
//////////////////////////////////////////////////////////////////////////////////

mjTexel::mjTexel(float u, float v) {
	m_Idx = 0;
	m_Coord = new mjPos2(u, v);
}

mjTexel::mjTexel(const mjTexel& cpy) {
	m_Idx = cpy.m_Idx;
	m_Coord = cpy.m_Coord;
}

mjTexel::~mjTexel() {
	delete m_Coord;
}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjNormal									// 
//////////////////////////////////////////////////////////////////////////////////
mjNormal::mjNormal(float x, float y, float z) {
	m_Dir = new mjVec3();

	m_Dir->x = x;
	m_Dir->y = y;
	m_Dir->z = z;
}

mjNormal::mjNormal(mjVec3 *n) {
	m_Dir = new mjVec3();

	m_Dir->x = n->x;
	m_Dir->y = n->y;
	m_Dir->z = n->z;
}

mjNormal::mjNormal(const mjNormal& cpy) {
	m_Idx = cpy.m_Idx;
	m_Dir = cpy.m_Dir;
}

mjNormal::~mjNormal() {

}

//////////////////////////////////////////////////////////////////////////////////
//	 									mjEdge									// 
//////////////////////////////////////////////////////////////////////////////////
mjEdge::mjEdge(mjVertex *v0, mjVertex *v1) {
	m_Verts = new std::vector<mjVertex*>();

	m_Verts->push_back(v0);
	m_Verts->push_back(v1);
}

mjEdge::~mjEdge() {

}


//////////////////////////////////////////////////////////////////////////////////
//	 									mjFace									// 
//////////////////////////////////////////////////////////////////////////////////
mjFace::mjFace(
		mjVertex *v0, mjVertex *v1, mjVertex *v2,
		mjTexel *t0, mjTexel *t1, mjTexel *t2,
		mjNormal *n0, mjNormal *n1, mjNormal * n2) {
	m_Verts = new std::vector<mjVertex*>();
	m_Texels = new std::vector<mjTexel*>();
	m_Normals = new std::vector<mjNormal *>();
	m_Edges = new std::vector<mjEdge*>();

	v0->m_Normal = n0;
	v1->m_Normal = n1;
	v2->m_Normal = n2;

	v0->m_Texel = t0;
	v1->m_Texel = t1;
	v2->m_Texel = t2;


	m_Verts->push_back(v0);
	m_Verts->push_back(v1);
	m_Verts->push_back(v2);

	m_Texels->push_back(t0);
	m_Texels->push_back(t1);
	m_Texels->push_back(t2);

	m_Normals->push_back(n0);
	m_Normals->push_back(n1);
	m_Normals->push_back(n2);

	mjEdge *e0 = new mjEdge(v0, v1);
	mjEdge *e1 = new mjEdge(v1, v2);
	mjEdge *e2 = new mjEdge(v2, v0);

	e0->m_Prev = e2;
	e0->m_Next = e1;

	e1->m_Prev = e0;
	e1->m_Next = e2;

	e2->m_Prev = e1;
	e2->m_Next = e0;

	e0->m_Face = this;
	e1->m_Face = this;
	e2->m_Face = this;

	m_Edges->push_back(e0);
	m_Edges->push_back(e1);
	m_Edges->push_back(e2);
}

mjFace::mjFace(
		mjVertex *v0, mjVertex *v1, mjVertex *v2,
		mjTexel *t0, mjTexel *t1, mjTexel *t2,
		mjNormal *n0, mjNormal *n1, mjNormal * n2,
		mjMaterial *mtl, std::string group
	) {
	m_Material = mtl;
	m_Group = group;

	m_Verts = new std::vector<mjVertex*>();
	m_Texels = new std::vector<mjTexel*>();
	m_Normals = new std::vector<mjNormal *>();
	m_Edges = new std::vector<mjEdge*>();

	v0->m_Normal = n0;
	v1->m_Normal = n1;
	v2->m_Normal = n2;

	v0->m_Texel = t0;
	v1->m_Texel = t1;
	v2->m_Texel = t2;


	m_Verts->push_back(v0);
	m_Verts->push_back(v1);
	m_Verts->push_back(v2);

	m_Texels->push_back(t0);
	m_Texels->push_back(t1);
	m_Texels->push_back(t2);

	m_Normals->push_back(n0);
	m_Normals->push_back(n1);
	m_Normals->push_back(n2);

	mjEdge *e0 = new mjEdge(v0, v1);
	mjEdge *e1 = new mjEdge(v1, v2);
	mjEdge *e2 = new mjEdge(v2, v0);

	e0->m_Prev = e2;
	e0->m_Next = e1;

	e1->m_Prev = e0;
	e1->m_Next = e2;

	e2->m_Prev = e1;
	e2->m_Next = e0;

	e0->m_Face = this;
	e1->m_Face = this;
	e2->m_Face = this;

	m_Edges->push_back(e0);
	m_Edges->push_back(e1);
	m_Edges->push_back(e2);
}

mjFace::mjFace(const mjFace &cpy) {
	m_Idx = cpy.m_Idx;
	m_Verts = cpy.m_Verts;
	m_Texels = cpy.m_Texels;
	m_Normals = cpy.m_Normals;
	m_Edges = cpy.m_Edges;
}

mjFace::~mjFace() {

}

mjVertex* mjFace::GetVert(int idx) {
	return (*m_Verts)[idx];
}

mjPos3* mjFace::GetVertPos(int idx) {
	return (*m_Verts)[idx]->m_Coord;
}

int mjFace::GetVertIdx(int idx) {
	return (*m_Verts)[idx]->m_Idx;
}

mjTexel* mjFace::GetTex(int idx) {
	return (*m_Texels)[idx];
}

mjPos2* mjFace::GetTexPos(int idx) {
	return (*m_Texels)[idx]->m_Coord;
}

int mjFace::GetTexIdx(int idx) {
	return (*m_Texels)[idx]->m_Idx;
}

mjNormal* mjFace::GetNorm(int idx) {
	return (*m_Normals)[idx];
}

mjVec3* mjFace::GetNormDir(int idx) {
	return (*m_Normals)[idx]->m_Dir;
}

int mjFace::GetNormIdx(int idx) {
	return (*m_Normals)[idx]->m_Idx;
}


//////////////////////////////////////////////////////////////////////////////////
//	 								 mjJoint									// 
//////////////////////////////////////////////////////////////////////////////////
mjJoint::mjJoint(float _x, float _y, float _z) {
	m_Coord = new mjPos3();

	m_Coord->x = _x;
	m_Coord->y = _y;
	m_Coord->z = _z;
}

mjJoint::mjJoint(const mjJoint &cpy) {
	m_Idx = cpy.m_Idx;

	m_Coord->x = cpy.m_Coord->x;
	m_Coord->y = cpy.m_Coord->y;
	m_Coord->z = cpy.m_Coord->z;
}

mjJoint::~mjJoint() {
	delete [] m_Coord;
}

void mjJoint::SetHuman(HumanObject *h) {
	m_Human = h;
}


//////////////////////////////////////////////////////////////////////////////////
//	 								  mjBone									// 
//////////////////////////////////////////////////////////////////////////////////
mjBone::mjBone() {
	m_Human = NULL;
	m_Skeleton = NULL;

	isLeaf = false;

	m_Idx = 0;

	m_ChildNum = 0;
	m_Children = new std::vector<mjBone *>();

	m_Length = 0;
	m_Bone = new mjLine();

	m_UpperJoint = new mjJoint();
	m_LowerJoint = new mjJoint();
}

mjVec3 *create_Vec3(mjPos3 *s, mjPos3 *e) {
	return new mjVec3(e->x - s->x, e->y - s->y, e->z - s->z);
}

mjBone::mjBone(mjJoint *upper, mjJoint *lower) {
	m_Human = NULL;
	m_Skeleton = NULL;

	isLeaf = false;
	m_Idx = 0;

	m_Bone = new mjLine(*create_Vec3(upper->m_Coord, lower->m_Coord), *upper->m_Coord);
	m_Length = dist(*upper->m_Coord, *lower->m_Coord);

	m_ChildNum = 0;
	m_Children = new std::vector<mjBone *>();

	m_UpperJoint = upper;
	m_LowerJoint = lower;

	m_VertList = new std::vector<mjVertex *>();
	m_WeightList = new std::vector<float>();
}

mjBone::mjBone(const mjBone& cpy) {
	m_Human = cpy.m_Human;
	m_Skeleton = cpy.m_Skeleton;

	isLeaf = cpy.isLeaf;

	m_ChildNum = cpy.m_ChildNum;

	m_UpperJoint = cpy.m_UpperJoint;
	m_LowerJoint = cpy.m_LowerJoint;

	m_Parent = cpy.m_Parent;
	m_Children = cpy.m_Children;

	m_VertList = cpy.m_VertList;
	m_WeightList = cpy.m_WeightList;
}

mjBone::~mjBone() {
	delete[] m_VertList;
	delete[] m_WeightList;
}

void mjBone::SetParent(mjBone *p) {
	m_Parent = p;
}

void mjBone::SetChild(mjBone *c) {
	c->SetParent(this);
	m_Children->push_back(c);
	m_ChildNum++;
}

void mjBone::SetHuman(HumanObject *h) {
	m_Human = h;
}

void mjBone::SetSkeleton(mjSkeleton *s) {
	m_Skeleton = s;
}

void mjBone::AddVertexWeight(mjVertex *v, float w) {
	m_VertList->push_back(v);
	m_WeightList->push_back(w);
}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjSkeleton									// 
//////////////////////////////////////////////////////////////////////////////////
mjSkeleton::mjSkeleton() {
	m_Bones = new std::vector<mjBone *>(Bone_Num);
	m_Joints = new std::vector<mjJoint *>(Joint_Num);
}

mjSkeleton::mjSkeleton(const mjSkeleton& cpy) {
	m_Bones = cpy.m_Bones;
	m_Joints = cpy.m_Joints;
}

mjSkeleton::~mjSkeleton() {

}

// @params 
// type = 
void mjSkeleton::SetHierarchy(int type) {

}

void mjSkeleton::AddJoint(int idx, mjJoint *joint) {
	joint->m_Idx = idx;
	joint->SetHuman(m_Human);

	(*m_Joints)[idx] = joint;
}

void mjSkeleton::AddBone(int idx, mjBone *bone) {
	bone->m_Idx = idx;

	bone->SetHuman(m_Human);
	bone->SetSkeleton(this);

	(*m_Bones)[idx] = bone;
}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjLandmark									// 
//////////////////////////////////////////////////////////////////////////////////
mjLandmark::mjLandmark(const char* name , int type, float lvl, float val) {
	m_Name = name;
	m_Type = type;
	m_Level = lvl;
	m_Value = val;
}

mjLandmark::mjLandmark(const mjLandmark& cpy) {
	m_Value = cpy.m_Value;
}

mjLandmark::~mjLandmark() {

}

void mjLandmark::SetName(char* lname) {
	m_Name = lname;
}

void mjLandmark::SetLandmark(mjLandmark* l) {
}

void mjLandmark::SetHuman(HumanObject *h) {
	m_Human = h;
}

void mjLandmark::SetSegment(int idx) {
	m_SegmentIdx.push_back(idx);
}

int mjLandmark::GetIndex() {
	return m_Idx;
}

int mjLandmark::GetIndex(char *lname) {
	return m_Idx;
}

std::string mjLandmark::GetName() {
	return m_Name;
}

std::string mjLandmark::GetName(int idx) {
	return m_Name;
}

std::vector<int> mjLandmark::GetSegments() {
	return m_SegmentIdx;
}

float mjLandmark::GetSize() {
	return m_Value;
}

bool mjLandmark::HasSegment(int idx) {
	for (int i : m_SegmentIdx) {
		if (i == idx)
			return true;
	}
	return false;
}

bool sortByPosAngle(std::tuple<float, mjPos3> a, std::tuple<float, mjPos3> b) {
	return std::get<0>(a) < std::get<0>(b);
}

float mjLandmark::CalcSize() {
	mjPos3 centerPos = mjPos3(0, 0, 0);
	std::vector<mjPos3> circPos;
	float distance = 0;

	if (m_Type == Girth) {
		// plane과 mesh의 교차점들의 길이
		mjPlane pln = mjPlane(mjPos3(0, m_Level, 0), mjPos3(10, m_Level, 0), mjPos3(0, m_Level, -10));

		for (mjFace *f : (*m_Human->m_Faces)) {
			mjPos3 p0 = (*f->GetVertPos(0));
			mjPos3 p1 = (*f->GetVertPos(1));
			mjPos3 p2 = (*f->GetVertPos(2));
			// mjPlane pln = mjPlane(p0, p1, p2);

			std::vector<mjPos3> intersections;
			if (intersect_plane_mesh(pln, p0, p1, p2, intersections)) {
				circPos.insert(circPos.end(), intersections.begin(), intersections.end());
			}
		}


		// centerPos 기준으로 centerPos ~ p, centerPos의 x축 방향이랑 이루는 각도를 오름차순 정렬
		printf("\nintersections size: %d\n", circPos.size());
		for (mjPos3 p : circPos) {
			printf("%f %f %f\n", p.x, p.y, p.z);

			centerPos.x += p.x;
			centerPos.y += p.y;
			centerPos.z += p.z;
		}
		centerPos.x /= circPos.size();
		centerPos.y /= circPos.size();
		centerPos.z /= circPos.size();

		mjPos3 centerPos_x = centerPos;
		centerPos_x.x += 1.0;
		mjVec3 m = centerPos_x - centerPos;

		std::vector<std::tuple<float, mjPos3>> alignedCirc;
		for (mjPos3 p : circPos) {
			mjVec3 l = p - centerPos;

			mjVec3 dotProduct = l * m;
			float dotSum = dotProduct.x + dotProduct.y + dotProduct.z;

			float angle = (dotSum / (l.length() * m.length())) * 180 / M_PI;

			alignedCirc.push_back(std::make_tuple(angle, p));
		}

		// 오름차순 정렬
		std::sort(alignedCirc.begin(), alignedCirc.end(), sortByPosAngle);

		// distance로 거리 구하기
		float girth = 0;
		printf("\n=========================================================\nAligned circ : ");
		m_RelatedPos.push_back(std::get<1>(alignedCirc[0]));
		for (int i = 1; i < alignedCirc.size(); i++) {
			float angle = std::get<0>(alignedCirc[i]);
			mjPos3 p = std::get<1>(alignedCirc[i]);
			// printf("%f : %f %f %f\n", angle, p.x, p.y, p.z);
			girth += dist(std::get<1>(alignedCirc[i]), std::get<1>(alignedCirc[i - 1]));
			m_RelatedPos.push_back(std::get<1>(alignedCirc[i]));
		}
		girth += dist(std::get<1>(alignedCirc[0]), std::get<1>(alignedCirc[alignedCirc.size() - 1]));
		printf("New circ: %f\n", girth);
		printf("=========================================================\n");


	}
	else if (m_Type == Length) {
		// ToDo::손끝은 lowerJoint가 없어서 포함안될듯 ?
		// segment의 upper joint ~ lower joint 거리
		for (int boneIdx : m_SegmentIdx) {
			mjBone *thisBone = (*m_Human->m_Skeleton->m_Bones)[boneIdx];
			distance += dist(*thisBone->m_UpperJoint->m_Coord, *thisBone->m_LowerJoint->m_Coord);
		}
		m_Value = distance;
	}

	return m_Value;
}

void mjLandmark::Deform(float nval, float upperBound, float lowerBound) {
	switch (m_Type) {
	case Length:
		DeformLengthType(nval);
		break;
	case Girth:
		DeformGirthType(nval, upperBound, lowerBound);
		break;
	default:
		DeformGirthType(nval, upperBound, lowerBound);
		break;
	}

	m_Value = CalcSize();
}

void mjLandmark::DeformLengthType(float nval) {	
	std::cout << "Length type deformation... " << std::endl;
	float scale = nval / m_Value;
	std::cout << "Curr value : " << m_Value << ", Modifying to " << nval << std::endl;
	std::cout << "scale is " << scale << std::endl;

	for (int boneIdx : m_SegmentIdx) {
		mjBone *thisBone = (*m_Human->m_Skeleton->m_Bones)[boneIdx];
		
		mjJoint *upperJoint = thisBone->m_UpperJoint;
		mjJoint *lowerJoint = thisBone->m_LowerJoint;

		std::cout << upperJoint->m_Idx << " " << lowerJoint->m_Idx << std::endl;

		mjVec3 deformVec; 
		deformVec.m_Pos->x = scale * (lowerJoint->m_Coord->x - upperJoint->m_Coord->x);
		deformVec.m_Pos->y = scale * (lowerJoint->m_Coord->y - upperJoint->m_Coord->y);
		
		for (mjVertex* v : *thisBone->m_VertList) {
			// upperJoint를 원점으로 이동

			// Scale
			v->m_Coord->x *= deformVec.m_Pos->x;
			v->m_Coord->y *= deformVec.m_Pos->y;
		}
		/*
		lowerJoint->m_Coord->x = deformVec.m_Pos->x;
		lowerJoint->m_Coord->y = deformVec.m_Pos->y;
		*/
	}

}

void mjLandmark::DeformGirthType(float nval, float upperBound, float lowerBound) {
	std::cout << "Girth type deformation... ";
	float scale = nval / m_Value;

	// upperBound와 lowerBound 사이에서 
	// m_Level을 원점으로 Quadratic하게 변형시킨다 -> 갖고 있는 Human의 vertices를 불러와서 변형
	for (mjVertex *vert : *m_Human->m_Vertices) {
		// vert가 변형 segment에 포함될 경우에만 deform
		if (HasSegment(vert->m_Segment)) {
			mjPos3 pos = *vert->m_Coord;
			if (pos.y < upperBound && pos.y >= m_Level) {
				float n = upperBound - pos.y;
				float m = pos.y - m_Level;

				mjPos3 scaledCoord = scale * pos;

				mjPos3 result = (m * pos + n * scaledCoord) / (n + m);

				vert->m_Coord->x = result.x;
				vert->m_Coord->z = result.z;
			}
			else if (pos.y < m_Level && pos.y >= lowerBound) {
				float n = m_Level - pos.y;
				float m = pos.y - lowerBound;

				mjPos3 scaledCoord = scale * pos;

				mjPos3 result = (m * scaledCoord + n * pos) / (n + m);

				vert->m_Coord->x = result.x;
				vert->m_Coord->z = result.z;
			}
		}
	}

}


//////////////////////////////////////////////////////////////////////////////////
//	 								mjTexture									// 
//////////////////////////////////////////////////////////////////////////////////

mjTexture::mjTexture(int id, char* fname) {
	m_Id = id;
	m_Filename = fname;
}

mjTexture::~mjTexture() {

}

void mjTexture::LoadTexture() {
	printf("Importing %s...\n", m_Filename);

	// glGenTextures(1, &m_Id);
	// glBindTexture(GL_TEXTURE_2D, m_Id);

	/*
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	*/

	stbi_set_flip_vertically_on_load(true);
	m_TextureData = stbi_load(m_Filename.c_str(), &m_Width, &m_Height, &m_Channels, 0);

	if (m_TextureData)
	{   
		if (m_Channels == 3) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_Width, m_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_TextureData);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_TextureData);
		}

		// glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		printf("Loading %s failed...\n", m_Filename);
	}


	stbi_image_free(m_TextureData);
}

//////////////////////////////////////////////////////////////////////////////////
//	 								mjMaterial									// 
//////////////////////////////////////////////////////////////////////////////////

mjMaterial::mjMaterial(std::string name) {
	m_Idx = -1;
	m_Name = name;
	SetAmbient(0.2f, 0.2f, 0.2f, 1.0f);
	SetDiffuse(0.7f, 0.7f, 0.7f, 1.0f);
	SetSpecular(0.7f, 0.7f, 0.7f, 1.0f);
	m_Shiness = 32.0f;
}

mjMaterial::mjMaterial(const mjMaterial &cpy) {
	m_Idx = -1;
	m_Name = cpy.m_Name;

	memmove(m_Ambient, cpy.m_Ambient, sizeof(cpy.m_Ambient[0]) * 4);
	memmove(m_Diffuse, cpy.m_Diffuse, sizeof(cpy.m_Diffuse[0]) * 4);
	memmove(m_Specular, cpy.m_Specular, sizeof(cpy.m_Specular[0]) * 4);

	m_Shiness = cpy.m_Shiness;
	m_Texture = cpy.m_Texture;
}

mjMaterial::~mjMaterial() {

}


void mjMaterial::SetAmbient(float r, float g, float b, float a) {
	m_Ambient[0] = r;
	m_Ambient[1] = g;
	m_Ambient[2] = b;
	m_Ambient[3] = a;
}

void mjMaterial::SetDiffuse(float r, float g, float b, float a) {
	m_Diffuse[0] = r;
	m_Diffuse[1] = g;
	m_Diffuse[2] = b;
	m_Diffuse[3] = a;
}

void mjMaterial::SetSpecular(float r, float g, float b, float a) {
	m_Specular[0] = r;
	m_Specular[1] = g;
	m_Specular[2] = b;
	m_Specular[3] = a;
}

void mjMaterial::SetShiness(float s) {
	m_Shiness = s;
}

void mjMaterial::SetTexture(mjTexture *pTexture) {
	m_Texture = pTexture;
}

void mjMaterial::Enable() {
	// 알파 값이 1이 아니라면 블렌딩을 활성화하고,
	if (m_Diffuse[3] != 1.0f)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else // 아니면 블렌딩을 비활성화 한다.
		glDisable(GL_BLEND);

	// 삼각형의 앞면과 뒷면에 동일한 재질을 적용한다.
	/*
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, m_Ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, m_Diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, m_Specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, m_Shiness);
	*/
}

void mjMaterial::Disable() {
	glDisable(GL_BLEND);
}


//////////////////////////////////////////////////////////////////////////////////
//	 							mjBoudningBox									// 
//////////////////////////////////////////////////////////////////////////////////
mjBoundingBox::mjBoundingBox() {
	m_MinX = m_MaxX = m_MinY = m_MaxY = m_MinZ = m_MaxZ = 0;
}

mjBoundingBox::mjBoundingBox(const mjBoundingBox &cpy) {
	m_MinX = cpy.m_MinX;
	m_MinY = cpy.m_MinY;
	m_MinZ = cpy.m_MinZ;

	m_MaxX = cpy.m_MaxX;
	m_MaxY = cpy.m_MaxY;
	m_MaxZ = cpy.m_MaxZ;
}

mjBoundingBox::~mjBoundingBox() {

}

void mjBoundingBox::SetBounds(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) {
	m_MinX = minX;
	m_MaxX = maxX;

	m_MinY = minY;
	m_MaxY = maxY;

	m_MinZ = minZ;
	m_MaxZ = maxZ;
}


//////////////////////////////////////////////////////////////////////////////////
//	 								HumanObject									// 
//////////////////////////////////////////////////////////////////////////////////

HumanObject::HumanObject() {
	m_Gender = Female;
	m_RenderType = RENDER_SHADE;

	m_BoundingBox = new mjBoundingBox();

	m_Vertices = new std::vector<mjVertex*>();
	m_Texels = new std::vector<mjTexel*>();
	m_Normals = new std::vector<mjNormal*>();

	m_Edges = new std::vector<mjEdge*>();
	m_Faces = new std::vector<mjFace*>();

	m_Textures = new std::vector<mjTexture*>();
	m_Materials = new std::vector<mjMaterial*>();

	m_Landmarks = new std::vector<mjLandmark*>();

	m_VertBuf = std::map<std::string, std::vector<float>>();
	// m_VertBuf = std::vector<float>();
	// m_IndexBuf = std::vector<int>();
	m_IndexBuf = std::map<std::string, std::vector<int>>();

	m_Skeleton = new mjSkeleton();
	m_Skeleton->m_Human = this;
}

HumanObject::HumanObject(const HumanObject &cpy) {
	m_Skeleton = cpy.m_Skeleton;
}

HumanObject::~HumanObject() {
	delete[] m_Vertices;
	delete[] m_Texels;
	delete[] m_Normals;

	delete[] m_Edges;
	delete[] m_Faces;
}


mjVertex *create_Vertex(float x, float y, float z) {
	return new mjVertex(x, y, z);
}

mjTexel *create_Texel(float u, float v) {
	return new mjTexel(u, v);
}

mjNormal *create_Normal(float x, float y, float z) {
	return new mjNormal(x, y, z);
}

mjJoint *create_Joint(float x, float y, float z) {
	return new mjJoint(x, y, z);
}

mjLandmark* create_Landmark(const char* name, int type, float lvl, float val) {
	return new mjLandmark(name, type, lvl, val);
}

mjFace *create_Face(mjVertex *v0, mjVertex *v1, mjVertex *v2, mjTexel *t0, mjTexel *t1, mjTexel *t2, mjNormal *n0, mjNormal *n1, mjNormal *n2, mjMaterial *material, std::string groupName) {
	return new mjFace(v0, v1, v2, t0, t1, t2, n0, n1, n2, material, groupName);
}

bool HumanObject::LoadObj(const char* fname) {
	FILE *fp;
	fopen_s(&fp, fname, "r");

	if (!fp) {
		printf("Loading %s failed...\n", fname);
		return false;
	}
	else {
		printf("Importing %s...\n", fname);
	}

	std::string fname_s = (std::string) fname;
	size_t pos = fname_s.find_last_of("\\/");
	std::string filePath_s = (std::string::npos == pos) ? "" : fname_s.substr(0, pos);
	filePath_s += "/";

	this->filepath = filePath_s.c_str();
	this->filename = fname_s.c_str();

	char tag[256];
	bool isTexture = false;
	bool isNormal = false;

	std::string GroupName;
	mjMaterial *pCurrMtl = NULL;

	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	while (fscanf_s(fp, "%s", tag, 256) != EOF) {
		// mtlib-tag
		if (!strcmp(tag, "mtllib")) {
			// 재질 파일명(주로 상대 경로)을 구하여
			char mtl_fname[128];
			fscanf_s(fp, "%s", mtl_fname, 128);
			std::string absoluteFP_s(filepath);
			char* absoluteFP = (char*) absoluteFP_s.c_str();

			mtlFilename = mtl_fname;

			LoadObjMtl(strcat(absoluteFP, mtl_fname));
		}

		// g-tag
		else if (!strcmp(tag, "g")) {
			char grp_name[256];
			fscanf_s(fp, "%s", grp_name, 256);
			GroupName = std::string(grp_name);
		}

		// usemtl-tag 
		else if (!strcmp(tag, "usemtl")) {
			char mtl_name[256];
			fscanf_s(fp, "%s", mtl_name, 256);
			pCurrMtl = GetMaterial(mtl_name);
		}


		// v-tag
		else if (!strcmp(tag, "v")) {
			float x, y, z;
			fscanf_s(fp, "%f%f%f", &x, &y, &z);
			AddVertex(create_Vertex(x, y, z));

			if (x < minX)
				minX = x;
			if (x > maxX)
				maxX = x;

			if (y < minY)
				minY = y;
			if (y > maxY)
				maxY = y;

			if (z < minZ)
				minZ = z;
			if (z > maxZ)
				maxZ = z;
		}

		// vt-tag
		else if (!strcmp(tag, "vt")) {
			float u, v;
			isTexture = true;
			fscanf_s(fp, "%f%f", &u, &v);
			AddTexel(create_Texel(u, v));
		}

		// vn-tag
		else if (!strcmp(tag, "vn")) {
			float x, y, z;
			isNormal = true;
			fscanf_s(fp, "%f%f%f", &x, &y, &z);
			AddNormal(create_Normal(x, y, z));
		}

		// f-tag
		else if (!strcmp(tag, "f")) {
			std::vector<int> vIndices, tIndices, nIndices;

			// Get a whole line
			char line[256];
			fgets(line, 256, fp);
			char *data = line;

			// Reading line...
			while (true) {
				// Move past spaces and newlines
				while(*data == ' ' || *data == '\n')
					data++;

				// When a line ends, break
				if (!strcmp(data, ""))
					break;


				// Read index info to a buffer
				char buffer[256];
				sscanf_s(data, "%s", buffer, 256);
				data += strlen(buffer);


				char separator;
				int vidx, tidx, nidx;

				// when both texture and normal
				if (isTexture && isNormal) {
					sscanf_s(buffer, "%d%c%d%c%d", &vidx, &separator, 1, &tidx, &separator, 1, &nidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					tidx = (tidx < 0) ? -tidx : tidx;
					nidx = (nidx < 0) ? -nidx : nidx;

					vIndices.push_back(vidx - 1);
					tIndices.push_back(tidx - 1);
					nIndices.push_back(nidx - 1);
					continue;
				}
				// when only texture
				else if (isTexture && !isNormal) {
					sscanf_s(buffer, "%d%c%d", &vidx, &separator, 1, &tidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					tidx = (tidx < 0) ? -tidx : tidx;
					vIndices.push_back(vidx - 1);
					tIndices.push_back(tidx - 1);
					continue;
				}
				// when only normal 
				else if (!isTexture && isNormal) {
					sscanf_s(buffer, "%d%c%d", &vidx, &separator, 1, &nidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					nidx = (nidx < 0) ? -nidx : nidx;
					vIndices.push_back(vidx - 1);
					nIndices.push_back(nidx - 1);
					continue;
				}
				// when only vIndex
				else if (!isTexture && !isNormal) {
					sscanf_s(buffer, "%d", &vidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					vIndices.push_back(vidx - 1);
					continue;
				}
					
			}
			// End of reading line...

			if (vIndices.size() == 3) {
				/////// Add Face Info to HumanObject
				mjVertex *v0 = (*m_Vertices)[vIndices[0]],
					*v1 = (*m_Vertices)[vIndices[1]],
					*v2 = (*m_Vertices)[vIndices[2]];

				mjTexel *t0 = 0, *t1 = 0, *t2 = 0; 
				if (isTexture) {
					t0 = (*m_Texels)[tIndices[0]];
					t1 = (*m_Texels)[tIndices[1]];
					t2 = (*m_Texels)[tIndices[2]];
				}

				mjNormal *n0 = 0, *n1 = 0, *n2 = 0;
				if (isNormal) {
					n0 = (*m_Normals)[nIndices[0]];
					n1 = (*m_Normals)[nIndices[1]];
					n2 = (*m_Normals)[nIndices[2]];
				}
				else {
					// Generate normal if not provided
					// Assign triangle's normal
					mjVec3 *n = new mjVec3((v1->m_Coord - v0->m_Coord) ^ (v2->m_Coord - v0->m_Coord));

					//normalize
					n->x /= n->length();
					n->y /= n->length();
					n->z /= n->length();

					mjNormal *norm = new mjNormal(n);

					n0 = n1 = n2 = norm;
				}

				v0->m_Texel = t0;
				v0->m_Normal = n0;

				v1->m_Texel = t1;
				v1->m_Normal = n1;

				v2->m_Texel = t2;
				v2->m_Normal = n2;

				AddFace(create_Face(v0, v1, v2, t0, t1, t2, n0, n1, n2, pCurrMtl, GroupName));
			}
			else if (vIndices.size() == 4) {
				/////// Add Face Info to HumanObject
				mjVertex *v0 = (*m_Vertices)[vIndices[0]],
					*v1 = (*m_Vertices)[vIndices[1]],
					*v2 = (*m_Vertices)[vIndices[2]],
					*v3 = (*m_Vertices)[vIndices[3]];

				mjTexel *t0 = 0, *t1 = 0, *t2 = 0, *t3 = 0;
				if (isTexture) {
					t0 = (*m_Texels)[tIndices[0]];
					t1 = (*m_Texels)[tIndices[1]];
					t2 = (*m_Texels)[tIndices[2]];
					t3 = (*m_Texels)[tIndices[3]];
				}

				mjNormal *n0 = 0, *n1 = 0, *n2 = 0, *n3 = 0;
				if (isNormal) {
					n0 = (*m_Normals)[nIndices[0]];
					n1 = (*m_Normals)[nIndices[1]];
					n2 = (*m_Normals)[nIndices[2]];
					n3 = (*m_Normals)[nIndices[3]];
				}
				else {
					// Generate normal if not provided
					// Assign triangle's normal
					mjVec3 *n = new mjVec3((v1->m_Coord - v0->m_Coord) ^ (v2->m_Coord - v0->m_Coord));

					//normalize
					n->x /= n->length();
					n->y /= n->length();
					n->z /= n->length();

					mjNormal *norm = new mjNormal(n);

					n0 = n1 = n2 = n3 = norm;
				}

				v0->m_Texel = t0;
				v0->m_Normal = n0;

				v1->m_Texel = t1;
				v1->m_Normal = n1;

				v2->m_Texel = t2;
				v2->m_Normal = n2;

				v3->m_Texel = t3;
				v3->m_Normal = n3;

				AddFace(create_Face(v0, v1, v2, t0, t1, t2, n0, n1, n2, pCurrMtl, GroupName));
				AddFace(create_Face(v0, v2, v3, t0, t2, t3, n0, n2, n3, pCurrMtl, GroupName));
			}


			/////// Add Edge
			/*
			mjEdge *e0 = new mjEdge(v0, v1), 
				*e1 = new mjEdge(v1, v2), 
				*e2 = new mjEdge(v2, v0);

			AddEdge(e0);
			AddEdge(e1);
			AddEdge(e2);
			*/
		}
	}

	if (isTexture)
		m_RenderType = RENDER_TEXTURE;
	else
		m_RenderType = RENDER_SHADE;

	m_BoundingBox->SetBounds(minX, maxX, minY, maxY, minZ, maxZ);

	m_MinX = minX;
	m_MaxX = maxX;
	m_MinY = minY;
	m_MaxY = maxY;
	m_MinZ = minZ;
	m_MaxZ = maxZ;

	UpdateVertBuff();
	UpdateIndexBuff();
}

mjTexture* create_texture(int id, char* fname)
{
	return new mjTexture(id, fname);
}

mjBone *create_Bone(mjJoint *upper, mjJoint *lower) {
	return new mjBone(upper, lower);
}

bool HumanObject::LoadObjMtl(const char* fname) {
	// 생성할 재질 변수를 정의한다.
	mjMaterial *pMtl = NULL;

	// 파일을 열고, 
	FILE *fp;
	fopen_s(&fp, fname, "r");
	if (!fp) // 실패하면 false를 반환한다
	{
		printf("Importing %s failed...\n", fname);
		return false;
	}
	else
		printf("Importing %s...\n", fname);

	// 파일의 끝까지 한 단어씩 읽어, tag 배열에 저장한다.
	char tag[256];
	while (fscanf_s(fp, "%s", tag, 256) != EOF)
	{
		// newmtl (new material) 태그라면,
		if (!strcmp(tag, "newmtl"))
		{
			// 재질의 이름을 읽고,
			char tmp[256];
			fscanf_s(fp, "%s", tmp, 256);

			// 재질을 생성하여, 메쉬의 재질 리스트에 추가한다.
			pMtl = new mjMaterial(tmp);
			AddMaterial(pMtl);
		}

		// Ka (ambient coefficients) 태그라면,
		if (!strcmp(tag, "Ka"))
		{
			// ambient 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetAmbient(c[0], c[1], c[2]);
		}

		// Kd (diffuse coefficients) 태그라면,
		if (!strcmp(tag, "Kd"))
		{
			// diffuse 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetDiffuse(c[0], c[1], c[2]);
		}

		// Ks (specular coefficients) 태그라면,
		if (!strcmp(tag, "Ks"))
		{
			// specular 성분을 읽어서 재질을 설정한다.
			GLfloat c[3];
			fscanf_s(fp, "%f%f%f", &c[0], &c[1], &c[2]);
			pMtl->SetSpecular(c[0], c[1], c[2]);
		}

		// map_Kd (diffuse texture file) 태그라면,
		if (!strcmp(tag, "map_Kd"))
		{
			// 텍스처 파일을 읽는다.
			char tex_name[512];
			fscanf_s(fp, "%s", tex_name, 512);

			std::string texFP_s(filepath);
			char* texFP = (char*)texFP_s.c_str();

			strcat(texFP, tex_name);

			// 이미 로드된 텍스처라면 기존의 텍스처를 설정한다.
			pMtl->m_Texture = GetTexture(texFP);


			// 새로운 텍스처라면
			if (pMtl->m_Texture == NULL)
			{
				// 새로운 텍스처를 생성하여 메쉬에 추가하고, 재질에 설정한다.
				mjTexture *pTexture = create_texture(0, texFP);
				if (pTexture != NULL)
				{
					// pTexture->LoadTexture();
					AddTexture(pTexture);
					pMtl->SetTexture(pTexture);
				}
			}
		}
	}

	// 파일을 닫는다.
	fclose(fp);
	return true;
}

bool HumanObject::LoadJoints(const char* fname) {
	if (m_Skeleton == NULL)
		m_Skeleton = new mjSkeleton();

	FILE *fp;
	fopen_s(&fp, fname, "r");

	if (!fp) {
		printf("Loading %s failed...\n", fname);
		return false;
	}
	else {
		printf("Importing %s...\n", fname);
	} 
	int idx = 0;
	float x, y, z;
	while (fscanf_s(fp, "%f%f%f", &x, &y, &z) != EOF) {
		m_Skeleton->AddJoint(idx, create_Joint(x, y, z));
		idx++;
	}


	// Set-up skeleton and bones
	if (!SetSkeleton())
		return false;

	// Set body segment with bones
	if (!SetSegment())
		return false;

	// Assign weights to bones
	if (!AssignWeight())
		return false;

	return true;
}

bool HumanObject::LoadLandmarks(const char* fname) {
	if (m_Landmarks == NULL)
		m_Landmarks = new std::vector<mjLandmark *>();

	FILE *fp;
	fopen_s(&fp, fname, "r");

	if (!fp) {
		printf("Loading %s failed...\n", fname);
		return false;
	}
	else {
		printf("Importing %s...\n", fname);
	}

	char tag[256], type[128];
	float level, value;
	while (fscanf_s(fp, "%s%s%f%f", tag, 256, type, 128, &level, &value) != EOF) {
		if (!strcmp(type, "Length")) {
			AddLandmark(create_Landmark(tag, Length, level, value));
		}
		else if (!strcmp(type, "Girth")) {
			AddLandmark(create_Landmark(tag, Girth, level, value));
		}
		else {
			std::cout << "Non-existent Landmark type " << type << std::endl;
		}
	}

	return true;
}

bool HumanObject::LoadHuman(const char* fname) {
	FILE *fp;
	fopen_s(&fp, fname, "r");

	if (!fp) {
		printf("Loading %s failed...\n", fname);
		return false;
	}
	else {
		printf("Importing %s...\n", fname);
	}

	if (m_Skeleton == NULL) {
		m_Skeleton = new mjSkeleton();
	}
	int jointIdx = 0;

	if (m_Landmarks == NULL) {
		m_Landmarks = new std::vector<mjLandmark *>();
	}

	std::string fname_s = (std::string) fname;
	size_t pos = fname_s.find_last_of("\\/");
	std::string filePath_s = (std::string::npos == pos) ? "" : fname_s.substr(0, pos);
	filePath_s += "/";

	this->filepath = filePath_s.c_str();
	this->filename = fname_s.c_str();

	bool isTexture = false;
	bool isNormal = false;

	std::string GroupName;
	mjMaterial *pCurrMtl = NULL;

	float minX = INFINITY, minY = INFINITY, minZ = INFINITY;
	float maxX = -INFINITY, maxY = -INFINITY, maxZ = -INFINITY;

	// mjPos3 handR = mjPos3(), handL = mjPos3();

	// mtl 파일이 있다면 Human 파일과 같은 디렉토리에 있어야한다
	char tag[256];
	while (fscanf_s(fp, "%s", tag, 256) != EOF) {
		// mtlib-tag
		if (!strcmp(tag, "mtllib")) {
			// 재질 파일명(주로 상대 경로)을 구하여
			char mtl_fname[128];
			fscanf_s(fp, "%s", mtl_fname, 128);
			std::string absoluteFP_s(filepath);
			char* absoluteFP = (char*)absoluteFP_s.c_str();

			mtlFilename = mtl_fname;

			LoadObjMtl(strcat(absoluteFP, mtl_fname));
		}

		// g-tag
		else if (!strcmp(tag, "g")) {
			char grp_name[256];
			fscanf_s(fp, "%s", grp_name, 256);
			GroupName = std::string(grp_name);
		}

		// usemtl-tag 
		else if (!strcmp(tag, "usemtl")) {
			char mtl_name[256];
			fscanf_s(fp, "%s", mtl_name, 256);
			pCurrMtl = GetMaterial(mtl_name);
		}

		// v-tag
		else if (!strcmp(tag, "v")) {
			float x, y, z;
			fscanf_s(fp, "%f%f%f", &x, &y, &z);
			AddVertex(create_Vertex(x, y, z));

			if (x < minX) {
				minX = x;
				/*
				handR.x = x;
				handR.y = y;
				handR.z = z;
				*/
			}
			if (x > maxX) {
				maxX = x;
				/*
				handL.x = x;
				handL.y = y;
				handL.z = z;
				*/
			}

			if (y < minY)
				minY = y;
			if (y > maxY)
				maxY = y;

			if (z < minZ)
				minZ = z;
			if (z > maxZ)
				maxZ = z;
		}

		// vt-tag
		else if (!strcmp(tag, "vt")) {
			float u, v;
			isTexture = true;
			fscanf_s(fp, "%f%f", &u, &v);
			AddTexel(create_Texel(u, v));
		}

		// vn-tag
		else if (!strcmp(tag, "vn")) {
			float x, y, z;
			isNormal = true;
			fscanf_s(fp, "%f%f%f", &x, &y, &z);
			AddNormal(create_Normal(x, y, z));
		}

		// f-tag
		else if (!strcmp(tag, "f")) {
			std::vector<int> vIndices, tIndices, nIndices;

			// Get a whole line
			char line[256];
			fgets(line, 256, fp);
			char *data = line;

			// Reading line...
			while (true) {
				// Move past spaces and newlines
				while (*data == ' ' || *data == '\n')
					data++;

				// When a line ends, break
				if (!strcmp(data, ""))
					break;


				// Read index info to a buffer
				char buffer[256];
				sscanf_s(data, "%s", buffer, 256);
				data += strlen(buffer);


				char separator;
				int vidx, tidx, nidx;

				// when both texture and normal
				if (isTexture && isNormal) {
					sscanf_s(buffer, "%d%c%d%c%d", &vidx, &separator, 1, &tidx, &separator, 1, &nidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					tidx = (tidx < 0) ? -tidx : tidx;
					nidx = (nidx < 0) ? -nidx : nidx;

					vIndices.push_back(vidx - 1);
					tIndices.push_back(tidx - 1);
					nIndices.push_back(nidx - 1);
					continue;
				}
				// when only texture
				else if (isTexture && !isNormal) {
					sscanf_s(buffer, "%d%c%d", &vidx, &separator, 1, &tidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					tidx = (tidx < 0) ? -tidx : tidx;
					vIndices.push_back(vidx - 1);
					tIndices.push_back(tidx - 1);
					continue;
				}
				// when only normal 
				else if (!isTexture && isNormal) {
					sscanf_s(buffer, "%d%c%d", &vidx, &separator, 1, &nidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					nidx = (nidx < 0) ? -nidx : nidx;
					vIndices.push_back(vidx - 1);
					nIndices.push_back(nidx - 1);
					continue;
				}
				// when only vIndex
				else if (!isTexture && !isNormal) {
					sscanf_s(buffer, "%d", &vidx);
					vidx = (vidx < 0) ? -vidx : vidx;
					vIndices.push_back(vidx - 1);
					continue;
				}

			}
			// End of reading line...

			if (vIndices.size() == 3) {
				/////// Add Face Info to HumanObject
				mjVertex *v0 = (*m_Vertices)[vIndices[0]],
					*v1 = (*m_Vertices)[vIndices[1]],
					*v2 = (*m_Vertices)[vIndices[2]];

				mjTexel *t0 = 0, *t1 = 0, *t2 = 0;
				if (isTexture) {
					t0 = (*m_Texels)[tIndices[0]];
					t1 = (*m_Texels)[tIndices[1]];
					t2 = (*m_Texels)[tIndices[2]];
				}

				mjNormal *n0 = 0, *n1 = 0, *n2 = 0;
				if (isNormal) {
					n0 = (*m_Normals)[nIndices[0]];
					n1 = (*m_Normals)[nIndices[1]];
					n2 = (*m_Normals)[nIndices[2]];
				}
				else {
					// Generate normal if not provided
					// Assign triangle's normal
					mjVec3 *n = new mjVec3((v1->m_Coord - v0->m_Coord) ^ (v2->m_Coord - v0->m_Coord));

					//normalize
					n->x /= n->length();
					n->y /= n->length();
					n->z /= n->length();

					mjNormal *norm = new mjNormal(n);

					n0 = n1 = n2 = norm;
				}

				v0->m_Texel = t0;
				v0->m_Normal = n0;

				v1->m_Texel = t1;
				v1->m_Normal = n1;

				v2->m_Texel = t2;
				v2->m_Normal = n2;

				AddFace(create_Face(v0, v1, v2, t0, t1, t2, n0, n1, n2, pCurrMtl, GroupName));
			}
			else if (vIndices.size() == 4) {
				/////// Add Face Info to HumanObject
				mjVertex *v0 = (*m_Vertices)[vIndices[0]],
					*v1 = (*m_Vertices)[vIndices[1]],
					*v2 = (*m_Vertices)[vIndices[2]],
					*v3 = (*m_Vertices)[vIndices[3]];

				mjTexel *t0 = 0, *t1 = 0, *t2 = 0, *t3 = 0;
				if (isTexture) {
					t0 = (*m_Texels)[tIndices[0]];
					t1 = (*m_Texels)[tIndices[1]];
					t2 = (*m_Texels)[tIndices[2]];
					t3 = (*m_Texels)[tIndices[3]];
				}

				mjNormal *n0 = 0, *n1 = 0, *n2 = 0, *n3 = 0;
				if (isNormal) {
					n0 = (*m_Normals)[nIndices[0]];
					n1 = (*m_Normals)[nIndices[1]];
					n2 = (*m_Normals)[nIndices[2]];
					n3 = (*m_Normals)[nIndices[3]];
				}
				else {
					// Generate normal if not provided
					// Assign triangle's normal
					mjVec3 *n = new mjVec3((v1->m_Coord - v0->m_Coord) ^ (v2->m_Coord - v0->m_Coord));

					//normalize
					n->x /= n->length();
					n->y /= n->length();
					n->z /= n->length();

					mjNormal *norm = new mjNormal(n);

					n0 = n1 = n2 = n3 = norm;
				}

				v0->m_Texel = t0;
				v0->m_Normal = n0;

				v1->m_Texel = t1;
				v1->m_Normal = n1;

				v2->m_Texel = t2;
				v2->m_Normal = n2;

				v3->m_Texel = t3;
				v3->m_Normal = n3;

				AddFace(create_Face(v0, v1, v2, t0, t1, t2, n0, n1, n2, pCurrMtl, GroupName));
				AddFace(create_Face(v0, v2, v3, t0, t2, t3, n0, n2, n3, pCurrMtl, GroupName));
			}
		}
	
		// joint-tag
		else if (!strcmp(tag, "jo")) {
			float x, y, z;
			fscanf_s(fp, "%f%f%f", &x, &y, &z);
			m_Skeleton->AddJoint(jointIdx, create_Joint(x, y, z));
			jointIdx++;
		}

		// landmark-tag
		else if (!strcmp(tag, "la")) {
			char ltag[256], ltype[128];
			float level, value;

			fscanf_s(fp, "%s%s%f%f", ltag, 128, ltype, 128, &level, &value, 512);
			if (!strcmp(ltype, "Length")) {
				AddLandmark(create_Landmark(ltag, Length, level, value));
			}
			else if (!strcmp(ltype, "Girth")) {
				AddLandmark(create_Landmark(ltag, Girth, level, value));
			}
			else {
				printf("Non-existent Landmark type %s\n", ltype);
			}
		}
	}

	if (isTexture)
		m_RenderType = RENDER_TEXTURE;
	else
		m_RenderType = RENDER_SHADE;


	// printf("handR : %f %f %f\nhandL : %f %f %f\n", handR.x, handR.y, handR.z, handL.x, handL.y, handL.z);

	// Set-up skeleton and bones
	if (!SetSkeleton())
		return false;

	// Set body segment with bones
	if (!SetSegment())
		return false;

	// Assign weights to bones
	if (!AssignWeight())
		return false;


	m_BoundingBox->SetBounds(minX, maxX, minY, maxY, minZ, maxZ);

	m_MinX = minX;
	m_MaxX = maxX;
	m_MinY = minY;
	m_MaxY = maxY;
	m_MinZ = minZ;
	m_MaxZ = maxZ;

	UpdateVertBuff();
	UpdateIndexBuff();

	return true;
}

bool HumanObject::SetSkeleton() {
	std::cout << "Setting Skeleton... ";
	if (m_Skeleton == NULL) {
		printf("No skeleton instance exists.\n");
		return false;
	}

	// Add Bone to Skeleton
	m_Skeleton->AddBone(Bone_neck, create_Bone((*m_Skeleton->m_Joints)[Joint_neck], (*m_Skeleton->m_Joints)[Joint_shoulderMid]));
	m_Skeleton->AddBone(Bone_spine3, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderMid], (*m_Skeleton->m_Joints)[Joint_spine3]));
	m_Skeleton->AddBone(Bone_spine2, create_Bone((*m_Skeleton->m_Joints)[Joint_spine3], (*m_Skeleton->m_Joints)[Joint_spine2]));
	m_Skeleton->AddBone(Bone_spine1, create_Bone((*m_Skeleton->m_Joints)[Joint_spine2], (*m_Skeleton->m_Joints)[Joint_spine1]));
	m_Skeleton->AddBone(Bone_spine, create_Bone((*m_Skeleton->m_Joints)[Joint_spine1], (*m_Skeleton->m_Joints)[Joint_spine]));
	m_Skeleton->AddBone(Bone_waist, create_Bone((*m_Skeleton->m_Joints)[Joint_spine], (*m_Skeleton->m_Joints)[Joint_waist]));
	m_Skeleton->AddBone(Bone_pelvis, create_Bone((*m_Skeleton->m_Joints)[Joint_waist], (*m_Skeleton->m_Joints)[Joint_pelvisMid]));

	m_Skeleton->AddBone(Bone_collarboneR, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderMid], (*m_Skeleton->m_Joints)[Joint_collarboneR]));
	m_Skeleton->AddBone(Bone_shoulderR, create_Bone((*m_Skeleton->m_Joints)[Joint_collarboneR], (*m_Skeleton->m_Joints)[Joint_shoulderR]));
	m_Skeleton->AddBone(Bone_upperArmR, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderR], (*m_Skeleton->m_Joints)[Joint_shoulderTwistR]));
	m_Skeleton->AddBone(Bone_upperArm1R, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderTwistR], (*m_Skeleton->m_Joints)[Joint_elbowR]));
	m_Skeleton->AddBone(Bone_lowerArmR, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowR], (*m_Skeleton->m_Joints)[Joint_elbowTwistR]));
	m_Skeleton->AddBone(Bone_lowerArm1R, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowTwistR], (*m_Skeleton->m_Joints)[Joint_elbowTwist1R]));
	m_Skeleton->AddBone(Bone_lowerArm2R, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowTwist1R], (*m_Skeleton->m_Joints)[Joint_wristR]));

	m_Skeleton->AddBone(Bone_collarboneL, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderMid], (*m_Skeleton->m_Joints)[Joint_collarboneL]));
	m_Skeleton->AddBone(Bone_shoulderL, create_Bone((*m_Skeleton->m_Joints)[Joint_collarboneL], (*m_Skeleton->m_Joints)[Joint_shoulderL]));
	m_Skeleton->AddBone(Bone_upperArmL, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderL], (*m_Skeleton->m_Joints)[Joint_shoulderTwistL]));
	m_Skeleton->AddBone(Bone_upperArm1L, create_Bone((*m_Skeleton->m_Joints)[Joint_shoulderTwistL], (*m_Skeleton->m_Joints)[Joint_elbowL]));
	m_Skeleton->AddBone(Bone_lowerArmL, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowL], (*m_Skeleton->m_Joints)[Joint_elbowTwistL]));
	m_Skeleton->AddBone(Bone_lowerArm1L, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowTwistL], (*m_Skeleton->m_Joints)[Joint_elbowTwist1L]));
	m_Skeleton->AddBone(Bone_lowerArm2L, create_Bone((*m_Skeleton->m_Joints)[Joint_elbowTwist1L], (*m_Skeleton->m_Joints)[Joint_wristL]));

	m_Skeleton->AddBone(Bone_pelvisR, create_Bone((*m_Skeleton->m_Joints)[Joint_pelvisMid], (*m_Skeleton->m_Joints)[Joint_pelvisR]));
	m_Skeleton->AddBone(Bone_hipR, create_Bone((*m_Skeleton->m_Joints)[Joint_pelvisR], (*m_Skeleton->m_Joints)[Joint_hipR]));
	m_Skeleton->AddBone(Bone_upperLegR, create_Bone((*m_Skeleton->m_Joints)[Joint_hipR], (*m_Skeleton->m_Joints)[Joint_hipTwistR]));
	m_Skeleton->AddBone(Bone_upperLeg1R, create_Bone((*m_Skeleton->m_Joints)[Joint_hipTwistR], (*m_Skeleton->m_Joints)[Joint_kneeR]));
	m_Skeleton->AddBone(Bone_lowerLegR, create_Bone((*m_Skeleton->m_Joints)[Joint_kneeR], (*m_Skeleton->m_Joints)[Joint_ankleR]));

	m_Skeleton->AddBone(Bone_pelvisL, create_Bone((*m_Skeleton->m_Joints)[Joint_pelvisMid], (*m_Skeleton->m_Joints)[Joint_pelvisL]));
	m_Skeleton->AddBone(Bone_hipL, create_Bone((*m_Skeleton->m_Joints)[Joint_pelvisL], (*m_Skeleton->m_Joints)[Joint_hipL]));
	m_Skeleton->AddBone(Bone_upperLegL, create_Bone((*m_Skeleton->m_Joints)[Joint_hipL], (*m_Skeleton->m_Joints)[Joint_hipTwistL]));
	m_Skeleton->AddBone(Bone_upperLeg1L, create_Bone((*m_Skeleton->m_Joints)[Joint_hipTwistL], (*m_Skeleton->m_Joints)[Joint_kneeL]));
	m_Skeleton->AddBone(Bone_lowerLegL, create_Bone((*m_Skeleton->m_Joints)[Joint_kneeL], (*m_Skeleton->m_Joints)[Joint_ankleL]));

	m_Skeleton->AddBone(Bone_ribR, create_Bone((*m_Skeleton->m_Joints)[Joint_spine3], (*m_Skeleton->m_Joints)[Joint_ribR]));
	m_Skeleton->AddBone(Bone_ribL, create_Bone((*m_Skeleton->m_Joints)[Joint_spine3], (*m_Skeleton->m_Joints)[Joint_ribL]));

	m_Skeleton->AddBone(Bone_handR, create_Bone((*m_Skeleton->m_Joints)[Joint_wristR], (*m_Skeleton->m_Joints)[Joint_handR]));
	m_Skeleton->AddBone(Bone_handL, create_Bone((*m_Skeleton->m_Joints)[Joint_wristL], (*m_Skeleton->m_Joints)[Joint_handL]));


	// Set Bone Parent and Child
	(*m_Skeleton->m_Bones)[Bone_neck]->SetChild((*m_Skeleton->m_Bones)[Bone_spine3]);
	(*m_Skeleton->m_Bones)[Bone_neck]->SetChild((*m_Skeleton->m_Bones)[Bone_collarboneR]);
	(*m_Skeleton->m_Bones)[Bone_neck]->SetChild((*m_Skeleton->m_Bones)[Bone_collarboneL]);

	(*m_Skeleton->m_Bones)[Bone_spine3]->SetChild((*m_Skeleton->m_Bones)[Bone_spine2]);
	(*m_Skeleton->m_Bones)[Bone_spine3]->SetChild((*m_Skeleton->m_Bones)[Bone_ribR]);
	(*m_Skeleton->m_Bones)[Bone_spine3]->SetChild((*m_Skeleton->m_Bones)[Bone_ribL]);

	(*m_Skeleton->m_Bones)[Bone_spine2]->SetChild((*m_Skeleton->m_Bones)[Bone_spine1]);

	(*m_Skeleton->m_Bones)[Bone_spine1]->SetChild((*m_Skeleton->m_Bones)[Bone_spine]);

	(*m_Skeleton->m_Bones)[Bone_spine]->SetChild((*m_Skeleton->m_Bones)[Bone_waist]);

	(*m_Skeleton->m_Bones)[Bone_waist]->SetChild((*m_Skeleton->m_Bones)[Bone_pelvis]);

	(*m_Skeleton->m_Bones)[Bone_pelvis]->SetChild((*m_Skeleton->m_Bones)[Bone_pelvisR]);
	(*m_Skeleton->m_Bones)[Bone_pelvis]->SetChild((*m_Skeleton->m_Bones)[Bone_pelvisL]);

	(*m_Skeleton->m_Bones)[Bone_collarboneR]->SetChild((*m_Skeleton->m_Bones)[Bone_shoulderR]);

	(*m_Skeleton->m_Bones)[Bone_shoulderR]->SetChild((*m_Skeleton->m_Bones)[Bone_upperArmR]);

	(*m_Skeleton->m_Bones)[Bone_upperArmR]->SetChild((*m_Skeleton->m_Bones)[Bone_upperArm1R]);

	(*m_Skeleton->m_Bones)[Bone_upperArm1R]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArmR]);

	(*m_Skeleton->m_Bones)[Bone_lowerArmR]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArm1R]);

	(*m_Skeleton->m_Bones)[Bone_lowerArm1R]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArm2R]);

	(*m_Skeleton->m_Bones)[Bone_collarboneL]->SetChild((*m_Skeleton->m_Bones)[Bone_shoulderL]);

	(*m_Skeleton->m_Bones)[Bone_shoulderL]->SetChild((*m_Skeleton->m_Bones)[Bone_upperArmL]);

	(*m_Skeleton->m_Bones)[Bone_upperArmL]->SetChild((*m_Skeleton->m_Bones)[Bone_upperArm1L]);

	(*m_Skeleton->m_Bones)[Bone_upperArm1L]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArmL]);

	(*m_Skeleton->m_Bones)[Bone_lowerArmL]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArm1L]);

	(*m_Skeleton->m_Bones)[Bone_lowerArm1L]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerArm2L]);

	(*m_Skeleton->m_Bones)[Bone_pelvisR]->SetChild((*m_Skeleton->m_Bones)[Bone_hipR]);

	(*m_Skeleton->m_Bones)[Bone_hipR]->SetChild((*m_Skeleton->m_Bones)[Bone_upperLegR]);

	(*m_Skeleton->m_Bones)[Bone_upperLegR]->SetChild((*m_Skeleton->m_Bones)[Bone_upperLeg1R]);

	(*m_Skeleton->m_Bones)[Bone_upperLeg1R]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerLegR]);

	(*m_Skeleton->m_Bones)[Bone_pelvisL]->SetChild((*m_Skeleton->m_Bones)[Bone_hipL]);

	(*m_Skeleton->m_Bones)[Bone_hipL]->SetChild((*m_Skeleton->m_Bones)[Bone_upperLegL]);

	(*m_Skeleton->m_Bones)[Bone_upperLegL]->SetChild((*m_Skeleton->m_Bones)[Bone_upperLeg1L]);

	(*m_Skeleton->m_Bones)[Bone_upperLeg1L]->SetChild((*m_Skeleton->m_Bones)[Bone_lowerLegL]);

	(*m_Skeleton->m_Bones)[Bone_lowerArm2R]->SetChild((*m_Skeleton->m_Bones)[Bone_handR]);
	(*m_Skeleton->m_Bones)[Bone_lowerArm2L]->SetChild((*m_Skeleton->m_Bones)[Bone_handL]);

	// Set leaf
	(*m_Skeleton->m_Bones)[Bone_handR]->isLeaf = true;
	(*m_Skeleton->m_Bones)[Bone_handL]->isLeaf = true;

	std::cout << "Successful!" << std::endl;
}

bool HumanObject::SetSegment() {
	std::cout << "Setting Segments... " << std::endl;

	// 각 Bone에 가까운 점들로 Segment를 구성한다
	for (mjVertex *v : *m_Vertices) {
		int closestBoneIdx = -1;
		float minDistance = INFINITY;

		for (mjBone *b : (*m_Skeleton->m_Bones)) {
			mjPos3 upperJoint = *b->m_UpperJoint->m_Coord;
			mjPos3 lowerJoint = *b->m_LowerJoint->m_Coord;

			float distance = distToLineSegment(*v->m_Coord, upperJoint, lowerJoint);

			if (distance < minDistance) {
				closestBoneIdx = b->m_Idx;
				minDistance = distance;
			}
		}

		if (closestBoneIdx != -1) {
			m_Segment[closestBoneIdx].push_back(v);
			v->m_Segment = closestBoneIdx;
		}
		else {
			std::cout << "No closest bone found for vertex " << v->m_Idx << " !!\n" << std::endl;
			return false;
		}
	}

	UpdateVertBuff();

	std::cout << "Successful!" << std::endl;
	return true;
}

bool HumanObject::AssignWeight() {
	char* fname;

	if (m_Gender == Female)
		fname = (char*) "Weight_Female";
	else
		fname = (char*) "Weight_Male";

	FILE *fp;
	fopen_s(&fp, fname, "r");

	if (!fp) {
		printf("Loading %s failed...\n", fname);
		return false;
	}
	else {
		printf("Importing %s...\n", fname);
	}

	int vertIdx = 0;
	int bone0, bone1, bone2, bone3;
	float weight0, weight1, weight2, weight3;
	while (fscanf_s(fp, "%d%d%d%d%f%f%f%f", &bone0, &bone1, &bone2, &bone3, &weight0, &weight1, &weight2, &weight3) != EOF) {
		mjVertex *v = (*m_Vertices)[vertIdx];

		// 해당 bone에 영향을 받는 vertex와 corresponding weight를 bone의 리스트에 삽입한다
		(*m_Skeleton->m_Bones)[bone0]->AddVertexWeight(v, weight0);
		(*m_Skeleton->m_Bones)[bone1]->AddVertexWeight(v, weight1);
		(*m_Skeleton->m_Bones)[bone2]->AddVertexWeight(v, weight2);
		(*m_Skeleton->m_Bones)[bone3]->AddVertexWeight(v, weight3);

		vertIdx++;
	}

	return true;
}

bool HumanObject::WriteObj(const char* fname) {
	printf("Writing to %s...\n", fname);

	std::ofstream oFile(fname);

	if (m_Materials != NULL) {
		// Assuming a single material file
		oFile << "mtllib " << mtlFilename << std::endl;
	}

	for (mjVertex *v : *m_Vertices) {
		oFile << "v " <<  v->m_Coord->x << " " << v->m_Coord->y << " " << v->m_Coord->z << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	for (mjTexel *t : *m_Texels) {
		oFile << "vt " << t->m_Coord->x << " " << t->m_Coord->y << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	for (mjNormal *n : *m_Normals) {
		oFile << "vn " << n->m_Dir->x << " " << n->m_Dir->y << " " << n->m_Dir->z << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	// 일단은 material이 하나라고 처리하기 때문에 그냥 usemtl로 사용
	oFile << "usemtl default" << std::endl;

	for (mjFace *f : *m_Faces) {
		// std::string mtlName = (f->m_Material == NULL) ? "default" : f->m_Material->m_Name;
		
		int idx0 = f->GetVertIdx(0) + 1;
		int idx1 = f->GetVertIdx(1) + 1;
		int idx2 = f->GetVertIdx(2) + 1;

		if (m_Texels->empty() && m_Normals->empty()) {
			oFile << "f " << idx0 << " " << idx1 << " " << idx2 << std::endl;
		}
		else if (m_Normals->empty()) {
			int t0 = f->GetTexIdx(0) + 1;
			int t1 = f->GetTexIdx(1) + 1;
			int t2 = f->GetTexIdx(2) + 1;

			oFile << "f " 
				<< idx0 <<  "/" << t0 << " " 
				<< idx1 << "/" << t1 << " "
				<< idx2 << "/" << t2 << " "
				<< std::endl;
		}
		else {
			int t0 = f->GetTexIdx(0) + 1;
			int t1 = f->GetTexIdx(1) + 1;
			int t2 = f->GetTexIdx(2) + 1;

			int n0 = f->GetNormIdx(0) + 1;
			int n1 = f->GetNormIdx(1) + 1;
			int n2 = f->GetNormIdx(2) + 1;

			oFile << "f " 
				<< idx0 <<  "/" << t0 << "/" << n0 << " " 
				<< idx1 << "/" << t1 << "/" << n1 << " "
				<< idx2 << "/" << t2 << "/" << n2 << " "
				<< std::endl;
		}
	}

	oFile.close();
	return true;
}

void HumanObject::WriteHuman(const char* fname) {
	std::ofstream oFile(fname);

	//////////////////////////////////////////////// write as OBJ

	if (m_Materials != NULL) {
		// Assuming a single material file
		oFile << "mtllib " << mtlFilename << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	for (mjVertex *v : *m_Vertices) {
		oFile << "v " <<  v->m_Coord->x << " " << v->m_Coord->y << " " << v->m_Coord->z << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	for (mjTexel *t : *m_Texels) {
		oFile << "vt " << t->m_Coord->x << " " << t->m_Coord->y << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	for (mjNormal *n : *m_Normals) {
		oFile << "vn " << n->m_Dir->x << " " << n->m_Dir->y << " " << n->m_Dir->z << std::endl;
	}

	oFile << std::endl;
	oFile << std::endl;

	// 일단은 material이 하나라고 처리하기 때문에 그냥 usemtl로 사용
	oFile << "usemtl default" << std::endl;

	for (mjFace *f : *m_Faces) {
		// std::string mtlName = (f->m_Material == NULL) ? "default" : f->m_Material->m_Name;
		
		int idx0 = f->GetVertIdx(0) + 1;
		int idx1 = f->GetVertIdx(1) + 1;
		int idx2 = f->GetVertIdx(2) + 1;

		if (m_Texels->empty() && m_Normals->empty()) {
			oFile << "f " << idx0 << " " << idx1 << " " << idx2 << std::endl;
		}
		else if (m_Normals->empty()) {
			int t0 = f->GetTexIdx(0) + 1;
			int t1 = f->GetTexIdx(1) + 1;
			int t2 = f->GetTexIdx(2) + 1;

			oFile << "f " 
				<< idx0 <<  "/" << t0 << " " 
				<< idx1 << "/" << t1 << " "
				<< idx2 << "/" << t2 << " "
				<< std::endl;
		}
		else {
			int t0 = f->GetTexIdx(0) + 1;
			int t1 = f->GetTexIdx(1) + 1;
			int t2 = f->GetTexIdx(2) + 1;

			int n0 = f->GetNormIdx(0) + 1;
			int n1 = f->GetNormIdx(1) + 1;
			int n2 = f->GetNormIdx(2) + 1;

			oFile << "f " 
				<< idx0 <<  "/" << t0 << "/" << n0 << " " 
				<< idx1 << "/" << t1 << "/" << n1 << " "
				<< idx2 << "/" << t2 << "/" << n2 << " "
				<< std::endl;
		}
	}


	//////////////////////////////////////////////// JOINTS

	oFile << std::endl;
	oFile << std::endl;

	for (mjJoint *joint : *m_Skeleton->m_Joints) {
		oFile << "jo " << joint->m_Coord->x << " " << joint->m_Coord->y << " " << joint->m_Coord->z << std::endl;
	}

	//////////////////////////////////////////////// LANDMARKS

	oFile << std::endl;
	oFile << std::endl;

	for (mjLandmark * landmark : *m_Landmarks) {
		std::string type = (landmark->m_Type == Girth) ? "Girth" : "Length";

		oFile << "la " << landmark->m_Name << " " << type << " " << landmark->m_Level << " " << landmark->m_Value << std::endl;
	}

	oFile.close();
}


void HumanObject::SetMale(){
	m_Gender = Male;
}

void HumanObject::SetFemale() {
	m_Gender = Female;
}


void HumanObject::AddVertex(mjVertex *v) {
	v->m_Idx = m_Vertices->size();
	m_Vertices->push_back(v);
}

void HumanObject::AddTexel(mjTexel *t) {
	t->m_Idx = m_Texels->size();
	m_Texels->push_back(t);
}

void HumanObject::AddNormal(mjNormal *n) {
	n->m_Idx = m_Normals->size();
	m_Normals->push_back(n);
}

void HumanObject::AddEdge(mjVertex *v0, mjVertex *v1) {

}

void HumanObject::AddEdge(mjEdge *e0) {

}

void HumanObject::AddFace(mjFace *f) {
	f->m_Idx = m_Faces->size();
	m_Faces->push_back(f);
}

void HumanObject::AddTexture(mjTexture *pTexture)
{
	m_Textures->push_back(pTexture);
}

void HumanObject::AddMaterial(mjMaterial *pMaterial)
{
	// 재질의 인덱스를 설정하고 재질 배열에 추가한다.
	pMaterial->m_Idx = (int) m_Materials->size();
	m_Materials->push_back(pMaterial);
}

void HumanObject::AddLandmark(mjLandmark *pLandmark) {
	pLandmark->m_Idx = m_Landmarks->size();
	pLandmark->m_Human = this;
	m_Landmarks->push_back(pLandmark);
}

void HumanObject::UpdateJoints() {

}

void HumanObject::UpdateLandmarks() {
	for (mjLandmark *landmark : *m_Landmarks) {
		landmark->GetSize();
	}
}


void HumanObject::UpdateIndexBuff() {
	m_IndexBuf.clear();

	for (mjFace *f : *m_Faces) {
		std::string mtlName = (f->m_Material == NULL) ? "default" : f->m_Material->m_Name;

		m_IndexBuf[mtlName].push_back(3 * f->m_Idx);
		m_IndexBuf[mtlName].push_back(3 * f->m_Idx + 1);
		m_IndexBuf[mtlName].push_back(3 * f->m_Idx + 2);
	}
}


void HumanObject::UpdateVertBuff() {
	m_VertBuf.clear();

	// Generate rand colors for segments
	float r[Bone_Num], g[Bone_Num], b[Bone_Num];
	for (int i = 0; i < Bone_Num; i++) {
		r[i] = rand() / double(RAND_MAX);
		g[i] = rand() / double(RAND_MAX);
		b[i] = rand() / double(RAND_MAX);
	}


	for (mjFace *f : *m_Faces) {
		std::string mtlName = (f->m_Material == NULL) ? "default" : f->m_Material->m_Name;

		for (int i = 0; i < 3; i++) {
			mjVertex *v = f->GetVert(i);

			// Position
			m_VertBuf[mtlName].push_back(v->m_Coord->x);
			m_VertBuf[mtlName].push_back(v->m_Coord->y);
			m_VertBuf[mtlName].push_back(v->m_Coord->z);

			// Texel
			m_VertBuf[mtlName].push_back(v->m_Texel->m_Coord->x);
			m_VertBuf[mtlName].push_back(v->m_Texel->m_Coord->y);

			// Normal
			m_VertBuf[mtlName].push_back(v->m_Normal->m_Dir->x);
			m_VertBuf[mtlName].push_back(v->m_Normal->m_Dir->y);
			m_VertBuf[mtlName].push_back(v->m_Normal->m_Dir->z);

			// Color
			// Bone Segment 별로 다른 색상 부여
			m_VertBuf[mtlName].push_back(r[v->m_Segment]);
			m_VertBuf[mtlName].push_back(g[v->m_Segment]);
			m_VertBuf[mtlName].push_back(b[v->m_Segment]);
		}
	}


	/*
	for (mjVertex *v : *m_Vertices) {
		// Position
		m_VertBuf.push_back(v->m_Coord->x);
		m_VertBuf.push_back(v->m_Coord->y);
		m_VertBuf.push_back(v->m_Coord->z);

		// Texel
		m_VertBuf.push_back(v->m_Texel->x);
		m_VertBuf.push_back(v->m_Texel->y);

		// Normal
		m_VertBuf.push_back(v->m_Normal->m_Dir->x);
		m_VertBuf.push_back(v->m_Normal->m_Dir->y);
		m_VertBuf.push_back(v->m_Normal->m_Dir->z);

		// Color
		// Bone Segment 별로 다른 색상 부여
		m_VertBuf.push_back(r[v->m_Segment]);
		m_VertBuf.push_back(g[v->m_Segment]);
		m_VertBuf.push_back(b[v->m_Segment]);
	}
	*/
}


void HumanObject::UpdateNormBuff() {
	m_NormBuf.clear();

}

void HumanObject::UpdateTexBuff() {
	m_TexBuf.clear();

}

mjMaterial *HumanObject::GetMaterial(std::string fname) {
	for (mjMaterial *m : *m_Materials) {
		if (m->m_Name == fname) {
			return m;
		}
	}

	return NULL;
}

mjTexture *HumanObject::GetTexture(std::string fname)
{
	for (mjTexture *t : *m_Textures)
	{
		if (t->m_Filename == fname)
			return t;
	}

	return NULL;
}


/////// Measure 
// 측정항목 개수
int HumanObject::GetLandmarkNum() {
	return m_Landmarks->size();
}



// i번째 측정항목 이름
// @param
// [in] i : 
// [out] buffer : buffer에 이름 저장
void HumanObject::GetLandmarkName(int i, char* buffer) {
	buffer = (char*) (*m_Landmarks)[i]->m_Name.c_str();
}


// i번째 측정항목 값
float HumanObject::GetLandmarkValue(int i) {
	return (*m_Landmarks)[i]->m_Value;
}


// lname 측정항목 값
// return : 맞는 이름이 없을 경우 -1
float HumanObject::GetLandmarkValue(char* lname) {
	for (int i = 0; i < GetLandmarkNum(); i++) {
		if ((*m_Landmarks)[i]->m_Name == lname) {
			return (*m_Landmarks)[i]->m_Value;
		}
	}
	return -1; 
}


// sizes 순서대로 치수 변형
void HumanObject::SetSizes(float *sizes) {
}

// i번째 측정항목 치수 변형
// 일단은 Segment를 각 랜드마크마다 하드코딩해서 지정해준다 (20. 8. 25)
// ToDo::임의의 Landmark segment import 자동화 필요. 방법 구상해볼 것.
// @param[in] i : i번째 랜드마크
// @param[in] value : 변형되기를 원하는 치수
void HumanObject::SetSize(int i, float value) {
	std::cout << "Setting size... ";

	mjLandmark *thisLandmark = (*m_Landmarks)[i];
	// i번째 랜드마크가 Girth일 경우,
	if (thisLandmark->m_Type == Girth) {
		std::cout << "Landmark Girth type... ";
		// Girth 타입을 갖는 랜드마크들의 level을 비교하여
		// 해당 랜드마크 바로 위, 아래 level을 갖는 level를 각각 upperBound, lowerBound로 정의한다
		float upperBound = m_BoundingBox->m_MaxY,
			lowerBound = m_BoundingBox->m_MinY;
		for (mjLandmark *l : *m_Landmarks) {
			if (l->m_Type == Girth) {
				// Set upperBound
				if (l->m_Level < upperBound && l->m_Level > thisLandmark->m_Level) {
					upperBound = l->m_Level;
				}
				
				// Set lowerBound
				if (l->m_Level < thisLandmark->m_Level && l->m_Level > lowerBound) {
					lowerBound = l->m_Level;
				}
			}
		}
		
		if (i == Bust && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_neck);
			thisLandmark->SetSegment(Bone_spine3);
			thisLandmark->SetSegment(Bone_spine2);
			thisLandmark->SetSegment(Bone_spine1);
			thisLandmark->SetSegment(Bone_spine);
			thisLandmark->SetSegment(Bone_waist);
			thisLandmark->SetSegment(Bone_pelvis);

			thisLandmark->SetSegment(Bone_collarboneR);
			thisLandmark->SetSegment(Bone_collarboneL);
			thisLandmark->SetSegment(Bone_shoulderR);
			thisLandmark->SetSegment(Bone_shoulderL);

			thisLandmark->SetSegment(Bone_ribR);
			thisLandmark->SetSegment(Bone_ribL);

			// ToDo::Bust sizing의 경우 joint position이 업데이트되어야한다
			float deformationScale = value / thisLandmark->m_Value;
			float jointMovement_R = abs((*m_Skeleton->m_Joints)[Joint_shoulderR]->m_Coord->x * (deformationScale - 1));
			float jointMovement_L = abs((*m_Skeleton->m_Joints)[Joint_shoulderL]->m_Coord->x * (deformationScale - 1));

			(*m_Skeleton->m_Joints)[Joint_shoulderR]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_shoulderTwistR]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_elbowR]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_elbowTwistR]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_elbowTwist1R]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_wristR]->m_Coord->x -= jointMovement_R;
			(*m_Skeleton->m_Joints)[Joint_handR]->m_Coord->x -= jointMovement_R;

			for (mjVertex *v : m_Segment[Bone_upperArmR]) {
				v->m_Coord->x -= jointMovement_R;
			}
			for (mjVertex *v : m_Segment[Bone_upperArm1R]) {
				v->m_Coord->x -= jointMovement_R;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArmR]) {
				v->m_Coord->x -= jointMovement_R;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArm1R]) {
				v->m_Coord->x -= jointMovement_R;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArm2R]) {
				v->m_Coord->x -= jointMovement_R;
			}
			for (mjVertex *v : m_Segment[Bone_handR]) {
				v->m_Coord->x -= jointMovement_R;
			}


			(*m_Skeleton->m_Joints)[Joint_shoulderL]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_shoulderTwistL]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_elbowL]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_elbowTwistL]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_elbowTwist1L]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_wristL]->m_Coord->x += jointMovement_L;
			(*m_Skeleton->m_Joints)[Joint_handL]->m_Coord->x += jointMovement_L;

			for (mjVertex *v : m_Segment[Bone_upperArmL]) {
				v->m_Coord->x += jointMovement_L;
			}
			for (mjVertex *v : m_Segment[Bone_upperArm1L]) {
				v->m_Coord->x += jointMovement_L;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArmL]) {
				v->m_Coord->x += jointMovement_L;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArm1L]) {
				v->m_Coord->x += jointMovement_L;
			}
			for (mjVertex *v : m_Segment[Bone_lowerArm2L]) {
				v->m_Coord->x += jointMovement_L;
			}
			for (mjVertex *v : m_Segment[Bone_handL]) {
				v->m_Coord->x += jointMovement_L;
			}
		}
		if (i == Waist && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_neck);
			thisLandmark->SetSegment(Bone_spine3);
			thisLandmark->SetSegment(Bone_spine2);
			thisLandmark->SetSegment(Bone_spine1);
			thisLandmark->SetSegment(Bone_spine);
			thisLandmark->SetSegment(Bone_waist);
			thisLandmark->SetSegment(Bone_pelvis);

			thisLandmark->SetSegment(Bone_pelvisR);
			thisLandmark->SetSegment(Bone_hipR);
			thisLandmark->SetSegment(Bone_upperLegR);
			thisLandmark->SetSegment(Bone_upperLeg1R);
			thisLandmark->SetSegment(Bone_lowerLegR);

			thisLandmark->SetSegment(Bone_pelvisL);
			thisLandmark->SetSegment(Bone_hipL);
			thisLandmark->SetSegment(Bone_upperLegL);
			thisLandmark->SetSegment(Bone_upperLeg1L);
			thisLandmark->SetSegment(Bone_lowerLegL);

			thisLandmark->SetSegment(Bone_collarboneR);
			thisLandmark->SetSegment(Bone_collarboneL);
			thisLandmark->SetSegment(Bone_shoulderR);
			thisLandmark->SetSegment(Bone_shoulderL);

			thisLandmark->SetSegment(Bone_ribR);
			thisLandmark->SetSegment(Bone_ribL);
		}
		if (i == Hip && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_spine);
			thisLandmark->SetSegment(Bone_waist);
			thisLandmark->SetSegment(Bone_pelvis);

			thisLandmark->SetSegment(Bone_pelvisR);
			thisLandmark->SetSegment(Bone_hipR);
			thisLandmark->SetSegment(Bone_upperLegR);
			thisLandmark->SetSegment(Bone_upperLeg1R);
			thisLandmark->SetSegment(Bone_lowerLegR);

			thisLandmark->SetSegment(Bone_pelvisL);
			thisLandmark->SetSegment(Bone_hipL);
			thisLandmark->SetSegment(Bone_upperLegL);
			thisLandmark->SetSegment(Bone_upperLeg1L);
			thisLandmark->SetSegment(Bone_lowerLegL);
		}

		thisLandmark->Deform(value, upperBound, lowerBound);
	}
	// i번째 랜드마크가 Length일 경우,
	else if (thisLandmark->m_Type == Length) {
		std::cout << "Landmark Length type... ";

		// 각 segment를 구성하는 bone vector에 따라 scale을 수행한다
		// Shoulder length 경우, 가로 길이이기 때문에 따로 처리 필요
		if (i == Height && thisLandmark->GetSegments().empty()) {
			for (int i = 0; i < Bone_Num; i++)
				thisLandmark->SetSegment(i);
		}
		if (i == ArmLengthR && thisLandmark->GetSegments().empty()) {
			// thisLandmark->SetSegment(Bone_shoulderR);
			thisLandmark->SetSegment(Bone_upperArmR);
			thisLandmark->SetSegment(Bone_upperArm1R);
			thisLandmark->SetSegment(Bone_lowerArmR);
			thisLandmark->SetSegment(Bone_lowerArm1R);
			/*
			thisLandmark->SetSegment(Bone_lowerArm2R);
			*/
		}
		if (i == ArmLengthL && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_upperArmL);
			thisLandmark->SetSegment(Bone_upperArm1L);
			thisLandmark->SetSegment(Bone_lowerArmL);
			thisLandmark->SetSegment(Bone_lowerArm1L);
			thisLandmark->SetSegment(Bone_lowerArm2L);
		}
		if (i == LegLengthR && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_hipR);
			thisLandmark->SetSegment(Bone_upperLegR);
			thisLandmark->SetSegment(Bone_upperLeg1R);
			thisLandmark->SetSegment(Bone_lowerLegR);
		}
		if (i == LegLengthL && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_hipL);
			thisLandmark->SetSegment(Bone_upperLegL);
			thisLandmark->SetSegment(Bone_upperLeg1L);
			thisLandmark->SetSegment(Bone_lowerLegL);
		}
		if (i == ShoulderLength && thisLandmark->GetSegments().empty()) {
			thisLandmark->SetSegment(Bone_neck);
			thisLandmark->SetSegment(Bone_spine3);
			thisLandmark->SetSegment(Bone_spine2);
			thisLandmark->SetSegment(Bone_spine1);
			thisLandmark->SetSegment(Bone_spine);
			thisLandmark->SetSegment(Bone_waist);

			thisLandmark->SetSegment(Bone_collarboneR);
			thisLandmark->SetSegment(Bone_shoulderR);
			
			thisLandmark->SetSegment(Bone_collarboneL);
			thisLandmark->SetSegment(Bone_shoulderL);
			// ToDo::Shoulder length sizing의 경우에도 joint position이 업데이트 되어야 한다
		}

		thisLandmark->Deform(value);
	}

	// Update Joint positions (for Bust && shoulder length)
	// Joint position update에 따른 vertex 위치의 상대적 이동
	// UpdateJoints();

	// Update Landmarks
	// UpdateLandmarks();

	// Update vertex bindings for render function
	UpdateVertBuff();
}


// lname 측정항목 치수 변형
// @param[in] lname : 이름이 lname인 랜드마크
// @param[in] value : 변형되기를 원하는 치수
void HumanObject::SetSize(char* lname, float value) {

}


/////// Geometry
int HumanObject::GetVertNum() {
	return m_Vertices->size();
}


int HumanObject::GetFaceNum() {
	return m_Faces->size();
}


// i번째 정점의 좌표 반환
// @param
// [out] coord
void HumanObject::GetVert(int i, float *coord) {
	coord[0] = (*m_Vertices)[i]->m_Coord->x;
	coord[1] = (*m_Vertices)[i]->m_Coord->y;
	coord[2] = (*m_Vertices)[i]->m_Coord->z;
}


// 전체 정점 좌표 반환
// @ params
// [out] coord
void HumanObject::GetVerts(float *coord) {
	int i = 0;
	for (mjVertex *v : *m_Vertices) {
		coord[i] = v->m_Coord->x;
		coord[i + 1] = v->m_Coord->y;
		coord[i + 2] = v->m_Coord->z;
		i += 3;
	}
}


// i번째 face의 번호를 반환
// @ params
// [out] node
void HumanObject::GetIndex(int i, int *node) {
	*node = (*m_Faces)[i]->m_Idx;
}


// 모든 face 번호를 반환
// @ params
// [out] node
void HumanObject::GetIndices(int *node) {
	int i = 0;
	for (mjFace *f : *m_Faces) {
		node[i] = f->m_Idx;
		i++;
	}
}



/////// Bounding / Collision
// 지정된 이름의 파트번호
// @ params
// [in] name
int HumanObject::GetSegmentNum(char* name) {
	/*
	if (!strcmp(name, "Head")) {

	}
	*/
	return 0;
}

// i번째 부위의 시작점 좌표
// @ params
// [in] i
// [out] coord
void HumanObject::GetSegmentOrigin(int i, float* coord) {

}

// i번째 부위의 종점 좌표
// @ params
// [in] i
// [out] coord
void HumanObject::GetSegmentTermination(int i, float* coord) {

}

// i번째 부위에 속한 vertex들의 indices
// @ params
// [in] i
// [out] nums
void HumanObject::GetSegmentVertIndices(int i, int* nums) {

}

// i번째 부위에 속한 vertex들의 좌표들
// @ params
// [in] i
// [out] coord
void HumanObject::GetSegmentVertPos(int i, float* coord) {

}


/////// Pose
float GetAngle(mjVec3 a, mjVec3 b) {
	float degree = 0;
	a.normalize();
	b.normalize();

	mjVec3 dot = a * b;

	float acosRad = acos(dot.x + dot.y + dot.z);

	degree = acosRad * (180 / M_PI);

	return degree;
}

void HumanObject::SetTPose(int s) {
	mjPos3 UpperArmL = (*(*m_Skeleton->m_Bones)[Bone_upperArmL]->m_UpperJoint->m_Coord),
		LowerArmL = (*(*m_Skeleton->m_Bones)[Bone_upperArmL]->m_LowerJoint->m_Coord);

	mjVec3 arm_left = LowerArmL - UpperArmL,
		axis = mjVec3(0, -1, 0);


	// degree만큼 움직이는게 아니라 degree만큼 팔을 벌리고 있어야한다
	// float degree = abs(90 * s - GetAngle(arm_left, axis));
	float degree = -90/2;

	float radian = degree * M_PI / 180;

	std::cout << "\nInitial Left Arm angle is " << GetAngle(arm_left, axis) << std::endl;
	std::cout << "\nRotate by angle in degrees " << degree << " and in radians " << radian << std::endl;


	/*** Copyright GAIA : from gmath.cpp SetFromMatrix(double *mat, bool isGL) ***/
	mjMatrix4x4 rotationMatrix;

	rotationMatrix.value[0] = cos(radian);
	rotationMatrix.value[1] = -sin(radian);
	rotationMatrix.value[2] = 0;
	rotationMatrix.value[3] = 0; // translation x ?

	rotationMatrix.value[4] = sin(radian);
	rotationMatrix.value[5] = cos(radian);
	rotationMatrix.value[6] = 0;
	rotationMatrix.value[7] = 0; // translation y ?

	rotationMatrix.value[8] = 0;
	rotationMatrix.value[9] = 0;
	rotationMatrix.value[10] = 1;
	rotationMatrix.value[11] = 0; // translation z ?

	rotationMatrix.value[12] = 0;
	rotationMatrix.value[13] = 0;
	rotationMatrix.value [14] = 0;
	rotationMatrix.value[15] = 1;


	mjQuaternion *q = rotationMatrix.MatrixToQuaternion();


	// collarbone, rib, shoulder, upperarm, upperarm1, lowerarm1, lowerarm2가 모두 T 포즈 변형에 가담
	// rib: 변형에 가담하지 않음 -> rot = 0
	// 일단 rib는 빼고 변형에 가담하는 애들만 사용하여 테스트

	// Bone들이 움직이고
	// Bone.m_VertList들이 따라 움직인다

	// Left
	// shoulder부터 움직이고 child가 없을때까지 회전한다
	// collarboneR부터 움직이던걸 수정 (20. 8. 19)
	// Bone_shoulderR이 Joint_collarboneR~Joint_shoulderR이라서 Bone_shoulderR 빼고 Bone_upperArmR부터 시작 (20. 8. 22)
	mjBone *thisBone = (*m_Skeleton->m_Bones)[Bone_upperArmR];
	mjQuaternion weighted_q = mjQuaternion();

	std::cout << "Bones commited to deformation : ";
	while (true) {
		std::cout << thisBone->m_Idx << std::endl;
		mjJoint *upperJoint = thisBone->m_UpperJoint;

		// thisBone이 가지고 있는 vertex들을
		for (int i = 0; i < thisBone->m_VertList->size(); i++) {
			mjVertex *v = (*thisBone->m_VertList)[i];
			float weight = (*thisBone->m_WeightList)[i];

			// Translate하고
			v->m_Coord->x -= upperJoint->m_Coord->x;
			v->m_Coord->y -= upperJoint->m_Coord->y;
			v->m_Coord->z -= upperJoint->m_Coord->z;

			// 회전하고
			weighted_q.w = weight * q->w;
			weighted_q.x = weight * q->x;
			weighted_q.y = weight * q->y;
			weighted_q.z = weight * q->z;

			// Unit Quaternion으로 바꾸고
			// weighted_q.Normalize();

			// 새로운 값으로 업데이트
			// *v->m_Coord = weighted_q * (*v->m_Coord) * weighted_q_conjugate;
			weighted_q.Rotate(v->m_Coord);


			// Translate back
			v->m_Coord->x += upperJoint->m_Coord->x;
			v->m_Coord->y += upperJoint->m_Coord->y;
			v->m_Coord->z += upperJoint->m_Coord->z;
		}

		// 마지막으로 thisBone의 lower joint를 회전한다
		// Translate
		thisBone->m_LowerJoint->m_Coord->x -= thisBone->m_UpperJoint->m_Coord->x;
		thisBone->m_LowerJoint->m_Coord->y -= thisBone->m_UpperJoint->m_Coord->y;
		thisBone->m_LowerJoint->m_Coord->z -= thisBone->m_UpperJoint->m_Coord->z;
		// Rotate
		q->Rotate(thisBone->m_LowerJoint->m_Coord);
		// Translate Back
		thisBone->m_LowerJoint->m_Coord->x += thisBone->m_UpperJoint->m_Coord->x;
		thisBone->m_LowerJoint->m_Coord->y += thisBone->m_UpperJoint->m_Coord->y;
		thisBone->m_LowerJoint->m_Coord->z += thisBone->m_UpperJoint->m_Coord->z;


		// Leaf면 terminate
		if (thisBone->isLeaf)
			break;

		// children이 한명이라고 생각하고 가정하고,
		// thisBone을 첫번째 자식으로 업데이트

		thisBone = (*thisBone->m_Children)[0];
	}

	std::cout << std::endl;


	// Right



	// UpdateVertexBuffer
	UpdateVertBuff();
}


/////// Visual
void HumanObject::Render() {
	static mjMaterial Mtl("default");
	std::map<std::string, std::vector<float>>::iterator it0 = m_VertBuf.begin();
	// std::map<std::string, std::vector<float>>::iterator it1 = m_NormBuf.begin();
	for (; it0 != m_VertBuf.end(); it0++)
	{
		// 재질을 활성화 하여
		mjMaterial *pMtl = GetMaterial(it0->first);
		if (pMtl == NULL)
			pMtl = &Mtl;
		// pMtl->Enable();

		// 정점, 법선 배열을 지정하여 삼각형으로 렌더링 하고
		// glVertexPointer(3, GL_FLOAT, 0, &(it0->second[0]));
		// glNormalPointer(GL_FLOAT, 0, &(it1->second[0]));
		// glDrawArrays(GL_TRIANGLES, 0, (int)it0->second.size() / 3);

		// 재질을 비활성화 한다.
		// pMtl->Disable();
	}

}

void HumanObject::RenderTexture() {
	// 정점, 법선, 텍스처 버퍼를 재생성 한다.
	if (m_VertBuf.empty())
		UpdateVertBuff();

}

