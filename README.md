# Proyecto 1 - Cache coherence modelling and evalution - README

Este README proporciona instrucciones sobre cómo compilar y ejecutar el código en el Taller 1 de OpenMP. 

Realizado por Jose Alejandro Chavarría.

## Requisitos

### Tools 

- **GCC**:  El código está diseñado para ser compilado con el compilador GCC (GNU Compiler Collection).
- **Make**:  Utilizado para llevar a cabo el proceso de compilado o execución del backend.


### Software 

- **Unity**: (v2018.3.8) Motor gráfico para crear interfaces gráficas 



### SO y architectura 

Este proyecto fue desarrollado en Linux, especificamente en `Ubuntu 18.0.4` y `Linux Mint 21.2 Victoria` para un arquitectura `x86_64`



## Quick Start

## Backend

Para compilar y ejecutar rápidamente todos los programas del backend, sigue estos pasos:

1. Compila todos los programas utilizando el siguiente comando:

   ```bash
   make build
   ```

   Esto compilará todos los programas dentro del directio `backend/src`.
   

3. Ejecuta el backend a partir del siguiente comando:

   ```bash
   make exec
   ```

## Frontend

Estos son los pasos generales para compilar un proyecto de Unity y crear un ejecutable para tu plataforma de destino.
### 1. Abre el Proyecto

Abre tu proyecto de Unity en el editor.

### 2. Configura la Plataforma

Dirígete a "File" (Archivo) > "Build Settings" (Configuración de Compilación) en el menú principal.

- Selecciona Linux como plataforma de destino en la que deseas compilar tu proyecto.

### 3. Ajusta las Configuraciones de Compilación

Configura las opciones de compilación según tus necesidades:

- Ajusta la resolución de pantalla y otras configuraciones específicas de la plataforma.

### 4. Compila el Proyecto

Haz clic en el botón "Build" (Compilar) en la ventana de "Build Settings".

- Elige o crea una carpeta donde se guardarán los archivos de compilación. Asegúrate de mantener esta carpeta organizada.

### 5. Ejecuta el Ejecutable

Una vez completada la compilación, ve a la carpeta de salida que especificaste en el paso anterior.

- Encuentra el archivo ejecutable correspondiente a tu plataforma

- Ejecuta el archivo ejecutable haciendo doble clic en él o usando la línea de comandos, según la plataforma.



o bien, puedes dirijirse al directorio `bin` y ejecute el siguiente comando

   ```bash
   ./frontend
   ```

#
## Docs 

[Manual de usuario](./docs/) <br>
[White paper](./docs/) <br>
