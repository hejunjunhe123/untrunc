
#include<signal.h>

#include <mutex>
#include "mp4.h"
#include "atom.h"
#include "log.h"
#include "libavcodec/avcodec.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <jni.h>
#include <vector>
#include "com_hejunsaturday_videoinpainting_VideoUntrunc.h"
#include <android/log.h>

#include <assert.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

// using namespace android;

const int RESULT_SUCCESS = 0;
const int RESULT_FAILURE = -1;


static std::once_flag sSignalOcFlag;

#define TAG "videountrunc"  // 这个是自定义的LOG的标识
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)  // 定义LOGD类型
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)  // 定义LOGI类型
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)  // 定义LOGW类型
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)  // 定义LOGE类型
#define LOGF(...) __android_log_print(ANDROID_LOG_FATAL, TAG, __VA_ARGS__)  // 定义LOGF类型


jobject   mainActivityClz;
jobject  mainActivityObj;
class UntruncRepairCallback : public RepairCallback
{
public:
	UntruncRepairCallback(JNIEnv *env, jobject obj)
	: menv(env)
	, mobj(obj)
	 {

	 };
public :
	virtual void onRepairProcess(long cur, long total)
	{


	//	jclass ClassCJM = menv->FindClass("com/hejunsaturday/videoinpainting/VideoUntrunc");
		jmethodID MethodDisplayMessage = menv->GetMethodID((jclass)mainActivityClz, "onNative", "(Ljava/lang/String;)V");
		char mesg[100] = { 0 };
		if (cur == -1  && total == -1) {
		    sprintf(mesg, "repair failed\n");
		
		} 
		else {
		    sprintf(mesg, "repair process = %d\n", 100 * cur/total);
		}
		jstring value = menv->NewStringUTF(mesg);
		LOGI("befor call onRepairProcess = %p cur = %d, total = %d\n", MethodDisplayMessage, cur, total);
		menv->CallVoidMethod(mobj, MethodDisplayMessage, value);
		LOGI("after call onRepairProcess = %p\n", MethodDisplayMessage);
	}
private:
	JNIEnv *menv;
	jobject mobj;
};
static UntruncRepairCallback *grepaircallback = NULL;

std::string okfilepath = "";
std::string corruptfilepath = "";
std::string output_filename = "";
bool ginfo = false;
bool ganalyze = false;
bool gsimulate = false;
bool gdrifting = false;
bool gskip_zeros = false;
int gmdat_strategy = 0;
int ganalyze_track = 0;
int gmdat_begin = 0;

