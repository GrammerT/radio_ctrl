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
RATIO_DLL_LIBRARY int __stdcall  connectTo(char **mip);
///**
// * 载入配置文件，形式参考data文件夹中的数据
// */
RATIO_DLL_LIBRARY int __stdcall  loadConversionAndUpConversion( char** mdownconversionpath,
                                                                                                    char** msearchconversionpath,
                                                                                                    char** mupconversionpath);
///**
// * 发送开始扫描指令
// */
RATIO_DLL_LIBRARY int __stdcall sendStartScanMsg(float freq_start,float freq_step,
                                                                        float freq_stop,int freq_enable,
                                                                        char* mpower_tabel_file_path, int power_start, int power_step,
                                                                        int power_stop, int up_power_enable);
///**
// *发送锁相捕获指令
// */
RATIO_DLL_LIBRARY int __stdcall sendPhaseLockMsg(float dds1Freq,
                                                                            int dds1_rf,
                                                                            int dds1_lf);
///**
// * 发送设置参数指令
// */
RATIO_DLL_LIBRARY int __stdcall sendSetParamMsg(double DDS1Freq,
                                                                             double DDS2Freq,
                                                                             int DDS2Phase,
                                                                             int inputPower,
                                                                             int outputPower,
                                                                             int is_400);
///**
// *最后调用此函数释放资源
// */
RATIO_DLL_LIBRARY void __stdcall destroyRatioCtrl(void *);

#endif // RATIO_DLL_H
