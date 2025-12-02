# m_autonotary — Verified Badge Module for InspIRCd  
### Exclusivo para ElCorito.org — Creado por **r4f3**

**m_autonotary** es el primer módulo de certificación automática y manual para InspIRCd, diseñado exclusivamente para ElCorito.org.

Permite marcar usuarios como **verificados** (similar al badge de Twitter/Instagram) mediante:
- Autoverificación por WebIRC confiado  
- Reglas automáticas configurables  
- Comando manual para IRCopers

---

## ✨ Características

✔ Certificación automática vía WebIRC  
✔ Certificación manual con `/NOTARY`  
✔ Badge visible en WHOIS  
✔ Persistencia usando metadata  
✔ Completamente original  
✔ Compatible con InspIRCd 4.x  

---

# m_autonotary — Verified Badge Module for InspIRCd

## Ejemplo de WHOIS
r4f3 is using a verified ElCorito.org account ✔
Verification: Automatic (WebIRC Gateway)

---

## Instalación

### 1. Copiar el módulo
Coloca `m_autonotary.cpp` en:

/path/to/inspircd/src/modules/

### 2. Compilar el módulo

make module MODULE=m_autonotary.cpp
make install

### 3. Activar en inspircd.conf

<module name="m_autonotary">

<autonotary 
    trustgateway="yes"
    minregistered="7"
    minontime="5">