JavaVM *javaVM = NULL;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
	javaVM = vm;
	LOGI("JNI_OnLoad ndk test");
	return JNI_VERSION_1_4;
}
void *repair_work_entry(void *arg)
{
	JNIEnv *env;
	JavaVMAttachArgs args;
	args.version = JNI_VERSION_1_4;
	args.name = "pthread-repair";//给线程起个名字吧，这样在调试或者崩溃的时候能显示出名字，而不是thead-1,thread-2这样的名字。
	args.group = NULL;//java.lang.ThreadGroup的全局引用，作用你懂的。
	javaVM->AttachCurrentThread(&env, &args);
	LOGI("repair_work_entry env = %p arg = %p\n",env, arg);

	if (NULL != grepaircallback) {
		delete grepaircallback;
	}
	grepaircallback = new UntruncRepairCallback(env, mainActivityObj);

	Mp4 mp4;

	if(okfilepath.size()) {

		mp4.open(okfilepath);
	}


	//const char *version = env->GetStringUTFChars(value, NULL);
	//(env)->ReleaseStringUTFChars( value, version);


	if(corruptfilepath.size()) {

		bool success = mp4.repair(corruptfilepath, (Mp4::MdatStrategy)gmdat_strategy, gmdat_begin, gskip_zeros, gdrifting, grepaircallback);
	    LOGI("success = %d\n",success);
		//if the user didn't specify the strategy, try them all.
		if(!success  && gmdat_strategy == Mp4::FIRST) {
			std::vector<Mp4::MdatStrategy> strategies = { Mp4::SAME, Mp4::SEARCH, Mp4::LAST };
			for(Mp4::MdatStrategy strategy: strategies) {
				LOGI("\n\nTrying a different approach to locate mdat start");
				success = mp4.repair(corruptfilepath, strategy, gmdat_begin, gskip_zeros);
				if(success) break;
			}
		}
		if(!success) {
			LOGI("Failed recovering the file\n");
		    if (NULL != grepaircallback) {
        	    grepaircallback->onRepairProcess(-1, -1);
            }
		}

		size_t lastindex = corruptfilepath.find_last_of(".");
		std::string filetype = corruptfilepath.substr(lastindex);
		if(output_filename.size() == 0) {
		    output_filename = corruptfilepath.substr(0, lastindex) + "_fixed" + filetype;
		}
		mp4.saveVideo(output_filename, grepaircallback);
	}
	(env)->DeleteGlobalRef(mainActivityClz);
	(env)->DeleteGlobalRef(mainActivityObj);
	javaVM->DetachCurrentThread();
	return NULL;
}
extern "C" JNIEXPORT void JNICALL Java_com_hejunsaturday_videoinpainting_VideoUntrunc_untruncVideoFile
  (JNIEnv *env, jobject obj, jstring okfile, jstring corruptfile, jstring outputfilename, jboolean info, jboolean analyze, jboolean simulate, jboolean drifting, jboolean skip_zeros, jint mdat_strategy, jint analyze_track, jint mdat_begin)
{

	const char *strokfile = (env)->GetStringUTFChars(okfile, 0);
    LOGI("Java_com_hejunsaturday_videoinpainting_VideoUntrunc_untruncVideoFile okfile = %s", strokfile);
    okfilepath = strokfile;
	(env)->ReleaseStringUTFChars(okfile, strokfile);

	const char *strcorruptfile = (env)->GetStringUTFChars(corruptfile, 0);
    LOGI("Java_com_hejunsaturday_videoinpainting_VideoUntrunc_untruncVideoFile strcorruptfile = %s", strcorruptfile);
    corruptfilepath = strcorruptfile;
	(env)->ReleaseStringUTFChars(corruptfile, strcorruptfile);

	const char *stroutpufile = (env)->GetStringUTFChars(outputfilename, 0);
    LOGI("Java_com_hejunsaturday_videoinpainting_VideoUntrunc_untruncVideoFile stroutpufile = %s", stroutpufile);
    output_filename = stroutpufile;
	(env)->ReleaseStringUTFChars(corruptfile, stroutpufile);

	jclass ClassCJM = env->FindClass("com/hejunsaturday/videoinpainting/VideoUntrunc");
	jmethodID MethodDisplayMessage = env->GetMethodID(ClassCJM, "onNative", "(Ljava/lang/String;)V");
	jstring value = env->NewStringUTF("repair start");
	LOGI("befor call MethodDisplayMessage = %p\n", MethodDisplayMessage);
	env->CallVoidMethod(obj, MethodDisplayMessage, value);
	LOGI("after call MethodDisplayMessage = %p obj = %p\n", MethodDisplayMessage, obj);
	pthread_t repair_thread = 0;
	ginfo = info;
	ganalyze = analyze;
	gsimulate = simulate;
	gdrifting = drifting;
	gskip_zeros = skip_zeros;
	gmdat_strategy = mdat_strategy;
	ganalyze_track = analyze_track;
	gmdat_begin = mdat_begin;
    //jclass clz = (env)->GetObjectClass(obj);
    mainActivityClz = (env)->NewGlobalRef((jobject)ClassCJM);
    mainActivityObj = (env)->NewGlobalRef(obj);
    pthread_attr_t attr;
    size_t stack_size = 8 * 1024 * 1024; // 设置栈大小为8MB

    // 初始化属性对象
    if (pthread_attr_init(&attr) != 0) {
        perror("pthread_attr_init failed");
        return ;
    }

    // 设置栈大小
    if (pthread_attr_setstacksize(&attr, stack_size) != 0) {
        perror("pthread_attr_setstacksize failed");
        pthread_attr_destroy(&attr); // 清理属性对象
        return ;
    }


    int r = pthread_create(&repair_thread, &attr, repair_work_entry, NULL);
    pthread_detach(repair_thread);


}


#ifdef __cplusplus
}
#endif

