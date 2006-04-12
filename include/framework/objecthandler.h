#ifndef __OBJECTLOADER_H
#define __OBJECTLOADER_H

using namespace std;

#include<string>
#include<vector>
#include<map>

struct lt_str_comp
{
	bool operator()(string s1, string s2) const
	{
		return(s1 < s2);
	}
};

class Vertex
{
	public:
		Vertex()
		{
			memset(coord, 0, 3 * sizeof(float));
			memset(normal, 0, 3 * sizeof(float));
			memset(uv, 0, 2 * sizeof(float));
		};
		
		float coord[3];
		float normal[3];
		float uv[2];
};

class Face
{
	public:
		Face()
		{
			vertsCount = 0;
			useUV = 0;
			memset(verts, 0, 4 * sizeof(Vertex*));
			memset(normal, 0, 3 * sizeof(float));
			memset(uvCoord, 0, 4 * 2 * sizeof(float));
		};
		
		int vertsCount;
		int useUV;
		Vertex* verts[4];
		float normal[3];
		float uvCoord[4][2];
};

class VertexGroup
{
	public:
		string name;
		vector<Vertex*> verts;
		vector<float> weights;
};

class BlenderObject
{
	public:
		BlenderObject()
		{
			displayList = 0;
			useDisplayList = 0;
			useVertexNormal = 0;
			useTexture = 0;
			textureID = 0;
		};
		
		int displayList;
		int useDisplayList;
		int useVertexNormal;
		int useTexture;
		int textureID;
		vector<Vertex*> verts;
		vector<Face*> faces;
		map<string, VertexGroup*, lt_str_comp> vertexGroups;
};

void objecthandler_init();
int objecthandler_loadFile(char* fileName);
void objecthandler_drawObject(BlenderObject* object);
void objecthandler_createOpenGLCommands(BlenderObject* object);

extern map<string, BlenderObject*, lt_str_comp> objecthandler_objects;

#endif
