#include "ratio_dll.h"
#include <iostream>
#include <regex>
#include <fstream>
#include "zmq.h"
#include <vector>
#include <strstream>
#include <Windows.h>
//#include <QByteArray>
//#include <QDebug>

using namespace std;
struct Conversion
{
    float value;
    int RF;
    int LF;
};
struct UPConversion
{
    float value;
    int RF;
    int LF_ONE;
    int LF_TWO;
};

static void *m_pContext=NULL;
static void *request = NULL;
static int connFlag = 0;
static vector<Conversion> mDownConversionVec;
static vector<Conversion> mSearchDownConversionVec;
static vector<UPConversion> mUpConversionVec;
static string filename = "msg.txt";
//static char data[10240];

void recordMsg(string str)
{
    fstream fs;
    fs.open(filename,ios::app);
    if(fs.is_open())
    {
        fs<<str<<"\n";
        fs.close();
    }
}

void * __stdcall  initRatioCtrl()
{
    recordMsg("initRatioCtrl()");
    m_pContext = zmq_ctx_new();
    return m_pContext;
}

void  __stdcall destroyRatioCtrl(void *)
{
    if(m_pContext!=NULL)
    {
        recordMsg("destroy before");
        zmq_ctx_shutdown(m_pContext);
        recordMsg("destroy after");
    }
}

int  __stdcall connectTo(const char **mip)
{
    string ip = *mip;
    recordMsg("connectTo+"+ip);
    if(ip.size()<=0)
        return false;
    const std::regex pattern("(\\d{1,3}).(\\d{1,3}).(\\d{1,3}).(\\d{1,3})");
    std:: match_results<std::string::const_iterator> result;
    bool valid = std::regex_match(ip, result, pattern);
    if(!valid)
        return false;
    int rc;
    request = zmq_socket(m_pContext, ZMQ_REQ);
    string ip1 = "tcp://" + ip + ":5555";
    rc = zmq_connect(request, ip1.c_str());
    if(rc < 0)
    {
        connFlag = 0;
        recordMsg("connectTo--flag--0");
        return false;
    }
    recordMsg("connectTo--flag--1");
    connFlag = 1;
    return true;
}

void myFree(void *data, void *hint)
{
    free(data);
}

int sendMsg(char *data, int data_size)
{
    recordMsg(data);
    std::string result3;
    std::strstream ss3;
    ss3 <<  data_size;
    ss3 >> result3;
    recordMsg(result3);
    zmq_msg_t message;
    zmq_msg_t msg_rep;
    if(connFlag>0)
    {
        int ret1 = zmq_msg_init_size (&msg_rep, data_size);
        ret1 = zmq_msg_init_size (&message, data_size);
        memcpy(zmq_msg_data(&message), data, data_size);
//        zmq_msg_init_data(&message,data,data_size,myFree,NULL);
        recordMsg("before zmq_msg_send");
        zmq_msg_send(&message, request, 0);
        recordMsg("after zmq_msg_send");
        zmq_msg_recv(&msg_rep, request, 0);
        recordMsg("after zmq_msg_recv");
//        HeapFree(GetProcessHeap(),0,(void*)data);
        free(data);
        recordMsg("after free true");
        ret1 = zmq_msg_close(&msg_rep);
        ret1 = zmq_msg_close(&message);
        return true;
    }

    recordMsg("after free false");
    return false;
}

int findAttenuation(double powerValue, const std::vector<Conversion> &conversionVec)
{
    int index = 0;
    double minDistance = 0.0;
    for(int i = 0; i < conversionVec.size(); ++i)
    {
        double distance = fabs(powerValue - conversionVec[i].value);
        if(0 == i)
        {
            minDistance = distance;
            index = i;
        }
        else
        {
            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }
    }
    return index;
}

int  findUPAttenuation(double powerValue, const std::vector<UPConversion> &conversionVec)
{
    int index = 0;
    double minDistance = 0.0;
    for(int i = 0; i < conversionVec.size(); ++i)
    {
        double distance = fabs(powerValue - conversionVec[i].value);
        if(0 == i)
        {
            minDistance = distance;
            index = i;
        }
        else
        {
            if (distance < minDistance)
            {
                minDistance = distance;
                index = i;
            }
        }
    }
    return index;
}

