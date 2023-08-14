#pragma once
#include "injector\injector.hpp"

#define METHOD __fastcall
#define DUMMY_ARG int

template<typename RawType, typename Type>
RawType *raw_ptr(Type *p, unsigned int offset = 0) {
    return reinterpret_cast<RawType *>(reinterpret_cast<unsigned int>(p) + offset);
}

template <unsigned int address, typename... Args>
void Call(Args... args) {
    reinterpret_cast<void(__cdecl *)(Args...)>(address)(args...);
}

template <typename Ret, unsigned int address, typename... Args>
Ret CallAndReturn(Args... args) {
    return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
}

template <unsigned int address, typename C, typename... Args>
void CallMethod(C _this, Args... args) {
    reinterpret_cast<void(__thiscall *)(C, Args...)>(address)(_this, args...);
}

template <typename Ret, unsigned int address, typename C, typename... Args>
Ret CallMethodAndReturn(C _this, Args... args) {
    return reinterpret_cast<Ret(__thiscall *)(C, Args...)>(address)(_this, args...);
}

template <unsigned int tableIndex, typename C, typename... Args>
void CallVirtualMethod(C _this, Args... args) {
    reinterpret_cast<void(__thiscall *)(C, Args...)>((*reinterpret_cast<void ***>(_this))[tableIndex])(_this, args...);
}

template <typename Ret, unsigned int tableIndex, typename C, typename... Args>
Ret CallVirtualMethodAndReturn(C _this, Args... args) {
    return reinterpret_cast<Ret(__thiscall *)(C, Args...)>((*reinterpret_cast<void ***>(_this))[tableIndex])(_this, args...);
}

template <typename... Args>
void CallDynGlobal(unsigned int address, Args... args) {
    reinterpret_cast<void(__cdecl *)(Args...)>(address)(args...);
}

template <typename Ret, typename... Args>
Ret CallAndReturnDynGlobal(unsigned int address, Args... args) {
    return reinterpret_cast<Ret(__cdecl *)(Args...)>(address)(args...);
}

template <typename C, typename... Args>
void CallMethodDynGlobal(unsigned int address, C _this, Args... args) {
    reinterpret_cast<void(__thiscall *)(C, Args...)>(address)(_this, args...);
}

template <typename Ret, typename C, typename... Args>
Ret CallMethodAndReturnDynGlobal(unsigned int address, C _this, Args... args) {
    return reinterpret_cast<Ret(__thiscall *)(C, Args...)>(address)(_this, args...);
}

class patch {
public:
    inline static void Nop(unsigned int address, unsigned int size){
        injector::MakeNOP(address, size, true);
    }

    inline static unsigned int RedirectCall(unsigned int address, void *func) {
        return injector::MakeCALL(address, func, true).as_int();
    }

    inline static unsigned int RedirectJump(int address, void *func) {
        return injector::MakeJMP(address, func, true).as_int();
    }

    inline static void SetChar(unsigned int address, char value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetUChar(unsigned int address, unsigned char value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetShort(unsigned int address, short value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetUShort(unsigned int address, unsigned short value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetInt(unsigned int address, int value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetUInt(unsigned int address, unsigned int value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetFloat(unsigned int address, float value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetDouble(unsigned int address, double value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetPointer(unsigned int address, void *value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetPointer(unsigned int address, char const *value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static void SetPointer(unsigned int address, wchar_t const *value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    inline static char GetChar(unsigned int address, bool vp = true) {
        return injector::ReadMemory<char>(address, vp);
    }

    inline static unsigned char GetUChar(unsigned int address, bool vp = true) {
        return injector::ReadMemory<unsigned char>(address, vp);
    }

    inline static short GetShort(unsigned int address, bool vp = true) {
        return injector::ReadMemory<short>(address, vp);
    }

    inline static unsigned short GetUShort(unsigned int address, bool vp = true) {
        return injector::ReadMemory<unsigned short>(address, vp);
    }

    inline static int GetInt(unsigned int address, bool vp = true) {
        return injector::ReadMemory<int>(address, vp);
    }

    inline static unsigned int GetUInt(unsigned int address, bool vp = true) {
        return injector::ReadMemory<unsigned int>(address, vp);
    }

    inline static float GetFloat(unsigned int address, bool vp = true) {
        return injector::ReadMemory<float>(address, vp);
    }

    inline static double GetDouble(unsigned int address, bool vp = true) {
        return injector::ReadMemory<double>(address, vp);
    }

    inline static void *GetPointer(unsigned int address, bool vp = true) {
        return injector::ReadMemory<void *>(address, vp);
    }

    template <typename T>
    static void Set(unsigned int address, T value, bool vp = true) {
        injector::WriteMemory(address, value, vp);
    }

    template <typename T>
    static T Get(unsigned int address, bool vp = true) {
        return injector::ReadMemory<T>(address, vp);
    }
};
