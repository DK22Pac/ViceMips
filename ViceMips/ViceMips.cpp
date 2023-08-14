#include "Patch.h"

bool bDisableD3DPools = true;
bool bDontCreateFullMipChain = false; // Take number of levels from TXD instead of creating full chain. Works only when bDisableD3DPools is true
int nMagFilter = -1;
int nMinFilter = -1;
int nMipFilter = -1;
int nMaxMipLevel = -1;
int nMaxAnisotropy = -1;
float fMipMapLodBias = 0.0f;
unsigned int fnRwD3D8RWSetRasterStage = 0;
unsigned int fnSetTextureStageState = 0;
unsigned int fnReadTextureFormat = 0;
int nNumLevelsToCreate = -1;
unsigned int nGameVersion = 0;

class ViceMips {
public:
    static unsigned int ByVersion(unsigned int A_10EN, unsigned int A_11EN, unsigned int A_Steam) {
        switch (nGameVersion) {
        case 1:
            return A_10EN;
        case 2:
            return A_11EN;
        case 3:
            return A_Steam;
        }
        return 0;
    }

    static void ReadSettings() {
        bDisableD3DPools = GetPrivateProfileIntA("MAIN", "bDisableD3DPools", 1, ".\\ViceMips.ini");
        bDontCreateFullMipChain = GetPrivateProfileIntA("MAIN", "bDontCreateFullMipChain", 0, ".\\ViceMips.ini");
        nMagFilter = GetPrivateProfileIntA("STATES", "nMagFilter", -1, ".\\ViceMips.ini");
        nMinFilter = GetPrivateProfileIntA("STATES", "nMinFilter", -1, ".\\ViceMips.ini");
        nMipFilter = GetPrivateProfileIntA("STATES", "nMipFilter", -1, ".\\ViceMips.ini");
        nMaxMipLevel = GetPrivateProfileIntA("STATES", "nMaxMipLevel", -1, ".\\ViceMips.ini");
        nMaxAnisotropy = GetPrivateProfileIntA("STATES", "nMaxAnisotropy", -1, ".\\ViceMips.ini");
        fMipMapLodBias = 0.0f;
        char buf[32];
        GetPrivateProfileStringA("STATES", "fMipMapLodBias", "0.0", buf, 32, ".\\ViceMips.ini");
        sscanf(buf, "%f", &fMipMapLodBias);
    }

    static void OnRwD3D8RWSetRasterStage(void *raster, unsigned int stage) {
        if (nMagFilter != -1)
            CallDynGlobal(fnSetTextureStageState, stage, 16, nMagFilter); // RwD3D8SetTextureStageState
        if (nMinFilter != -1)
            CallDynGlobal(fnSetTextureStageState, stage, 17, nMinFilter); // RwD3D8SetTextureStageState
        if (nMipFilter != -1)
            CallDynGlobal(fnSetTextureStageState, stage, 18, nMipFilter); // RwD3D8SetTextureStageState
        if (nMaxMipLevel != -1)
            CallDynGlobal(fnSetTextureStageState, stage, 20, nMaxMipLevel); // RwD3D8SetTextureStageState
        if (nMaxAnisotropy != -1)
            CallDynGlobal(fnSetTextureStageState, stage, 21, nMaxAnisotropy); // RwD3D8SetTextureStageState
        if (fMipMapLodBias != 0.0)
            CallDynGlobal(fnSetTextureStageState, stage, 19, fMipMapLodBias); // RwD3D8SetTextureStageState
        if (fnRwD3D8RWSetRasterStage)
            CallDynGlobal(fnRwD3D8RWSetRasterStage, raster, stage); // original RwD3D8RWSetRasterStage
    }

    static void *GetD3DDevice() {
        return *(void **)ByVersion(0x7897A8, 0x7897B0, 0x7887B0);
    }

    static HRESULT MyCreateTexture(unsigned int width, unsigned int height, unsigned int oneLevel, unsigned int format, void **ppTexture) {
        if (bDontCreateFullMipChain) {
            if (nNumLevelsToCreate > 0)
                oneLevel = nNumLevelsToCreate;
            nNumLevelsToCreate = -1;
        }
        return CallVirtualMethodAndReturn<HRESULT, 20>(GetD3DDevice(), GetD3DDevice(), width, height, oneLevel, 0, format, 1, ppTexture); // GetD3DDevice()->CreateTexture()
    }

    static void MyDestroyTexture(void *texture) {
        CallVirtualMethod<2>(texture, texture); // texture->Release()
    }

