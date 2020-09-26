#define WIDTH 640
#define HEIGHT 480
#define EXIT_ON_ERROR
#define COMFORT_CAM
//#define _RELEASE
#define LOGING_IN_CONSOLE




#define BUFFER_OFFSET(i) ((char*)NULL + (i))
#define ASSERT(x) if (!(x)) __debugbreak();


#ifndef _RELEASE
#  define glcall(x)                                                         \
 	   GLClearError();                                                      \
	   x;                                                                   \
	   ASSERT(GLLogCall(#x, __FILE__, __LINE__));                           
#else
#  define glcall(x) x;
#endif


#define glretcall(x)                                                      \
	GLClearError();                                                       \
	ASSERT(GLLogCall(#x, __FILE__, __LINE__));                            \
	return x;


#define glifcall(x)														  \
	GLClearError();                                                       \
	if ((x)) {


#define glendifcall(x) } ASSERT(GLLogCall(#x, __FILE__, __LINE__));


#ifdef LOGING_IN_CONSOLE
#  define CONSOLE_LOG(x) std::cout << x;
#  define CONSOLE_FLUSH_LOG(x) std::cout << x << std::flush;
#else
#  define CONSOLE_LOG(x)
#endif

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);