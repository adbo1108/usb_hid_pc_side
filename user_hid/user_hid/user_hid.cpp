// user_hid.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include<Windows.h>
#include<setupapi.h>
#include<hidsdi.h>
#include<initguid.h>
#include <time.h>
#include <fstream>
#include <stdio.h>

using namespace std;


BOOL find_device(uint32_t PID, uint32_t VID);
HANDLE userHidFileHandle;

#define REPORT_ID 0x02
fstream myfile;

uint8_t ROTATION_0_CMD[17] = { 0x11,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00 };
uint8_t ROTATION_90_CMD[17] = { 0x11,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01 };


BOOL RotationTask(uint8_t* packet)
{
	BOOL Detect = false;
	if (!memcmp(packet, ROTATION_0_CMD, 17))
	{
		//rotation 0
		Detect = TRUE;
		cout << "Rotation 0 \n\r";
		system("RotationApp.exe 0");

	}
	if (!memcmp(packet, ROTATION_90_CMD, 17))
	{
		//rotation 90
		Detect = TRUE;
		cout << "Rotation 90 \n\r";
		system("RotationApp.exe 90");
		//Sleep(5000);
		//system("RotationApp.exe 0");

	}
	return Detect;

}
void Create_Log_File()
{
	myfile.open("C:\\EMR_Log.txt", ios::out);
	if (!myfile)
	{
		cout << "File Fail \n\r";
	}
	else
	{
		//myfile << Get_Date_() <<"\n\r";

	}
	
}

void Close_Log_File()
{
	myfile.close();
}

char buffer[128];
char* Get_Date_()
{
	
	SYSTEMTIME st = { 0 };
	GetLocalTime(&st);

	sprintf_s(buffer, "%d-%02d-%02d %02d:%02d:%02d:%02d",
		st.wYear,
		st.wMonth,
		st.wDay,
		st.wHour,
		st.wMinute,
		st.wSecond,
		st.wMilliseconds);
	//printf("%s\n\r",buffer);
	return buffer;
}


int main()
{
	BOOL isReadFile = false;
	BOOL isWriteFile = false;
	uint8_t readbuf[128]; // Buffer 請填大一點!!!
	uint8_t writebuf[128];
	DWORD numberOfBytesRead;
	DWORD numberOfBytesWrite;
	writebuf[0] = 0; 
	writebuf[1] = 1;
	cout << "[注意] 請以系統管理身分執行，否則旋轉失效!!!! \n\n ";
	cout << "***************User Hid Device Software!*******************\n\n\n\n\n"; 
	cout << "***************Finding USB Device*******************\n";

	//system("RotationApp.exe 90");
	//Sleep(5000);
	//system("RotationApp.exe 0");

	printf("%s\n\r", Get_Date_());

	while (!find_device(22352, 1155)) {
		Sleep(1000);
		printf(".");
	}
	printf("\n\r");
	Create_Log_File();
	

		while (true)
		{
#if 0	   // GetFeature via Endpoint0 用C++ API還無法用
			Sleep(1000);
			if (!HidD_GetFeature(userHidFileHandle, readbuf, 8)) {
				//cout << "Feature report failed" << endl;
			}
			else
			{
				cout << "Feature report successful" << endl;
				for (int i = 0; i < 8; i++)
					cout << hex << (int)readbuf[i];
				cout << endl;
			}
			
#endif

#if 1			
		//	Sleep(10);
			// 注意喔!!! STM32我們是發送64個Byte，可是這邊接收的長度要寫65，因為會在前面自動加一個報告ID (Report ID)
			// 或是乾脆就寫大一點，像這邊寫128

			isReadFile = ReadFile(userHidFileHandle, readbuf, 128, &numberOfBytesRead, NULL);
		
			if (isReadFile)
			{
				//printf("read data byte: %d\n\r", numberOfBytesRead);
			//	printf("%s\n\r",Get_Date_());
				if (RotationTask(readbuf)) {
					continue;

				}
				myfile << Get_Date_() << "\n\r";
				cout << " X= " << dec <<(uint32_t)(readbuf[4] + (readbuf[5] << 8)) << ",  Y= " <<dec<< (uint32_t)(readbuf[6] + (readbuf[7] << 8) )   ;
				cout << ", P=" << dec << (uint32_t)(readbuf[8] + ((readbuf[9]&0xff) << 8));
				for (int i = 0; i < numberOfBytesRead; i++)
				{
					//cout << "  0x" << hex << (uint32_t)readbuf[i] << ",";
					
					myfile<< "  0x" << hex << (uint32_t)readbuf[i] << ",";
				}
				printf("\n\r");
				myfile << "\n\r";
			}
			else
			{
			//	printf("no data read \n\r");
			}
#endif
#if 0
			Sleep(1000);
			writebuf[0]= REPORT_ID;

			//填值
			for (int ix = 1; ix < 65; ix++)
				writebuf[ix] = ix;
			
			isWriteFile = WriteFile( userHidFileHandle, writebuf, 65, &numberOfBytesWrite, NULL);
			if (isWriteFile)
			{
				cout << "write data byte:" << numberOfBytesWrite << endl;
				for (int i = 0; i < numberOfBytesWrite; i++)
				{
					printf("writebuf[%d]",i);
					cout << "  0x" << hex << (uint32_t)writebuf[i] << endl;
				}
			}
#endif
		}

		Close_Log_File();
	
}

