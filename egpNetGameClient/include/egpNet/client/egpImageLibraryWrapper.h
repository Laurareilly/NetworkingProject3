/*
	egpImageLibraryWrapper.h
	By Dan Buckstein

	As it turns out, DevIL does not play nicely with RakNet, so DevIL shall 
	have its own little space.
*/

#ifndef __EGP_IMAGELIBRARYWRAPPER_H_
#define __EGP_IMAGELIBRARYWRAPPER_H_


#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus


// start up image library
int egpImageLibraryInitialize();

// generate an OpenGL formatted texture
// returns positive integer if success, 0 if failed, -1 if invalid params
int egpImageLibraryLoad(char *imageFilePath, unsigned int *imageWidth_out_opt, unsigned int *imageHeight_out_opt);


#ifdef __cplusplus
}
#endif	// __cplusplus


#endif	// !__EGP_IMAGELIBRARYWRAPPER_H_