//★ Midi, Wave 共通の Audio Object を定義する     2007-09-01
#include    "audio.h"

//★★ Audio Object Class 関数
// Constructor(オブジェクトの初期化)
Audio::Audio()
{   g_pLoader= NULL;
    g_pPerformance= NULL;
    g_pPerformance2= NULL;
    g_pSegment= NULL;
    InitDirectAudio();
}
// Destructor(オブジェクトの終了)
Audio::~Audio()
{   FreeDirectAudio();
}

//★Load Midi Wave File
HRESULT  Audio::Load(LPSTR Audio, IDirectMusicSegment8** pSegment,
                     IDirectMusicPerformance8* pPerform)
{   HRESULT hr;
    WORD    i;

    if (NULL != pPerform)
    {   pPerform->Stop(NULL,NULL,0,0);
        if (NULL != *pSegment)
        {   (*pSegment)->Unload(pPerform);
            (*pSegment)= NULL;
        }
    }
    for(i=0; i<strlen(Audio); i++)  AudioFile[i]= Audio[i];
    AudioFile[i]= 0;
    // Load the segment
    hr = LoadSegment(pSegment);
    if (FAILED(hr))
    {   ERRMSG("Could not load segment");  return hr;  }
    (*pSegment)->Download(pPerform);
    return S_OK;
}

//★Load Midi Wave Resource
HRESULT  Audio::Load(HMODULE hMod, WORD ResourceID,
                     IDirectMusicSegment8** pSegment,
                     IDirectMusicPerformance8* pPerform)
{   HRESULT hr;
    if (NULL != pPerform)
    {   pPerform->Stop(NULL,NULL,0,0);
        if (NULL != *pSegment)
        {   (*pSegment)->Unload(pPerform);
            (*pSegment)= NULL;
        }
    }
    // Load the segment
    hr = LoadRes(hMod,ResourceID,pSegment);
    if (FAILED(hr))
    {   ERRMSG("Could not load segment");  return hr;  }
    (*pSegment)->Download(pPerform);
    return S_OK;
}

//★StopPlay
void  Audio::Play(int rep,IDirectMusicSegment8* pSegment,IDirectMusicPerformance8* pPerform)
{   // set the segment to repeat many times
    pSegment->SetRepeats(rep);
    pPerform->PlaySegmentEx(pSegment,NULL,NULL,0,0,NULL,NULL,NULL);
}

//★StopAudio
void  Audio::Stop(IDirectMusicPerformance8* pPerform)
{   if (NULL != pPerform)
        pPerform->Stop(NULL,NULL,0,0);
}

//★SetVol
void  Audio::SetVol(IDirectMusicPerformance8* pPerform, long *nVol)
{   if (NULL != pPerform)
    pPerform->SetGlobalParam(GUID_PerfMasterVolume,(void*)nVol,sizeof(long));
}

//★InitDirectAudio()
HRESULT Audio::InitDirectAudio()
{   HRESULT hr;

    hr= CoInitialize(NULL);
    if (FAILED(hr))
    {   ERRMSG("Could not initialize COM");  return hr;   }
    // Create loader object
    hr= CoCreateInstance(CLSID_DirectMusicLoader,NULL,CLSCTX_INPROC, 
                         IID_IDirectMusicLoader8,(void**)&g_pLoader);
    if (FAILED(hr))
    {   ERRMSG("Could not create DMusic Loader");
        FreeDirectAudio();
        return hr;
    }
    // Create performance object
    hr= CoCreateInstance(CLSID_DirectMusicPerformance,NULL,CLSCTX_INPROC,
                    IID_IDirectMusicPerformance8,(void**)&g_pPerformance);
    hr= CoCreateInstance(CLSID_DirectMusicPerformance,NULL,CLSCTX_INPROC,
                    IID_IDirectMusicPerformance8,(void**)&g_pPerformance2);
    if (FAILED(hr))
    {   ERRMSG("Could not create DMusic Performance");
        FreeDirectAudio();
        return hr;
    }
    // Initialize the software synthesizer
    hr= InitializeSynth();
    if (FAILED(hr))
    {   FreeDirectAudio();
        return hr;
    }
    return S_OK;
}

//★InitializeSynth()
HRESULT Audio::InitializeSynth()
{   HRESULT hr;

    hr= g_pPerformance->InitAudio(NULL,NULL,NULL,DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                  64,DMUS_AUDIOF_ALL,NULL);
    hr= g_pPerformance2->InitAudio(NULL,NULL,NULL,DMUS_APATH_SHARED_STEREOPLUSREVERB,
                                  64,DMUS_AUDIOF_ALL,NULL);
    if (FAILED(hr))
    {   ERRMSG("Could not initialize performance");  return hr;  }
    return S_OK;
}

//★LoadSegment()
HRESULT Audio::LoadSegment(IDirectMusicSegment8** pSegment)
{   HRESULT     hr;
    DMUS_OBJECTDESC ObjDesc; // Object descriptor for pLoader->GetObject()

    // sections load as type Segment, as do MIDI files, for example.
    ObjDesc.guidClass = CLSID_DirectMusicSegment;
    ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
    wcscpy_s(ObjDesc.wszFileName,MAX_PATH,AudioFile);
    ObjDesc.dwValidData= DMUS_OBJ_CLASS | DMUS_OBJ_FILENAME;
    hr= g_pLoader->GetObject(&ObjDesc,IID_IDirectMusicSegment8,(void**)pSegment);
    return hr;
}

//★Resource から Midi,Wave を取得
HRESULT  Audio::LoadRes(HMODULE hMod, WORD ResourceID,IDirectMusicSegment8** pSegment)
{   HRESULT         hr;
    DMUS_OBJECTDESC ObjDesc;

    HRSRC hFound= FindResource(hMod,MAKEINTRESOURCE(ResourceID),"WAVE");
    if (hFound==NULL)
    {   ERRMSG("Not Found Wave(Midi) Resource");
        return  S_FALSE;
    }
    HGLOBAL hRes= LoadResource(hMod,hFound);
    ObjDesc.dwSize = sizeof(DMUS_OBJECTDESC);
    ObjDesc.guidClass = CLSID_DirectMusicSegment;
    ObjDesc.dwValidData = DMUS_OBJ_CLASS | DMUS_OBJ_MEMORY;
    ObjDesc.pbMemData = (BYTE *) LockResource(hRes);
    ObjDesc.llMemLength = SizeofResource(hMod, hFound);
    hr= g_pLoader->GetObject(&ObjDesc,IID_IDirectMusicSegment8, 
                   (void**)pSegment);
    return hr;
}

//★FreeDirectAudio()
HRESULT Audio::FreeDirectAudio()
{   Stop(g_pPerformance);
    // Release the Segment
    if (NULL != g_pSegment)
    {   g_pSegment->Release();  g_pSegment= NULL;  }
    // If there is any music playing, stop it.
    if (NULL != g_pPerformance)
    {   g_pPerformance->Stop(NULL,NULL,0,0);
        // CloseDown and Release the performance object
        g_pPerformance->CloseDown();
        g_pPerformance->Release();
        g_pPerformance = NULL;
    }
    if (NULL != g_pPerformance2)
    {   g_pPerformance2->Stop(NULL,NULL,0,0);
        g_pPerformance2->CloseDown();
        g_pPerformance2->Release();
        g_pPerformance2 = NULL;
    }
    // Release the loader object
    if (NULL != g_pLoader)
    {   g_pLoader->Release();   g_pLoader= NULL;  }
    // Release COM
    CoUninitialize();
    return S_OK;
}


