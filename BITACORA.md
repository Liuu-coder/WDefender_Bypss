# Bitácora del Laboratorio
## Protocolo de contingencia

### Si falla la compilación de la DLL (C++)
- Plan A: Revisar errores de sintaxis, headers faltantes, configuración de VS
- Plan B: Compilar en modo Debug para ver el error exacto
- Plan C: Simplificar la DLL (solo bypass AMSI, sin ETW) y añadir ETW después
- Plan D: Usar la técnica sin DLL (AMSI bypass desde Python puro con ctypes)
- Plan E: Documentar el intento fallido como "hallazgo de investigación"

### Si falla la carga de la DLL desde Python
- Plan A: Verificar ruta, arquitectura (x64 vs x86), permisos
- Plan B: Cargar la DLL desde PowerShell con Add-Type como prueba aislada
- Plan C: Usar un loader separado en C++ que inyecte la DLL
- Plan D: Documentar como "limitación de la integración Python/C++"

### Si Defender detecta el keylogger
- Plan A: Identificar qué capa lo detectó (firma, heurística, comportamiento)
- Plan B: Aplicar la evasión específica para esa capa
- Plan C: Cambiar el vector de entrega (no ejecutar directamente)
- Plan D: Documentar la detección como hallazgo (¡esto es valioso para la conferencia!)

### Si Defender detecta la DLL de bypass
- Plan A: Ofuscar strings y imports
- Plan B: Compilar con diferentes flags (/O2, /MT, /GL)
- Plan C: Cambiar el entrypoint y la estructura PE
- Plan D: Documentar qué regla específica la detectó y por qué

### Si el bypass funciona pero no exfiltra
- Plan A: Verificar token/chat_id de Telegram
- Plan B: Cambiar a canal alternativo (Discord webhook, pastebin, DNS)
- Plan C: Usar exfiltración por archivo local (menos realista pero funcional)
- Plan D: Documentar el fallo como "limitación del canal"

### 2026-10-02 - Corrección metodológica: exclusión removida
- Qué hice: Removí la exclusión de C:\LabKeylogger en Windows Defender
- Razón: La prueba anterior pudo haber dado falso positivo por la exclusión
- Estado de Defender: Protección en tiempo real ACTIVA, sin exclusiones
- Próximo paso: Repetir test_bypass_amsi.py con Defender activo
- Hipótesis: Si el bypass es real, AMSI seguirá devolviendo 0
- Riesgo: Defender podría bloquear la DLL o el script al ejecutarlos

### 2026-10-02 - HITO CONFIRMADO: Bypass AMSI funcional sin exclusiones
- Defender: Protección en tiempo real ACTIVA, sin exclusiones
- Tiempo de espera post-remoción: 2+ minutos
- Resultado: AMSI Result = 0 para "Invoke-Mimikatz -DumpCreds"
- Sin bloqueos de Defender en la carga de la DLL
- Sin bloqueos en la ejecución del script Python
- Evidencia: 05_capturas\2026-10-02_bypass_amsi_defender_activo.png
- Conclusión: El bypass con NtContinue es REAL y evade AMSI sin exclusiones
- Diferenciador vs. técnicas clásicas: sin parches, sin ETW-TI
- Próximo paso: Integrar con keylogger + exfiltración Telegram

### 2026-10-02 - HITO: Keylogger + Bypass funcionando con Defender activo
- Qué hice: Integré keylogger_con_bypass.py con la DLL AmsiEtwBypass.dll
- Defender: Protección en tiempo real ACTIVA, sin exclusiones
- Resultado:
  * Bypass AMSI/ETW cargado correctamente
  * Captura de teclado con contexto de ventana funcional
  * CERO alertas de Defender
  * CERO errores
- Evidencia: 05_capturas\2026-10-02_keylogger_bypass_funcional.png
- Captura verificada: usuario@test.local, Demo1234!
- Conclusión: Cadena completa funcional sin detección
- Próximo paso: Exfiltración por Telegram

### 2026-10-02 - HITO PRINCIPAL: Cadena completa funcional
- Defender: Protección en tiempo real ACTIVA, sin exclusiones
- Componentes activos:
  * Bypass AMSI/ETW: ACTIVO (NtContinue + hardware breakpoints)
  * Keylogger con contexto de ventana: FUNCIONAL
  * Exfiltración Telegram: FUNCIONAL
- Evidencia:
  * Log local: 04_logs\captura_final.txt
  * Terminal: "[+] Exfiltrado: 641 caracteres @ 03:20:09"
  * Mensajes recibidos en Telegram (celular)
- Resultado:
  * CERO alertas de Defender
  * CERO errores
  * Cadena completa sin detección
- Conclusión:
  * Un keylogger con bypass AMSI/ETW puede comprometer credenciales
    corporativas en minutos, sin ser detectado por Windows Defender
- Próximo paso: Añadir screenshot + info del sistema + compilar a .exe

### 2026-10-02 - Verificación post-ejecución
- Historial de protección Defender: SIN EVENTOS nuevos
- Visor de Eventos Defender Operational: SIN EVENTOS nuevos
- Proceso python.exe: SIN bloqueos
- Conclusión: Defender no detectó la cadena completa

