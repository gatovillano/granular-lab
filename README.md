# GRANULAR LAB

**GRANULAR LAB** es un plugin profesional de procesamiento granular de audio en tiempo real para Linux (Ubuntu), diseñado para transformar señales convencionales (voces, sintetizadores, instrumentos acústicos, percusiones y foley) en texturas atmosféricas, nubes evolutivas, glitches rítmicos y paisajes sonoros envolventes.

Desarrollado en C++20 con JUCE 8 y soporte nativo para **CLAP** (mediante `clap-juce-extensions`) y **VST3**, optimizado específicamente para un rendimiento sin bloqueos (*lock-free*) y ultra-bajo consumo de CPU en **Bitwig Studio** y DAWs de Linux.

---

## 🌟 Características Principales

### 🎛️ 1. Motor Granular Polifónico de Alta Fidelidad
- **Polifonía**: Hasta 128 granos simultáneos preasignados en memoria (*zero dynamic allocation* en el hilo de audio).
- **Control de Tamaño (Grain Size)**: Desde micro-granos de 10 ms hasta fragmentos de 500 ms.
- **Densidad Dinámica**: Desde 1 grano/seg hasta 100 granos/seg (Hz).
- **Interpolación Hermite Cúbica (4 puntos)**: Lectura continua sin saltos ni asperezas digitales al aplicar pitch shifting.
- **Transposición y Aleatorización de Pitch**: De -24 a +24 semitonos con control de *Pitch Random*.
- **Posicionamiento Temporal & Spray**: Selección continua del cabezal de lectura en el buffer con fluctuación aleatoria (*Spray*).
- **Dirección Inversa (Reverse)**: Probabilidad configurable (0% a 100%) de granos invertidos.
- **Envolventes Suaves sin Clics**: Selección de ventanas matemáticas (Hann, Gaussian, Tukey, Triangle).
- **Buffer Circular de 10 Segundos**: Almacenamiento estéreo continuo con función **FREEZE** para congelar el sonido en bucles infinitos.
- **Feedback Granular**: Realimentación de los granos procesados al buffer de entrada con limitación suave analógica.

### 🕹️ 2. XY Pad Expresivo
- Área de interacción gestual de gran tamaño con cursor de neón y física suavizada.
- Asignación flexible e independiente de ejes:
  - **Eje X**: Density, Grain Size, Position, Spray, Pitch, Filter Cutoff, Mix.
  - **Eje Y**: Density, Grain Size, Position, Spray, Pitch, Filter Cutoff, Mix.
- Botón **RND** (*Randomize*) para generar instantáneamente variaciones musicales.
- Automatizable vía parámetros del host (`xy_x`, `xy_y`).

### 📊 3. Visualizaciones en Tiempo Real
- **Stereo Buffer Waveform View**: Muestra la forma de onda del buffer circular de 10 segundos, el cabezal de grabación (*write head*), la ventana activa de escaneo de granos y el indicador de *Freeze*.
- **Granular Particle View**: Campo de partículas reactivo donde cada grano activo se representa en tiempo real:
  - Eje horizontal: Posición en el buffer.
  - Eje vertical: Panorama estéreo.
  - Radio: Duración del grano.
  - Luminosidad/Opacidad: Envolvente de amplitud.
  - Color: Desplazamiento de tono (Cian = neutro, Violeta = agudo, Ámbar = grave, Menta = inverso).
- **Comunicación Lock-Free**: Cola atómica SPSC entre el motor DSP y la GUI, garantizando que el refresco visual a 60 FPS nunca interfiera con la estabilidad del audio.

### 🌊 4. Motor de Modulación (Modulation Engine)
- Dos LFOs independientes con formas de onda: **Sine**, **Triangle**, **Saw**, y **Sample & Hold** (interpolado).
- Rango de frecuencia de 0.05 Hz a 20 Hz.
- Destinos asignables:
  - Read Position
  - Grain Size
  - Grain Density
  - Pitch Shift
  - Stereo Pan
  - Filter Cutoff

### 🔮 5. Suite de Efectos Integrada (FX Engine)
- **Filtro Multimodo State Variable (SVF)**: Low Pass, High Pass y Band Pass con frecuencia de corte (20 Hz - 20 kHz) y resonancia Q.
- **Stereo Delay**: Con tiempo milimétrico (10 - 2000 ms), feedback con saturación suave, ampliación estéreo y control de mezcla.
- **Reverb Algorítmica**: Espacios exuberantes con control de tamaño, amortiguación (*damping*) y mezcla wet/dry.
- **Saturación Analógica**: Curva cúbica cálida estilo cinta magnética con eliminador de DC (*DC blocker*) y control de Drive.

### 💾 6. Gestor de Presets
- Presets de fábrica incluidos:
  - *Atmospheric Cloud* (Texturas atmosféricas amplias)
  - *Glitch Reverse* (Micro-fragmentación rítmica)
  - *Shimmer Pad* (Evolución celestial +12st)
  - *Frozen Drone* (Drones armónicos congelados)
  - *Vocal Scatter* (Esparcimiento estéreo para voces)
  - *Cosmic Dust* (Partículas cristalinas y nubes micro-granulares)
- Persistencia total del estado XML/ValueTree compatible con guardar y recargar proyectos en Bitwig Studio.

---

## 🛠️ Requisitos y Compilación en Ubuntu Linux

### Dependencias requeridas
En Ubuntu 24.04 / 26.04:
```bash
sudo apt-get install -y build-essential cmake ninja-build \
    libasound2-dev libfreetype-dev libgl-dev libx11-dev \
    libxcursor-dev libxinerama-dev libxrandr-dev
```

### Compilar e Instalar

```bash
# 1. Configurar proyecto con CMake y Ninja
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release

# 2. Compilar plugins CLAP, VST3 y Standalone
ninja -C build GranularLab_CLAP GranularLab_VST3 GranularLab_Standalone

# 3. Instalar automáticamente en ~/.clap y ~/.vst3
ninja -C build install_plugins
```

Los binarios instalados quedarán en:
- `~/.clap/Granular Lab.clap`
- `~/.vst3/Granular Lab.vst3`

### Ejecutar Pruebas Unitarias DSP
```bash
ninja -C build dsp_tests
./build/dsp_tests
```

---

## 🎵 Uso en Bitwig Studio

1. Abre **Bitwig Studio**.
2. En una pista de audio o instrumento (por ejemplo, Voz, Piano, Cuerdas o Sintetizador), inserta el plugin **Granular Lab** desde la sección de plugins (disponible en formato CLAP y VST3).
3. Selecciona un preset en el encabezado (*Atmospheric Cloud*, *Glitch Reverse*, etc.) o mueve el **XY Pad** para explorar texturas en tiempo real.
4. Presiona **FREEZE** en cualquier momento para capturar y mantener el sonido infinitamente mientras modulas tamaño y densidad.

---

## 📄 Licencia

Este proyecto está bajo la licencia **MIT**. Consulta el archivo [LICENSE](LICENSE) para más detalles.
