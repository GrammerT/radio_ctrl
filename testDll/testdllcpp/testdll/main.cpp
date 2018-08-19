#include <stdio.h>
#include <windows.h>
#include <iostream>
#include "../../../dll/ratio_dll.h"
//#ifdef __cplusplus
//extern "C"
//{
//#endif

typedef void*  (__stdcall *DLLCdeclInit)();
typedef int  (__stdcall *DLLCdeclSetParam)(double ,double ,int ,int ,int ,int);
typedef int  (__stdcall *DLLCdeclSetPath)(const char**,const  char** ,const char** );
typedef int  (__stdcall *DLLCdecladd)(int ,int );
typedef int  (__stdcall *DLLCdeclConn)(const char ** );

//#ifdef __cplusplus
//}
//#endif
static int panelHandle;

int main(int argc, char *argv[])
{

    int status;
    char message[80];
    //	HINSTANCE hinstLib;
    HMODULE hinstLib;
//    DLLCdeclFunction DLLFunction;

    hinstLib = LoadLibraryEx(L"E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\dll\\debug\\ratio_dll.dll",NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
    DWORD dw = GetLastError();
    if (hinstLib != NULL)//成功装载动态链接库ratio_dll.dll
    {
        DLLCdeclInit init = (DLLCdeclInit)GetProcAddress(hinstLib, "initRatioCtrl");
        if(init!=NULL)
        {
            init();
        }

        int ret1=0;
        DLLCdeclConn conn = (DLLCdeclConn)GetProcAddress(hinstLib, "connectTo");
        if(conn!=NULL)
        {
            const  char *ip = "192.168.1.3";
            ret1 = conn(&ip);
        }
        int ret=0;
        DLLCdeclSetPath setpath = (DLLCdeclSetPath)GetProcAddress(hinstLib, "loadConversionAndUpConversion");
        if(setpath!=NULL)
        {

//            char down[512] = "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\down_conversion.txt";
//                 char down1[512] =   "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\search_down_conversion.txt";
//                char down2[512] =    "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\up_conversion.txt";

           const char *down = "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\down_conversion.txt";
             const    char *down1 =   "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\search_down_conversion.txt";
               const char *down2 =    "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\up_conversion.txt";
//                char *mdown = (char*)malloc(sizeof(strlen(down)+1));
//                memset(mdown,0,strlen(down)+1);
//                memcpy(mdown,down,strlen(down));
            ret = setpath(&down,&down1,&down2);
        }
        DLLCdeclSetParam DLLFunction1 = (DLLCdeclSetParam)GetProcAddress(hinstLib, "sendSetParamMsg");
        //取函数指针地址
        // If the function address is valid, call the function.
        if ( DLLFunction1 != NULL)   //dll中有函数sendSetParamMsg()
        {
            std::cout<<111111111<<std::endl;
            DLLFunction1(2,3,3,3,3,3);
            std::cout<<222222222<<std::endl;
        }

        // Free the DLL module
        BOOL fFreeResult = FreeLibrary(hinstLib);//卸载动态链接库ratio_dll.dll

        return 0;
    }


}
