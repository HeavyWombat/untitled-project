#ifndef __FAKELIB_H
#define __FAKELIB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GP32
#include <GL/gl.h>
#endif

typedef void* fake_lib_t;

typedef struct{
	void		*data;
        long int	size;
} fake_data_t;

typedef struct {
	unsigned char	*pData;
	long int	iSize;
	long int	iPos;
} fake_memfile_t;

typedef struct{
	unsigned short		data[ 3*256 ];	/* always 16bit RGB tripples */
#ifdef GP32        
        unsigned short          gp32data[ 256 ];
#endif        
	unsigned short		num;		/* number of entries */
} fake_image_pal_t;

typedef struct{
	unsigned short int	width;
        unsigned short int	height;
        unsigned char		*data;
	unsigned char		mode;
	fake_image_pal_t	*palette;
} fake_image_t;

typedef struct{
        fake_lib_t                    *pfl;
#ifndef GP32
        GLenum  format;
        GLsizei stride;
        GLenum  mode;
        GLsizei count;
        GLvoid  *pointer;
#endif
} fake_oia_t;


typedef struct{
	fake_lib_t	*pfl;
	int		something;
} fake_ipo_t;


/* will be moved to internal !!!*/
typedef struct{
	unsigned short	id;
	unsigned char	*name;
	unsigned long	numVertices;
	float		*vertices;
	float		*normals;
	float		*texcoords;
	unsigned long	numFaces;
	unsigned long	*faces;
	unsigned int	texture;	
}fake_obj_mesh_t;

typedef struct _fake_obj_link_m{
	unsigned short		id;
	unsigned char		*name;
	float			matrix[ 16 ];
	unsigned short		numLinks;
	struct _fake_obj_link_m	**links;
	unsigned short		numMeshes;
	fake_obj_mesh_t		**meshes;
}fake_obj_link_t;
/* ----------------------- */


#define FAKE_IMAGE_MODE_PAL	1
#define FAKE_IMAGE_MODE_RGB	2
#define FAKE_IMAGE_MODE_RGBA	3
#define FAKE_IMAGE_MODE_GRAY	4
#define FAKE_IMAGE_MODE_GRAYA	5

fake_lib_t *fake_new( );
int fake_delete( fake_lib_t *fl );

int fake_dar_add( fake_lib_t *fkl, fake_data_t *fd );
int fake_dar_add_file( fake_lib_t *fkl, unsigned char *filename );
int fake_dar_add_dir( fake_lib_t *fkl, unsigned char *dirname );

#ifdef DEBUG
int fake_dar_dump( fake_lib_t *fkl, int iNum, unsigned char *filename );
#endif

fake_memfile_t *fake_memfile_new( fake_lib_t *fkl );
long fake_memfile_delete( fake_memfile_t **pfmf  );
long fake_memfile_load( fake_lib_t *fkl, fake_memfile_t *fm, char *name );
long fake_memfile_read( fake_lib_t *fld, fake_memfile_t *fm, void *buf, long num );

int fake_image_delete( fake_lib_t *fl, fake_image_t **fi );
fake_image_t* fake_imageFromPNG( fake_lib_t *fl, char *name );
long fake_image_toRGBA8888( fake_lib_t *fl, fake_image_t *fi );

fake_oia_t *fake_oia_load( fake_lib_t *pfl, char *name );
long fake_oia_delete( fake_lib_t *fl, fake_oia_t **fo );

long    fake_lastError( fake_lib_t *fkl );
char    *fake_getTrace( fake_lib_t *fkl );

fake_ipo_t *fake_ipo_new( fake_lib_t *fkl );
long fake_ipo_delete( fake_lib_t *fl, fake_ipo_t **pIpo );

#ifdef __cplusplus
}
#endif

#endif
