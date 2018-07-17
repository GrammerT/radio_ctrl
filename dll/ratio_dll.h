#ifndef RATIO_DLL_H
#define RATIO_DLL_H
#include <string>
/**
  *design by Grammer -------------- 20180710
  * */

#ifdef  RATIO_DLL_LIBRARY
#define RATIO_DLL_LIBRARY extern "C" __declspec(dllexport)
#else
#define RATIO_DLL_LIBRARY extern "C" __declspec(dllimport)
#endif



/**
 * 先调用此函数进行初始化
 */
RATIO_DLL_LIBRARY void* initRatioCtrl();
/**
 * 连接固定ip地址
 */
RATIO_DLL_LIBRARY int connectTo(std::string ip);
/**
 * 载入配置文件，形式参考data文件夹中的数据
 */
RATIO_DLL_LIBRARY int loadConversionAndUpConversion(std::string downconversionpath,
                                                                                                    std::string searchconversionpath,
                                                                                                    std::string upconversionpath);
/**
 * 发送开始扫描指令
 */
RATIO_DLL_LIBRARY int sendStartScanMsg(float freq_start,float freq_step,
                                                                        float freq_stop,int freq_enable,
                                                                        std::string power_tabel_file_path, int power_start, int power_step,
                                                                        int power_stop, int up_power_enable);
/**
 *发送锁相捕获指令
 */
RATIO_DLL_LIBRARY int sendPhaseLockMsg(float dds1Freq,
                                                                            int dds1_rf,
                                                                            int dds1_lf);
/**
 * 发送设置参数指令
 */
RATIO_DLL_LIBRARY int sendSetParamMsg(double DDS1Freq,
                                                                             double DDS2Freq,
                                                                             int DDS2Phase,
                                                                             int inputPower,
                                                                             int outputPower,
                                                                             int is_400);
/**
 *最后调用此函数释放资源
 */
RATIO_DLL_LIBRARY void destroyRatioCtrl(void *);

#endif // RATIO_DLL_H
