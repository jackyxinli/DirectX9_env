//Åö Midi,Wave ã§í ÇÃ Audio Object ÇíËã`Ç∑ÇÈ   2007-09-01
#include    <dmusici.h>
#pragma     comment(lib,"dxguid.lib")
#pragma     comment(lib,"dxerr9.lib")
#pragma     comment(lib,"winmm.lib")
#define     ERRMSG(x)  MessageBox(NULL,x,"Audio",MB_OK);

class Audio
{ protected:
    IDirectMusicLoader8*      g_pLoader;
    IDirectMusicSegment8*     g_pSegment;

    WCHAR       AudioFile[MAX_PATH];
    HRESULT     InitializeSynth();
    HRESULT     LoadSegment(IDirectMusicSegment8** pSegment);
    HRESULT     LoadRes(HMODULE hMod, WORD ResourceID,
                        IDirectMusicSegment8** pSegment);
    HRESULT     InitDirectAudio();
    HRESULT     FreeDirectAudio();

  public:
    IDirectMusicPerformance8* g_pPerformance;
    IDirectMusicPerformance8* g_pPerformance2;

    Audio();                    //Constructor
    ~Audio();                   //Destructor
    void        Play(int rep,IDirectMusicSegment8* pSegment,
                             IDirectMusicPerformance8* pPerform);
    void        Play(int rep= 0)  { Play(rep,g_pSegment,g_pPerformance); }
    void        Play2(int rep= 0) { Play(rep,g_pSegment,g_pPerformance2); }
    void        Stop(IDirectMusicPerformance8* pPerform);
    void        Stop()  { Stop(g_pPerformance); }
    void        Stop2() { Stop(g_pPerformance2); }
    void        SetVol(IDirectMusicPerformance8* pPerform, long *nVol);
    void        SetVol(long *nVol)  { SetVol(g_pPerformance, nVol);  }
    void        SetVol2(long *nVol) { SetVol(g_pPerformance2, nVol); }
    HRESULT     Load(LPSTR Audio,IDirectMusicSegment8** pSegment,
                                 IDirectMusicPerformance8* pPerform);
    HRESULT     Load(LPSTR Audio)
                { return(Load(Audio,&g_pSegment,g_pPerformance)); }
    HRESULT     Load2(LPSTR Audio)
                { return(Load(Audio,&g_pSegment,g_pPerformance2)); }
    HRESULT     Load(HMODULE hMod, WORD ResourceID,
                     IDirectMusicSegment8** pSegment,
                     IDirectMusicPerformance8* pPerform);
    HRESULT     Load(HMODULE hMod, WORD ResourceID)
                { return(Load(hMod,ResourceID,&g_pSegment,g_pPerformance)); }
    HRESULT     Load2(HMODULE hMod, WORD ResourceID)
                { return(Load(hMod,ResourceID,&g_pSegment,g_pPerformance2)); }
};