void show_guid(LPGUID   HidGuid)
{
	cout << hex << HidGuid->Data1 << "-";
	cout << hex << HidGuid->Data2 << "-";
	cout << hex << HidGuid->Data3 << "-";
	for (int i = 0; i < 8; i++)
	{
		cout << hex << (uint32_t)(HidGuid->Data4[i]);
	}
	cout << endl;
}

BOOL find_device(uint32_t PID, uint32_t VID)
{
	BOOL isFind = false;
	GUID hid_guid;
	HDEVINFO device_info;
	BOOL isGetDeviceInterfaces = false;
	BOOL isGetDeviceInterfaceDetail = false;
	BOOL isGetAttributes = false;
	uint32_t deviceIndex = 0;
	SP_DEVICE_INTERFACE_DATA device_interface_data;
	DWORD requiredSize = 0;
	PSP_DEVICE_INTERFACE_DETAIL_DATA p_device_interface_detail_data;
	HANDLE fileHandle;
	HIDD_ATTRIBUTES hid_attribute;

	HidD_GetHidGuid(&hid_guid);
	//show_guid(&hid_guid);
	
	device_info = SetupDiGetClassDevs(&hid_guid, NULL, NULL, 
		DIGCF_INTERFACEDEVICE | DIGCF_PRESENT);
	do
	{
		device_interface_data.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	    isGetDeviceInterfaces = SetupDiEnumDeviceInterfaces( device_info, NULL, &hid_guid,
		deviceIndex, &device_interface_data);

		deviceIndex++;
		if (isGetDeviceInterfaces)
		{
		//	cout << "GetDeviceInterfaces success" << endl;

			/*获取 RequiredSize*/
			SetupDiGetDeviceInterfaceDetail(
				device_info,
				&device_interface_data,
				NULL,
				0,
				&requiredSize,
				NULL);
		//	cout << "RequiredSize:" << requiredSize  << endl;

			p_device_interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(requiredSize);
			p_device_interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
			
			isGetDeviceInterfaceDetail = SetupDiGetDeviceInterfaceDetail(
				device_info,
				&device_interface_data,
				p_device_interface_detail_data,
				requiredSize,
				NULL,
				NULL);

			if (isGetDeviceInterfaceDetail)
			{
				//cout << "GetDeviceInterfaceDetail success" << endl;
				fileHandle = CreateFile(
					p_device_interface_detail_data->DevicePath,
					GENERIC_READ | GENERIC_WRITE,
					FILE_SHARE_READ | FILE_SHARE_WRITE,
					NULL,
					OPEN_EXISTING,
					FILE_ATTRIBUTE_NORMAL,
					NULL);

				if (fileHandle == INVALID_HANDLE_VALUE)
				{
				//	cout << "CreateFile ERROR" << endl;
				}
				else
				{
					//cout << "CreateFile success" << endl;
					isGetAttributes = HidD_GetAttributes( fileHandle, &hid_attribute );
					if (isGetAttributes)
					{

						if (hid_attribute.ProductID == PID && hid_attribute.VendorID == VID)
						{
							userHidFileHandle = fileHandle;
							cout << "\nPID:0x" << hex << hid_attribute.ProductID << endl;
							cout << "VID:0x" << hex << hid_attribute.VendorID << endl;
							printf("CreateFile success & find right device !!!!!!!!!!!!!!!\n\r");
							return true;
						}
					}
				}
			}

		}

	} while (isGetDeviceInterfaces);

	return isFind;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
