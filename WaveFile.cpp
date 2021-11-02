#include <stdio.h>
#include <string.h>
#include <math.h>

#define L_CHANNEL_FREQ 440.0
#define R_CHANNEL_FREQ (L_CHANNEL_FREQ * (double(3) / 2))

struct SMinimalWaveFileHeader
{
    //the main chunk
    unsigned char m_szChunkID[4]; // "RIFF"
    long m_nChunkSize;            // 4 + number of chunks
    unsigned char m_szFormat[4];  // "WAVE"

    //sub chunk 1
    unsigned char m_szSubChunk1ID[4]; // "fmt "
    long m_nSubChunk1Size;
    short m_nAudioFormat; // != 1 ? compressed : uncompressed
    unsigned short m_nNumChannels;
    unsigned long m_nSampleRate;
    unsigned long m_nByteRate;
    unsigned short m_nBlockAlign;
    unsigned short m_nBitsPerSample;

    //sub chunk 2 "data"
    unsigned char m_szSubChunk2ID[4]; // "data"
    long m_nSubChunk2Size;

    //then comes the data!
};

template <class T>
bool WriteWaveFile(const char *szFileName, T *pData, long numSamples, unsigned short nNumChannels, unsigned long nSampleRate, unsigned long nBitsPerSample)
{
    //open the file if we can
    FILE *File = fopen(szFileName, "w+b");
    if (!File)
    {
        return false;
    }

    SMinimalWaveFileHeader waveHeader;

    //fill out the main chunk
    memcpy(waveHeader.m_szChunkID, "RIFF", 4);
    long dataSize = sizeof(pData[0]) * numSamples;
    waveHeader.m_nChunkSize = dataSize + sizeof(SMinimalWaveFileHeader) - 8;
    memcpy(waveHeader.m_szFormat, "WAVE", 4);

    //fill out sub chunk 1 "fmt "
    memcpy(waveHeader.m_szSubChunk1ID, "fmt ", 4);
    waveHeader.m_nSubChunk1Size = 16;
    waveHeader.m_nAudioFormat = 1;
    waveHeader.m_nNumChannels = nNumChannels;
    waveHeader.m_nSampleRate = nSampleRate;
    waveHeader.m_nByteRate = nSampleRate * nNumChannels * nBitsPerSample / 8;
    waveHeader.m_nBlockAlign = nNumChannels * nBitsPerSample / 8;
    waveHeader.m_nBitsPerSample = nBitsPerSample;

    //fill out sub chunk 2 "data"
    memcpy(waveHeader.m_szSubChunk2ID, "data", 4);
    waveHeader.m_nSubChunk2Size = dataSize;

    //write the header
    fwrite(&waveHeader, sizeof(SMinimalWaveFileHeader), 1, File);

    //write the wave data itself
    fwrite(pData, nDataSize, 1, File);

    //close the file and return success
    fclose(File);
    return true;
}

int main()
{
    long sampleRate = 44100;
    long numSeconds = 4;
    long numChannels = 2;
    long numSamples = sampleRate * numSeconds * numChannels;
    short *pData = new short[numSamples];

    int i;
    double amp = 0.5;
    for (i = 0; i < numSamples; i += 2)
    {
        short rawValLeft = sin(L_CHANNEL_FREQ * 2 * M_PI * i / sampleRate) * SHRT_MAX;
        short rawValRight = sin(R_CHANNEL_FREQ * 2 * M_PI * i / sampleRate) * SHRT_MAX;
        pData[i] = rawValLeft * amp;
        pData[i + 1] = rawValRight * amp;
    }

    WriteWaveFile("sound.wav", pData, numSamples, numChannels, sampleRate, sizeof(pData[0]) * 8);

    delete[] pData;

    return true;
}