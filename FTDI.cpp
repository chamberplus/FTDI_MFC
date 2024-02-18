#include "stdafx.h"
#include <windows.h>
#include <exception>
#include "FTD2XX.H"


int _tmain(int argc, _TCHAR* argv[])
{
	FT_STATUS ftStatus;
	FT_HANDLE ftHandle;
	
	DWORD TotalRead=0;
	char Buf[64];

	ftStatus = FT_ListDevices(0, Buf, FT_LIST_BY_INDEX | FT_OPEN_BY_SERIAL_NUMBER);

	if (ftStatus!=FT_OK)
	{
		printf("Couldn't get FTDI device name");
		return 0;
	}

	ftHandle = FT_W32_CreateFile(Buf,
		GENERIC_READ|GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED | 	FT_OPEN_BY_SERIAL_NUMBER,
		0);
	


	if (ftHandle != INVALID_HANDLE_VALUE)
	{
		try
		{
			//dumping user area of eeprom only
			DWORD UASize=0;
			FT_EE_UASize(ftHandle,&UASize);

			UCHAR * pUAData=new UCHAR[UASize];
			DWORD SizeRead=0;
			ftStatus=FT_EE_UARead(ftHandle,pUAData,UASize,&SizeRead);
			if (ftStatus!=FT_OK)
			{
				delete pUAData;
				throw std::exception("Can't read EEPROM user area");
			}
			delete pUAData;

			//dumping whole eeprom
			WORD EEPromData[0x40]={0};
			DWORD Offset=0;
			while (Offset < (sizeof(EEPromData) / sizeof (WORD)) )
			{
				WORD DataChunk=0;
				ftStatus=FT_ReadEE(ftHandle,Offset,&EEPromData[Offset]);
				if (ftStatus!=FT_OK)
					break;
				Offset++;
			}

			//setting up the port
			ftStatus=FT_SetBaudRate(ftHandle, FT_BAUD_115200);
			if (ftStatus!=FT_OK)
				throw std::exception("Can't set baud rate");

			ftStatus=FT_SetDataCharacteristics(ftHandle,FT_BITS_8,FT_STOP_BITS_1,FT_PARITY_NONE);
			if (ftStatus!=FT_OK)
				throw std::exception("Can't set data characteristics");

			ftStatus=FT_ClrDtr(ftHandle);
			if (ftStatus!=FT_OK)
				throw std::exception("Can't clear dtr");

			ftStatus=FT_SetDtr(ftHandle);
			if (ftStatus!=FT_OK)
				throw std::exception("Can't set dtr");


			//AT command conversation
			OVERLAPPED Ovl={0};
			Ovl.hEvent=CreateEvent(0,FALSE,FALSE,0); //create auto reset event

			DWORD Written=0;
			unsigned char cmd[]={'A','T','\r','\n'};
			ftStatus=FT_W32_WriteFile(ftHandle,cmd,sizeof(cmd),&Written,&Ovl);
			if (WaitForSingleObject(Ovl.hEvent,INFINITE)!=WAIT_OBJECT_0)
				throw std::exception("Error waiting write to complete");


			unsigned char Buf[250];
			ftStatus=FT_W32_ReadFile(ftHandle,Buf,sizeof(Buf),&TotalRead,&Ovl);
			if (WaitForSingleObject(Ovl.hEvent,1000)!=WAIT_OBJECT_0)
				throw std::exception("Error waiting read operation");

		}
		catch (std::exception & e)
		{
			printf(e.what());
		}

		FT_W32_CloseHandle(ftHandle);
	}
	

	return 0;
}

