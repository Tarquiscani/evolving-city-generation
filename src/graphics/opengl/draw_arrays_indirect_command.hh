#ifndef GM_DRAW_ARRAYS_INDIRECT_COMMAND_HH
#define GM_DRAW_ARRAYS_INDIRECT_COMMAND_HH


#include <glad/glad.h>


namespace tgm
{



struct DrawArraysIndirectCommand 
{
    GLuint count;
    GLuint primCount;
    GLuint first;
    GLuint baseInstance;
};



} //namespace tgm


#endif //GM_DRAW_ARRAYS_INDIRECT_COMMAND_HH