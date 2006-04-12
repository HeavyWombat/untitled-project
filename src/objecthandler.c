#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <tinyxml.h>
#include <objecthandler.h>

map<string, BlenderObject*, lt_str_comp> objecthandler_objects;

void objecthandler_init()
{
  objecthandler_loadFile("data\\sign.xml");
  objecthandler_loadFile("data\\ground.xml");
  objecthandler_loadFile("data\\sack.xml");
}

int objecthandler_loadFile(char* fileName)
{
	TiXmlDocument* doc = new TiXmlDocument(fileName);
	int loadOk = doc->LoadFile();
	
	if(!loadOk)
	{
		delete doc;
		return(1);
	}

	TiXmlElement* root = NULL;
	TiXmlElement* mesh = NULL;
	TiXmlElement* vertex = NULL;
	TiXmlElement* face = NULL;
	TiXmlElement* vertexgroup = NULL;
	TiXmlElement* v = NULL;

	root = doc->RootElement();

	for(mesh = root->FirstChildElement("mesh"); mesh; mesh = mesh->NextSiblingElement("mesh"))
	{
		BlenderObject* tmpObject = new BlenderObject();
		string objectName = mesh->Attribute("name");
		objecthandler_objects[objectName] = tmpObject;

		for(vertex = mesh->FirstChildElement("vertex"); vertex; vertex = vertex->NextSiblingElement("vertex"))
		{
			Vertex* tmpVertex = new Vertex();
			tmpObject->verts.push_back(tmpVertex);

			TiXmlText* text = (TiXmlText*)vertex->FirstChild();
			sscanf(text->Value(), "%f, %f, %f, %f, %f, %f, %f, %f", &tmpVertex->coord[0], &tmpVertex->coord[1], &tmpVertex->coord[2], &tmpVertex->normal[0], &tmpVertex->normal[1], &tmpVertex->normal[2], &tmpVertex->uv[0], &tmpVertex->uv[1]);
		}
		
		for(face = mesh->FirstChildElement("face"); face; face = face->NextSiblingElement("face"))
		{
			int tmpVertIndices[4];
			
			Face* tmpFace = new Face();
			tmpObject->faces.push_back(tmpFace);

			face->QueryIntAttribute("vcount", &tmpFace->vertsCount);
			face->QueryIntAttribute("uv", &tmpFace->useUV);
			
			TiXmlText* text = (TiXmlText*)face->FirstChild();
			sscanf(text->Value(), "%d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f, %f", &tmpVertIndices[0], &tmpVertIndices[1], &tmpVertIndices[2], &tmpVertIndices[3], &tmpFace->normal[0], &tmpFace->normal[1], &tmpFace->normal[2], &tmpFace->uvCoord[0][0], &tmpFace->uvCoord[0][1], &tmpFace->uvCoord[1][0], &tmpFace->uvCoord[1][1], &tmpFace->uvCoord[2][0], &tmpFace->uvCoord[2][1], &tmpFace->uvCoord[3][0], &tmpFace->uvCoord[3][1]);
			tmpFace->verts[0] = tmpObject->verts[tmpVertIndices[0]];
			tmpFace->verts[1] = tmpObject->verts[tmpVertIndices[1]];
			tmpFace->verts[2] = tmpObject->verts[tmpVertIndices[2]];

			if(tmpFace->vertsCount == 4)
			{
				tmpFace->verts[3] = tmpObject->verts[tmpVertIndices[3]];
			}
		}		

		for(vertexgroup = mesh->FirstChildElement("vertexgroup"); vertexgroup; vertexgroup = vertexgroup->NextSiblingElement("vertexgroup"))
		{
			VertexGroup* tmpVertexGroup = new VertexGroup();
			tmpVertexGroup->name =  vertexgroup->Attribute("name");

			tmpObject->vertexGroups[tmpVertexGroup->name] = tmpVertexGroup;
			
			for(v = vertexgroup->FirstChildElement("v"); v; v = v->NextSiblingElement("v"))
			{
				int tmpVertexIndex;
				double tmpWeight;
				
				v->QueryIntAttribute("i", &tmpVertexIndex);
				v->QueryDoubleAttribute("w", &tmpWeight);
				
				tmpVertexGroup->verts.push_back(tmpObject->verts[tmpVertexIndex]);
				tmpVertexGroup->weights.push_back((float)tmpWeight);
			}	
		}

		int tmpDisplayListID = glGenLists(1);
		glNewList(tmpDisplayListID, GL_COMPILE);
		objecthandler_createOpenGLCommands(tmpObject);
		glEndList();
		tmpObject->useDisplayList = 1;
		tmpObject->displayList = tmpDisplayListID;
	}
	
		
	return(0);
}

void objecthandler_drawObject(BlenderObject* object)
{
	if(object->useTexture == 1)
	{
		glPushAttrib(GL_ENABLE_BIT);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, object->textureID);
	}
	
	if(object->useDisplayList == 1)
	{
		glCallList(object->displayList);
	}
	else
	{
		objecthandler_createOpenGLCommands(object);
	}

	if(object->useTexture == 1)
	{
		glPopAttrib();
	}
}

void objecthandler_createOpenGLCommands(BlenderObject* object)
{
	for(vector<Face*>::iterator i = object->faces.begin(); i != object->faces.end(); i++)
	{
		Face* tmpFace = (*i);
		int tmpVCount = tmpFace->vertsCount;
		int useUV = tmpFace->useUV;
		
		if(tmpVCount == 3)
		{
			glBegin(GL_TRIANGLES);
		}
		else
		{
			glBegin(GL_QUADS);
		}
		
		for(int j = 0; j < tmpVCount; j++)
		{
			Vertex* tmpVertex = tmpFace->verts[j];
			
			if(useUV == 0)
			{
				glTexCoord2f(tmpVertex->uv[0], tmpVertex->uv[1]);
			}
			else
			{
				glTexCoord2f(tmpFace->uvCoord[j][0], tmpFace->uvCoord[j][1]);
			}
			
			if(object->useVertexNormal == 0)
			{
				glNormal3f(tmpFace->normal[0], tmpFace->normal[1], tmpFace->normal[2]);
			}
			else
			{
				glNormal3f(tmpVertex->normal[0], tmpVertex->normal[1], tmpVertex->normal[2]);
			}
			
			glVertex3f(tmpVertex->coord[0], tmpVertex->coord[1], tmpVertex->coord[2]);
		}
		
		glEnd();
	}
}
