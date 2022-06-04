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
#	define glcall(x) GLClearError(); \
					 x; \
					 ASSERT(GLLogCall(#x, __FILE__, __LINE__));

#	define MAIN int main(int argc, char** argv)
#else
#	define glcall(x) x;
#	define MAIN int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#endif



#ifdef LOGING_IN_CONSOLE
#	define CONSOLE_LOG(x) std::cout << x;
#else
#	define CONSOLE_LOG(x) ;
#endif



void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);