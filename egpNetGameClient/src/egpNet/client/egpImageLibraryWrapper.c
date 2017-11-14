/*
	egpImageLibraryWrapper.c
	By Dan Buckstein

	As it turns out, DevIL does not play nicely with RakNet, so DevIL shall 
	have its own little space.
*/

#include "egpNet/client/egpImageLibraryWrapper.h"


#ifdef _UNICODE
#define _DEF__UNICODE
#undef _UNICODE
#endif	// _UNICODE

#ifdef UNICODE
#define _DEF_UNICODE
#undef UNICODE
#endif	// UNICODE

#include "IL/ilut.h"

#ifdef _DEF__UNICODE
#define _UNICODE
#undef _DEF__UNICODE
#endif	// _DEF__UNICODE

#ifdef _DEF_UNICODE
#define UNICODE
#undef _DEF_UNICODE
#endif	// _DEF_UNICODE


int egpImageLibraryInitialize()
{
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	return 1;
}


int egpImageLibraryLoad(char *imageFilePath, unsigned int *imageWidth_out_opt, unsigned int *imageHeight_out_opt)
{
	if (imageFilePath && *imageFilePath)
	{
		unsigned int handle = ilutGLLoadImage(imageFilePath);

		if (imageWidth_out_opt)
			*imageWidth_out_opt = handle ? ilGetInteger(IL_IMAGE_WIDTH) : 0;
		if (imageHeight_out_opt)
			*imageHeight_out_opt = handle ? ilGetInteger(IL_IMAGE_HEIGHT) : 0;

		return handle;
	}
	return -1;
}