int  __stdcall sendPhaseLockMsg(double dds1Freq, double OutputPwd)
{
    recordMsg("sendPhaseLockMsg:");
    int index = findAttenuation(OutputPwd,mSearchDownConversionVec);
    long long dds1Freq1 = (dds1Freq/ 400000000.0) * (((long long)1) << 48);
    std::string result3;
    std::strstream ss3;
    ss3 <<  dds1Freq1;
    ss3 >> result3;
    char str[] = "{\"dds1_control\":\"%s\","
                        "\"dds1_rf\":%d,"
                        "\"dds1_lf\":%d,"
                        "\"type\":\"search\","
                        "\"en\":0}";
    char *buf = (char*)malloc(1024);
//    char *buf = (char*)HeapAlloc(GetProcessHeap(),0,1024);
    memset(buf,0,1024);
    int ret = sprintf(buf, str, result3.c_str(), mSearchDownConversionVec[index].RF, mSearchDownConversionVec[index].LF);
    recordMsg(buf);
    return sendMsg(buf,ret);
}

int  __stdcall sendStartScanMsg(float freq_start,float freq_step,
                                float freq_stop,int freq_enable,
                                const char** mpower_tabel_file_path, float power_start, float power_step,
                                float power_stop, int up_power_enable)
{
    recordMsg("sendStartScanMsg:");
    string power_tabel_file_path = *mpower_tabel_file_path;
    ifstream fs(power_tabel_file_path,ios::out);
    if(!fs.is_open())
    {
        recordMsg(" can't open!!!");
        return false;
    }
    fs.seekg(0,std::ios::end);
    int length = fs.tellg();
    fs.seekg(0,ios::beg);
    char *buff = new char[length];
    memset(buff,0,length);
    fs.read(buff,length);
    fs.close();
    string str1;
    str1 = buff;
    delete buff;
//    recordMsg(str1);

    std::string freq_start_s;
    std::strstream ss3;
    ss3 <<  freq_start;
    ss3 >> freq_start_s;
    ss3.clear();

    std::string freq_step_s;
    ss3 <<  freq_step;
    ss3 >> freq_step_s;
    ss3.clear();

    std::string freq_stop_s;
    ss3 <<  freq_stop;
    ss3 >> freq_stop_s;
    ss3.clear();

    std::string freq_enable_s;
    ss3 <<  freq_enable;
    ss3 >> freq_enable_s;
    ss3.clear();

    std::string power_start_s;
    ss3 <<  power_start;
    ss3 >> power_start_s;
    ss3.clear();

    std::string power_step_s;
    ss3 <<  power_step;
    ss3 >> power_step_s;
    ss3.clear();

    std::string power_stop_s;
    ss3 <<  power_stop;
    ss3 >> power_stop_s;
    ss3.clear();

    std::string up_power_enable_s;
    ss3 <<  up_power_enable;
    ss3 >> up_power_enable_s;
    ss3.clear();



    char str[] = "{\"freq_start\":\"%s\","
                        "\"freq_step\":\"%s\","
                        "\"freq_stop\":\"%s\","
                        "\"freq_enable\":\"%s\","
                        "\"power_table\":\"%s\","
                        "\"power_start\":\"%s\","
                        "\"power_step\":\"%s\","
                        "\"power_stop\":\"%s\","
                        "\"up_power_enable\":\"%s\"}";
    char *buf = (char*)malloc(10240);
//    char *buf = (char*)HeapAlloc(GetProcessHeap(),0,10240);
    memset(buf,0,10240);
    int ret =sprintf(buf, str, freq_start_s.c_str(), freq_step_s.c_str(), freq_stop_s.c_str(),freq_enable_s.c_str(),str1.c_str(),
                    power_start_s.c_str(),power_step_s.c_str(),power_stop_s.c_str(),up_power_enable_s.c_str());

    recordMsg(buf);
    return sendMsg(buf,ret );
}

int  __stdcall loadConversionAndUpConversion( const char** mdownconversionpath,
                                                                             const char** msearchconversionpath,
                                                                             const char** mupconversionpath)
{
    recordMsg("loadConversionAndUpConversion:");
    const char * downconversionpath = *mdownconversionpath;
    string m1 = downconversionpath;
    string searchconversionpath = *msearchconversionpath;
    string upconversionpath = *mupconversionpath;

    ifstream fs(downconversionpath,ios::out);
    if(!fs.is_open())
    {
        recordMsg(m1+" error!!!");
        cout<<downconversionpath<<" can't open!!!";
        return false;
    }
    ifstream searchfs(searchconversionpath,ios::out);
    if(!searchfs.is_open())
    {
        recordMsg(searchconversionpath+" error!!!");
        cout<<searchconversionpath<<" can't open!!!";
        return false;
    }
    ifstream upfs(upconversionpath,ios::out);
    if(!upfs.is_open())
    {
        recordMsg(upconversionpath+" error!!!");
        cout<<upconversionpath<<" can't open!!!";
        return false;
    }
    float x;
    int y,z;
    while(fs>>x>>y>>z)
    {
        Conversion conversion;
        conversion.value = x;
        conversion.RF = y;
        conversion.LF = z;
        mDownConversionVec.push_back(conversion);
    }

    while(searchfs>>x>>y>>z)
    {
        Conversion conversion;
        conversion.value = x;
        conversion.RF = y;
        conversion.LF = z;
        mSearchDownConversionVec.push_back(conversion);
    }
    float x1,x2;
    int y1,z1;
    while(upfs>>x1>>x2>>y1>>z1)
    {
        UPConversion upconversion;
        upconversion.value        =x1;
        upconversion.RF           =x2;
        upconversion.LF_ONE   =y1;
        upconversion.LF_TWO   =z1;
        mUpConversionVec.push_back(upconversion);
    }
     fs.close();
     searchfs.close();
     upfs.close();

    return 1;
}


