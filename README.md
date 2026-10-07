# Laboratorio Keylogger Avanzado - Evasión con Hardware Breakpoints

## Objetivo
Investigar cómo un keylogger puede evadir Windows Defender sin escalada de privilegios,
usando hardware breakpoints (DR0-DR7) con la variante NtContinue para evitar ETW-TI.

## Arquitectura
- Keylogger y exfiltración: Python (rápido, legible)
- Bypass AMSI/ETW: DLL en C++ (nativo, pequeño, control total del CONTEXT)

## Diferenciación técnica
- NO se usa SetThreadContext (genera KERNEL_THREATINT_TASK_SETTHREADCONTEXT)
- SÍ se usa NtContinue (no genera ese evento ETW-TI)
- AMSI bypass patchless sobre AmsiScanBuffer
- ETW bypass patchless sobre EtwEventWrite

## Estructura
- 00_bitacora/      Documentación de avances
- 01_scripts_python/ Keylogger y exfiltración
- 02_dll_cpp/       Código fuente C++
- 03_compilado/     DLL compilada
- 04_logs/          Capturas de teclado
- 05_capturas/      Screenshots de cada fase


## Fases
- [ ] Fase 1: Archivos base y bitácora
- [ ] Fase 2: Crear proyecto DLL en Visual Studio
- [ ] Fase 3: Implementar bypass AMSI/ETW en C++
- [ ] Fase 4: Compilar DLL e integrar con Python
- [ ] Fase 5: Exfiltración por Telegram
- [ ] Fase 6: Medición con Defender activo


## Advertencia
Investigación educativa en sistema propio. No usar en sistemas de terceros.
