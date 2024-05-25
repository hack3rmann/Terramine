#define WIDTH 640
#define HEIGHT 480
#define EXIT_ON_ERROR
#define COMFORT_CAM
//#define _RELEASE
#define LOGING_IN_CONSOLE
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define ASSERT(x) if (!(x)) __debugbreak();



#ifndef _RELEASE
    #define glcall(call) do { \
        GLClearError(); \
        call; \
        ASSERT(GLLogCall(#call, __FILE__, __LINE__)); \
    } while (false)
#else
    #define glcall(call) call
#endif



#ifdef LOGING_IN_CONSOLE
    #define CONSOLE_LOG(x) do { std::cout << x; } while (false)
#else
    #define CONSOLE_LOG(x) do {} while (false)
#endif



void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);