int  __stdcall sendSetParamMsg(double DDS1Freq, double DDS2Freq, int DDS2Phase,
                               double inputPower, double outputPower, int is_400)
{
    recordMsg("sendSetParamMsg:");

    long long mDDS1word = (DDS1Freq/100000000.0)*(((long long)1) << 48);
    long long mDDS2word = (DDS2Freq/ 100000000.0)* (((long long)1) << 48);
    int mPhase = (DDS2Phase*16384)/2/3.1415;
    int inputIndex = findAttenuation(inputPower, mDownConversionVec);
    int outputIndex = findUPAttenuation(outputPower, mUpConversionVec);
    int lo = 0;
    string b_400 = "false";
    if(is_400)
    {
        b_400 = "true";
        lo=400;
    }
    else
    {
        lo=300;
    }
    char str[] = "{ \"type\":\"normal\","
                        "\"filter_word\":%s,"
                        "\"lo\":%d,"
                        "\"dds1_control\":\"%s\","
                        "\"dds2_control\":\"%s\","
                        "\"dds1_rf\":%d,"
                        "\"dds1_lf\":%d,"
                        "\"dds2_rf\":%d,"
                        "\"dds2_lf_1\":%d,"
                        "\"dds2_lf_2\":%d,"
                        "\"dds2_phase\":\"%s\"}";

    std::string result1;
    std::strstream ss1;

    std::string result2;
    std::strstream ss2;

    ss1 <<  mDDS1word;
    ss1 >> result1;
    ss2 <<  mDDS2word;
    ss2 >> result2;

    std::string result3;
    std::strstream ss3;
    ss3 <<  mPhase;
    ss3 >> result3;
    recordMsg("before malloc");
    char *buf = (char*)malloc(1024);
//    char *buf = (char*)HeapAlloc(GetProcessHeap(),0,1024);
    recordMsg("after malloc");
    memset(buf,0,1024);
    int ret = sprintf(buf, str,
            b_400.c_str(),
            lo,
            result1.c_str(),
            result2.c_str(),
            mDownConversionVec[inputIndex].RF,
            mDownConversionVec[inputIndex].LF,
            mUpConversionVec[outputIndex].RF,
            mUpConversionVec[outputIndex].LF_ONE,
            mUpConversionVec[outputIndex].LF_TWO,
            result3.c_str()
            );
    recordMsg(buf);
    return sendMsg(buf,ret);
}

int __stdcall sendSetJumpFreqParamMsg(double dds_f0,
                                      double dds_f1,
                                      double dds_t0,
                                      double dds_t1,
                                      int enable)
{
    recordMsg("sendSetJumpFreqParamMsg");

    long long mDDS1word = (dds_f0/100000000.0)*(((long long)1) << 48);
    long long mDDS2word = (dds_f1/100000000.0)* (((long long)1) << 48);


    char str[] = "{ \"dds_f0\":\"%s\","
                 "\"dds_f1\":\"%s\","
                 "\"dds_t0\":\"%s\","
                 "\"dds_t1\":\"%s\","
                 "\"hop_enable\":\"%s\"}";

    std::string result1;
    std::strstream ss1;

    std::string result2;
    std::strstream ss2;

    ss1 <<  mDDS1word;
    ss1 >> result1;
    ss2 <<  mDDS2word;
    ss2 >> result2;

    std::string result3;
    std::strstream ss3;
    ss3 <<  dds_t0;
    ss3 >> result3;

    std::string result4;
    std::strstream ss4;
    ss4 <<  dds_t1;
    ss4 >> result4;

    std::string result5;
    std::strstream ss5;
    ss5 <<  enable;
    ss5 >> result5;

    recordMsg("before malloc");
    char *buf = (char*)malloc(1024);
    recordMsg("after malloc");
    memset(buf,0,1024);
    int ret = sprintf(buf, str,
            result1.c_str(),result2.c_str(),
                      result3.c_str(),
                      result4.c_str(),
                      result5.c_str());
    recordMsg("after sprintf...");
    recordMsg(buf);
    return sendMsg(buf,ret);
}


