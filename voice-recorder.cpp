#include "stdafx.h"


#include <Windows.h>
#include <string>
#include <iostream>
#include <mmsystem.h>
#include <fstream>
using namespace std;
#pragma comment(lib,"winmm.lib")


HWAVEIN hWaveIn;
WAVEHDR WaveInHdr;
MMRESULT result;
WAVEFORMATEX pFormat;



int main()
{
	int samplesperchan;
	int sampleRate;
	int *waveIn;


	samplesperchan=2;
	sampleRate = 8000;

	cout << "HardWare Configuration*\n"; 


	pFormat.wFormatTag = WAVE_FORMAT_PCM; //simple format
	pFormat.nChannels = 1; //1=mono 2=stereo
	pFormat.nSamplesPerSec = sampleRate; //44100
	pFormat.wBitsPerSample = 16; // high
	pFormat.nBlockAlign = pFormat.nChannels*pFormat.wBitsPerSample / 8;
	pFormat.nAvgBytesPerSec = pFormat.nChannels*pFormat.wBitsPerSample / 8;
	pFormat.cbSize = 0;

	result = waveInOpen(&hWaveIn, WAVE_MAPPER, &pFormat,
		0L, 0L, WAVE_FORMAT_DIRECT);

	if (result)
	{
		cout << "Error(1) \n";
	}

	int nSec = 1; //time of record
	waveIn = new int[sampleRate*nSec];

	WaveInHdr.lpData = (LPSTR)waveIn;
	WaveInHdr.dwBufferLength = sampleRate*nSec*pFormat.nBlockAlign;
	WaveInHdr.dwBytesRecorded = 0;
	WaveInHdr.dwUser = 0L;
	WaveInHdr.dwFlags = 0L;
	WaveInHdr.dwLoops = 0L;
	waveInPrepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));

	result = waveInAddBuffer(hWaveIn, &WaveInHdr, sizeof(WAVEHDR));
	if (result)
	{
		cout << "Error(2) \n";
	}

	result = waveInStart(hWaveIn);
	if (result)
	{
		cout << "Error(3) \n";
	}
	
	cout << "Recording for nSec....";

	do
	{

	} while (waveInUnprepareHeader(hWaveIn, &WaveInHdr, sizeof(WAVEHDR)) == WAVERR_STILLPLAYING);
	
	waveInStop(hWaveIn);
	waveInClose(hWaveIn);
	cout << "Stop Recording.\n";

	//SAVE
	MMCKINFO ChunkInfo;
	MMCKINFO FormatChunkInfo;
	MMCKINFO DataChunkInfo;

	HMMIO handle = mmioOpen(
		"test.wav", 0, MMIO_CREATE | MMIO_WRITE);
	if (!handle)
		cout << "Error(4)";
	int j = 0;
	for (j; j <= sampleRate; j++)
		cout << waveIn[j]<<endl;

	memset(&ChunkInfo, 0, sizeof(MMCKINFO));
	ChunkInfo.fccType = mmioStringToFOURCC("WAVE", 0);
	DWORD Res = mmioCreateChunk(
		handle, &ChunkInfo, MMIO_CREATERIFF);
	
	FormatChunkInfo.ckid = mmioStringToFOURCC("fmt ", 0);
	FormatChunkInfo.cksize = sizeof(WAVEFORMATEX);
	Res = mmioCreateChunk(handle, &FormatChunkInfo, 0);

	mmioWrite(handle, (char*)&pFormat, sizeof(pFormat));

	Res = mmioAscend(handle, &FormatChunkInfo, 0);
	DataChunkInfo.ckid = mmioStringToFOURCC("data", 0);
	DWORD DataSize = WaveInHdr.dwBytesRecorded;
	DataChunkInfo.cksize = DataSize;
	Res = mmioCreateChunk(handle, &DataChunkInfo, 0);

	mmioWrite(handle, (char*)WaveInHdr.lpData, DataSize);
	mmioAscend(handle, &DataChunkInfo, 0);
	mmioAscend(handle, &ChunkInfo, 0);
	mmioClose(handle, 0);


	int _i;
	cout << endl << "Saving and Close." << endl;
	cin >> _i;

}
