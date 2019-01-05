#ifndef RATIO_DLL_H
#define RATIO_DLL_H
/**
  *design by Grammer -------------- 20180710
  * */

#ifdef  RATIO_DLL_LIBRARY
#define RATIO_DLL_LIBRARY  extern "C" __declspec(dllexport)
#else
#define RATIO_DLL_LIBRARY extern "C" __declspec(dllimport)
#endif



/**
 * 先调用此函数进行初始化
 */
RATIO_DLL_LIBRARY void* __stdcall initRatioCtrl();
/**
 * 连接固定ip地址
 */
RATIO_DLL_LIBRARY int __stdcall  connectTo(const char **mip);
///**
// * 载入配置文件，形式参考data文件夹中的数据
// */
RATIO_DLL_LIBRARY int __stdcall  loadConversionAndUpConversion(const char** mdownconversionpath,
                                                                                                    const char** msearchconversionpath,
                                                                                                    const char** mupconversionpath);
///**
// * 发送开始扫描指令
// */
RATIO_DLL_LIBRARY int __stdcall sendStartScanMsg(float freq_start,float freq_step,
                                                                        float freq_stop,int freq_enable,
                                                                        const char** mpower_tabel_file_path, float power_start, float power_step,
                                                                        float power_stop, int up_power_enable);
///**
// *发送锁相捕获指令
// */
RATIO_DLL_LIBRARY int __stdcall sendPhaseLockMsg(double dds1Freq,double OutputPwd);
///**
// * 发送设置参数指令
// */
RATIO_DLL_LIBRARY int __stdcall sendSetParamMsg(double DDS1Freq,
                                             double DDS2Freq,
                                             int DDS2Phase,
                                             double inputPower,
                                             double outputPower,
                                             int is_400);
//! 跳频function
RATIO_DLL_LIBRARY int __stdcall sendSetJumpFreqParamMsg(double dds_f0,
                                                     double dds_f1,
                                                     unsigned int  dds_t0,
                                                     unsigned int  dds_t1,
                                                     int enable);
///**
// *最后调用此函数释放资源
// */
RATIO_DLL_LIBRARY void __stdcall destroyRatioCtrl(void *);

#endif // RATIO_DLL_H
