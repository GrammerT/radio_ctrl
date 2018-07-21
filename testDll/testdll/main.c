#include <stdio.h>
#include <windows.h>
#include "ratio_dll.h"

static int panelHandle;
typedef int   (*DLLCdeclFunction)(double ,double ,int ,int ,int ,int);
typedef int   (*DLLCdeclSetPath)(char* ,char*,char*);

int main(int argc, char *argv[])
{

    int status;
    char message[80];
    //	HINSTANCE hinstLib;
    HMODULE hinstLib;
    DLLCdeclFunction DLLFunction;

    hinstLib = LoadLibraryEx("E:\\UnizWorkspace\\QTworkspace\\ratio_dll\\release\\ratio_dll.dll",NULL,LOAD_WITH_ALTERED_SEARCH_PATH);

    if (hinstLib != NULL)//成功装载动态链接库ratio_dll.dll
    {
        int ret=0;
        DLLCdeclSetPath setpath = (DLLCdeclFunction)GetProcAddress(hinstLib, "loadConversionAndUpConversion");
        if(setpath!=NULL)
        {
            ret = setpath("E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\down_conversion.txt",
                          "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\search_down_conversion.txt",
                          "E:\\UnizWorkspace\\QTworkspace\\radio_ctrl_new\\radio_ctrl\\data\\data1\\up_conversion.txt");
        }
        //	DLLFunction = (DLLCdeclFunction)GetProcAddress(hinstLib, (LPCSTR)"connectTo");
        DLLFunction = (DLLCdeclFunction)GetProcAddress(hinstLib, "sendSetParamMsg");
        //取函数指针地址
        // If the function address is valid, call the function.
        if ( (DLLFunction != NULL))   //dll中有函数sendSetParamMsg()
        {

            //    DLLFunction ("192.168.1.10");//调用dll函数!!!
            DLLFunction(9.0,9.0,0,-10,-40,1);
        }
        // Free the DLL module
        BOOL fFreeResult = FreeLibrary(hinstLib);//卸载动态链接库ratio_dll.dll



        return 0;
    }


}