    static HRESULT MyCreateIndexBuffer(unsigned int count, unsigned int format, void **ppIndexBuffer) {
        unsigned int pool = 1; // D3DPOOL_MANAGED
        if (nGameVersion != 1 && *(unsigned char *)ByVersion(0x0, 0xA10B56, 0xA0FB57)) // IsGeForceFX (1.1 patch)
            pool = 2; // D3DPOOL_SYSTEMMEM
        return CallVirtualMethodAndReturn<HRESULT, 24>(GetD3DDevice(), GetD3DDevice(), count * 2, 8, format, pool, ppIndexBuffer); // GetD3DDevice()->CreateIndexBuffer()
    }

    static void MyDestroyIndexBuffer(void *indexBuffer) {
        CallVirtualMethod<2>(indexBuffer, indexBuffer); // indexBuffer->Release()
    }

    static unsigned int OnReadTextureFormat(void *stream, unsigned char *format, unsigned int size) {
        unsigned int result = CallAndReturnDynGlobal<unsigned int>(fnReadTextureFormat, stream, format, size);
        if (result == size)
            nNumLevelsToCreate = format[0xD];
        return result;
    }

    ViceMips() {
        if (patch::GetUInt(0x667BF0) == 0x53E58955)
            nGameVersion = 1;
        else if (patch::GetUInt(0x667C40) == 0x53E58955)
            nGameVersion = 2;
        else if (patch::GetUInt(0x666BA0) == 0x53E58955)
            nGameVersion = 3;
        if (nGameVersion == 0) {
            MessageBox(NULL, "ViceMips plugin doesn't work with this game version", "ViceMips.asi", MB_ICONWARNING);
            return;
        }
        ReadSettings();
        patch::Nop(ByVersion(0x63F0B9, 0x63F109, 0x63E069), 8); // Remove code which disables mipmap flags
        if (bDisableD3DPools) {
            patch::SetUChar(ByVersion(0x6DB8E8, 0x786D44, 0x785D44), 0); // D3DResourceBuffering flag
            if (nGameVersion != 1)
                patch::SetUChar(ByVersion(0x0, 0x786BA8, 0x785BA8), 0); // D3DResourceBuffering flag 2
            patch::SetUChar(ByVersion(0x626F70, 0x626F50, 0x626BC0), 0xC3); // disable - EnableD3DResourceBuffering
            patch::SetUChar(ByVersion(0x626F80, 0x626F60, 0x626BD0), 0xC3); // disable - ReleaseD3DResourceBuffer
            patch::SetUChar(ByVersion(0x627080, 0x627060, 0x626CD0), 0xC3); // disable - ShutdownTexturePools
            patch::SetUChar(ByVersion(0x6271E0, 0x6271C0, 0x626E30), 0xC3); // disable - InitTexturePools
            patch::SetUChar(ByVersion(0x6272E0, 0x6272F0, 0x626F60), 0xC3); // disable - TidyUpIndexBufferPools
            patch::SetUChar(ByVersion(0x627590, 0x6275E0, 0x627250), 0xC3); // disable - TidyUpTexturePools
            patch::SetUChar(ByVersion(0x627B60, 0x627BB0, 0x627820), 0xC3); // disable - D3DResourceBuffering construction
            patch::RedirectJump(ByVersion(0x627840, 0x627890, 0x627500), MyCreateTexture);
            patch::RedirectJump(ByVersion(0x627620, 0x627670, 0x6272E0), MyDestroyTexture);
            patch::RedirectJump(ByVersion(0x6274D0, 0x6274F0, 0x627160), MyCreateIndexBuffer);
            patch::RedirectJump(ByVersion(0x6273B0, 0x6273C0, 0x627030), MyDestroyIndexBuffer);
            if (bDontCreateFullMipChain)
                fnReadTextureFormat = patch::RedirectCall(ByVersion(0x63F09B, 0x63F0EB, 0x63E04B), OnReadTextureFormat);
        }
        if (nMagFilter != -1 || nMinFilter != -1 || nMipFilter != -1 || nMaxMipLevel != -1 || nMaxAnisotropy != -1 || fMipMapLodBias != 0.0f)
            fnRwD3D8RWSetRasterStage = patch::RedirectCall(ByVersion(0x65988D, 0x6598DD, 0x65883D), OnRwD3D8RWSetRasterStage);
        fnSetTextureStageState = ByVersion(0x658310, 0x658360, 0x6572C0);

    }
} viceMips;
