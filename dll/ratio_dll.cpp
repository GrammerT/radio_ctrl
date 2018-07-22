#include "ratio_dll.h"
#include <iostream>
#include <regex>
#include <fstream>
#include "zmq.h"
#include <vector>

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

void * __stdcall  initRatioCtrl()
{
    m_pContext = zmq_ctx_new();
    return m_pContext;
}

void  __stdcall destroyRatioCtrl(void *)
{
    if(m_pContext!=NULL)
    {
        zmq_ctx_destroy(m_pContext);
    }
}

int  __stdcall connectTo(const char **mip)
{
    string ip = *mip;
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
        return false;
    }
    connFlag = 1;
    return true;
}

int sendMsg(char *data, int data_size)
{
    zmq_msg_t message;
    zmq_msg_t msg_rep;
    zmq_msg_init_size (&msg_rep, 100);
    if(connFlag>0)
    {
        zmq_msg_init_size (&message, data_size);
        memcpy(zmq_msg_data(&message), data, data_size);
        zmq_msg_send(&message, request, 0);
        zmq_msg_recv(&msg_rep, request, 0);
        free(data);
        return true;
    }
    free(data);
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

int  __stdcall sendPhaseLockMsg(float dds1Freq, int dds1_rf, int dds1_lf)
{
    int dds1Freq1 = (dds1Freq/ 400000000.0) * (((long long)1) << 48);
    char str[] = "{type:search,"
                        "dds1_control:%d,"
                        "dds1_rf:%d,"
                        "dds1_lf:%d,"
                        "en:0}";
    char *buf = (char*)malloc(strlen(str));
    sprintf(buf, str, dds1Freq1, dds1_rf, dds1_lf);
    return sendMsg(buf,strlen(str));
}

int  __stdcall sendStartScanMsg(float freq_start,float freq_step,
                                    float freq_stop,int freq_enable,
                                    char* mpower_tabel_file_path, int power_start, int power_step,
                                    int power_stop, int up_power_enable)
{
    string power_tabel_file_path = mpower_tabel_file_path;
    ifstream fs(power_tabel_file_path,ios::out);
    if(!fs.is_open())
    {
        cout<<power_tabel_file_path<<" can't open!!!";
        return false;
    }
    fs.seekg(0,std::ios::end);
    int length = fs.tellg();
    fs.seekg(0,ios::beg);
    char *buff = new char[length+1];
    memset(buff,0,length+1);
    fs.read(buff,length);
    fs.close();
    string str1;
    str1 = buff;
    char str[] = "{freq_start:%f,"
                        "freq_step:%f,"
                        "freq_stop:%f,"
                        "freq_enable:%d,"
                        "power_table:%s"
                        "power_start:%f,"
                        "power_step:%f,"
                        "power_stop:%f,"
                        "up_power_enable:%d}";
    char *buf = (char*)malloc(strlen(str));
    sprintf(buf, str, freq_start, freq_step, freq_stop,freq_enable,str1,power_start,power_step,power_stop,(int)up_power_enable);
    return sendMsg(buf,strlen(str));
}

int  __stdcall loadConversionAndUpConversion( char** mdownconversionpath,
                                                                              char** msearchconversionpath,
                                                                              char** mupconversionpath)
{
    char * downconversionpath = *mdownconversionpath;
    string m1 = downconversionpath;
    string searchconversionpath = *msearchconversionpath;
    string upconversionpath = *mupconversionpath;

    ifstream fs(downconversionpath,ios::out);
    if(!fs.is_open())
    {
        cout<<downconversionpath<<" can't open!!!";
        return false;
    }
    ifstream searchfs(searchconversionpath,ios::out);
    if(!searchfs.is_open())
    {
        cout<<searchconversionpath<<" can't open!!!";
        return false;
    }
    ifstream upfs(upconversionpath,ios::out);
    if(!upfs.is_open())
    {
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


int  __stdcall sendSetParamMsg(double DDS1Freq, double DDS2Freq, int DDS2Phase, int inputPower, int outputPower, int is_400)
{
    int mDDS1word = (DDS1Freq/400000000.0)*(((long long)1) << 48);
    int mDDS2word = (DDS2Freq/ 300000000.0)* (((long long)1) << 48);
    int mPhase = (DDS2Phase*16384)/2/3.1415;
    int inputIndex = findAttenuation(inputPower, mDownConversionVec);
    int outputIndex = findUPAttenuation(outputPower, mUpConversionVec);
    int lo = 0;
    if(is_400)
    {
        lo=400;
    }
    else
    {
        lo=300;
    }
    char str[] = "{type:normal,"
                        "filter_word:%d,"
                        "lo:%d,"
                        "dds1_control:%d,"
                        "dds2_control:%d,"
                        "dds1_rf:%d,"
                        "dds1_lf:%d,"
                        "dds2_rf:%d,"
                        "dds2_lf_1:%d,"
                        "dds2_lf_2:%d,"
                        "dds2_phase:%d}";
    char *buf = (char*)malloc(strlen(str));
    memset(buf,0,strlen(str));
    sprintf(buf, str,
            (int)is_400,
            lo,
            mDDS1word,
            mDDS2word,
            mDownConversionVec[inputIndex].RF,
            mDownConversionVec[inputIndex].LF,
            mUpConversionVec[outputIndex].RF,
            mUpConversionVec[outputIndex].LF_ONE,
            mUpConversionVec[outputIndex].LF_TWO,
            mPhase
            );
    return sendMsg(buf,strlen(str));
}


int __stdcall addAB(int a,int b)
{
    return a+b;
}