### 2026-10-02 - HITO: V2 Stealth funcional
- Recon: hostname, usuario, OS, MAC, IP local, RAM, CPUs, procesos
- Geolocalización: IP pública, ciudad, región, país, ISP
- Screenshot: 255 KB enviado en memoria (nunca tocó disco)
- Exfiltración con jitter: 128s de espera, 1862 chars enviados
- Bypass AMSI/ETW: ACTIVO
- Defender: SIN alertas
- Warning de mss corregido: compatible con mss < 9.0 y >= 9.0
- Evidencia: 05_capturas\2026-10-02_v2_stealth.png
- Conclusión: Cadena completa funcional, evasiva, sin patrones detectables
- Próximo paso: Compilar a .exe con Nuitka

### 2026-10-02 - HITO: C2 completo funcional vía Telegram
- Defender: Protección en tiempo real ACTIVA, sin exclusiones
- Comandos verificados:
  * /status      → respondido en <10s
  * /screenshot  → captura bajo demanda (230 KB, en memoria)
  * /info        → info completa del sistema
  * /clipboard   → pendiente de probar
  * /processes   → pendiente de probar
  * /kill        → pendiente de probar
- Recon automático: info + geolocalización + screenshot inicial
- Exfiltración de teclas: jitter 90-180s funcional
- Fix aplicado: inicializar_offset() para saltar mensajes viejos
- Evidencia: 05_capturas\2026-10-02_c2_completo.png
- Conclusión: Cadena completa con C2 remoto, sin detección
- Próximo paso: Funciones diferenciadoras (a elegir)

### 2026-10-03 - Fallback cifrado verificado
- WiFi apagado: fallback activado correctamente
- Archivo creado: %LOCALAPPDATA%\...\INetCache\Content.Outlook\thumbcache.dat
- Tamaño: 1192 bytes
- Contenido: Fernet cifrado (gAAAAABm...)
- Cifrado funciona: NO legible en texto plano
- Al reconectar WiFi: DNS cache negativo impidió reconexión inmediata
- /log ejecutó al presionar ESC (race condition)
- Fixes aplicados: ipconfig /flushdns post-reconexión + delay 3s en ESC
- Próximo paso: Compilación con Nuitka

### 2026-10-03 - HITO: Sincronización automática post-reconexión
- WiFi apagado → fallback cifrado funcionó (2522 bytes)
- WiFi reconectado → sync_automatico detectó conexión en ≤30s
- Log descifrado y enviado a Telegram automáticamente
- Archivo thumbcache.dat eliminado por el propio malware
- CERO intervención manual en el proceso de recuperación
- Evidencia: 05_capturas\2026-10-03_sync_automatico.png
- Conclusión: Cadena resiliente con auto-recuperación post-desconexión
- Próximo paso: Compilación con Nuitka

### 2026-10-05 - HITO PRINCIPAL: Loader C++ funcional (autónomo)
- Compilación: WinUpdateServiceLoader.exe (~35.6 MB)
- Recursos embebidos:
  * python_embed.zip (35 MB, runtime completo de Python 3.13)
  * script_hex.txt (42.8 KB, keylogger cifrado XOR)
- Funcionamiento verificado:
  * Extracción del ZIP a Content.IE5\ (carpeta oculta del sistema)
  * Carga de python313.dll desde extracción
  * Python 3.13 embebido inicializado correctamente
  * Script descifrado en memoria y ejecutado
  * Bypass AMSI/ETW activo
  * Recon completo enviado a Telegram
  * Screenshot 288 KB enviado
  * C2 escuchando comandos
- Evidencia: 05_capturas\2026-10-05_loader_cpp_funcional.png
- Conclusión: Keylogger autónomo en un único .exe, sin Python instalado en víctima
- Próximo paso: modo Windows silencioso + prueba con Defender

### 2026-10-05 - HALLAZGO CRÍTICO: Sin procesos python.exe visibles
- El loader C++ carga Python en su propio proceso
- NO hay procesos python.exe hijos
- Solo se ve el proceso win_update_Service.exe (nombre legítimo)
- En el Administrador de tareas, un usuario normal NO detecta el ataque
- Evasión de comportamiento: EXITOSA
- Evidencia: 05_capturas\2026-10-05_sin_procesos_python.png

### 2026-10-05 - HITO FINAL: Implante APT funcional
- Proceso corriendo: win_update_Service.exe
- PID: 84412
- Ruta: C:\LabKeylogger\03_compilado\win_update_Service.exe
- RAM: 121.14 MB
- StartTime: 5/10/2026 11:22:03
- Verificaciones:
  * Defender: CERO detecciones (Get-MpThreatDetection vacío)
  * Procesos python.exe hijos: NINGUNO (todo dentro del loader)
  * Telegram /status: RESPONDIENDO correctamente
  * Nombre del proceso: win_update_Service (masquerading exitoso)
  * RAM consumida: 121 MB (indistinguible de una app normal)
- Conclusión:
  * Implante completamente autónomo
  * Indetectable por Defender
  * Indetectable visualmente en Task Manager
  * C2 funcional vía canal legítimo
- Evidencia: 05_capturas\2026-10-05_implante_final.png