# GRANULAR LAB & GRANULAR SYNTH LAB

Suite profesional de plugins de audio granular en tiempo real para Linux (Ubuntu), compatible de forma nativa con **Bitwig Studio** y DAWs modernas en formatos **CLAP** y **VST3**.

Desarrollado en C++20 con JUCE 8 y extensiones CLAP (`clap-juce-extensions`), optimizado para procesamiento en tiempo real con cero asignación dinámica en el hilo de audio (*zero allocation in audio thread*), colas libres de bloqueo (*lock-free queues*) y diseño gráfico en tonalidades oscuras estilo grafito/neón.

---

## 📦 Plugins Incluidos en el Proyecto

### 1. 🎛️ GRANULAR LAB (Efecto de Audio)
Plugin de inserción diseñado para transformar señales entrantes (voces, sintetizadores, guitarras, percusiones y ambientes) en nubes de partículas granulares, paisajes sonoros evolutivos y texturas glitch.
- **Tipo**: Efecto de audio (CLAP / VST3 / Standalone).
- **Buffer Circular**: 10 segundos continuos con congelación instantánea (**FREEZE**).
- **Polifonía de Granos**: Hasta 128 granos simultáneos con interpolación Hermite cúbica.
- **Envolventes**: Hann, Gaussian, Tukey, Triangle.
- **26 Presets de fábrica**: Ambient, Rhythmic, Textures, Drones, Glitch, Lo-Fi y Experimental.
- **XY Pad**: Control bidireccional expresivo y aleatorizador.
- **Efectos Integrados**: Filtro SVF, Stereo Delay, Algorithmic Reverb y Saturación estilo cinta.
- **Modulación**: Doble LFO sincronizable.

---

### 2. 🎹 GRANULAR SYNTH LAB (Sintetizador Granular Polifónico)
Instrumento musical virtual polifónico controlado por MIDI que genera sonidos a partir de fuentes de audio granulares híbridas.
- **Tipo**: Instrumento virtual sintetizador (CLAP / VST3 / Standalone).
- **Polifonía MIDI**: Hasta 16 voces con seguimiento de tono cromático (1v/oct relative a nota raíz C3) y sensibilidad de velocidad.
- **Rueda de Pitch Bend & Modulación**: Rango configurable de 1 a 24 semitonos.
- **Fuente Híbrida**:
  - **Modo Oscilador**: Generador de síntesis integrado con 7 formas de onda modeladas analógicamente:
    - *Supersaw* (Stack de 7 sierras desafinadas con paneo estéreo)
    - *Warm Sine* (Fundamental pura con 2do armónico cálido)
    - *Analog Triangle* (Triángulo saturado)
    - *PWM Square* (Pulso con modulación de ancho de pulso)
    - *Pink Noise Cloud* (Ruido rosa estereofónico)
    - *FM Crystal Bell* (2 operadores FM metálicos cristalinos)
    - *Vocal Choir Formant* (Formantes vocales resonantes estilo coro 'Aah')
  - **Modo Sample**: Cargador de archivos de audio externos con **Drag & Drop** directo (.wav, .flac, .aiff, .ogg, .mp3) desde el navegador de Bitwig o el explorador de archivos.
- **Envolvente ADSR**: Control dedicado de Attack, Decay, Sustain y Release por voz con visualizador interactivo de curva.
- **Teclado Virtual Integrado**: Teclas interactivas en pantalla con iluminación de notas MIDI entrantes.
- **21 Presets de fábrica**: Ambient Pads, Celestial Choirs, Velvet Rhodes, Leads ciberpunk, Sub Basses, y Texturas experimentales.
- **XY Pad, Filtro SVF, Delay, Reverb y Saturación**.

---

## 🛠️ Compilación e Instalación

### Clonar Repositorio
```bash
git clone --recursive https://github.com/gatovillano/granular-lab.git
cd granular-lab
```

### Requisitos en Ubuntu (24.04 / 26.04)
```bash
sudo apt-get install -y build-essential cmake ninja-build \
    libasound2-dev libfreetype-dev libgl-dev libx11-dev \
    libxcursor-dev libxinerama-dev libxrandr-dev
```

### Compilar e Instalar Ambos Plugins
```bash
# 1. Configurar con Ninja
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 2. Compilar e instalar ambos plugins en ~/.clap y ~/.vst3
ninja -C build install_plugins
```

Archivos instalados en el sistema:
- `~/.clap/Granular Lab.clap`
- `~/.vst3/Granular Lab.vst3`
- `~/.clap/Granular Synth Lab.clap`
- `~/.vst3/Granular Synth Lab.vst3`

### Ejecutar Pruebas Unitarias DSP
```bash
# Pruebas del efecto
./build/dsp_tests

# Pruebas del sintetizador
./build/synth_dsp_tests
```

---

## 🎵 Uso en Bitwig Studio

1. Abre **Bitwig Studio**.
2. **Para procesar audio**: Arrastra **Granular Lab** a una pista de audio como efecto de inserción.
3. **Para tocar como instrumento**: Crea una pista de instrumento y carga **Granular Synth Lab**.
   - Toca notas desde tu teclado MIDI o secuenciador.
   - Alterna entre el modo *Oscillator* (Supersaw, FM, Vocal, etc.) o arrastra tus propios archivos de audio directamente a la vista de onda (*Waveform*).
   - Moldea la envolvente ADSR, modula con el XY Pad y activa los efectos de Reverb, Delay y Saturación.

---

## 📄 Licencia

Este proyecto está bajo la licencia **MIT**. Consulta el archivo [LICENSE](LICENSE) para más detalles.
