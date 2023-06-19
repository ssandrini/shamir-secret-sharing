# Esquema para compartir imágenes secretas

Programa en lenguaje C que implementa el esquema _"(k,n) secret image sharing scheme capable of cheating detection"_ de Yan-Xiao Liu, Quin-Dong Sun y Ching-Nung Yang de la Universidad de Tecnología de Xi'an (China).

## Programa

El programa implementado permite dividir una imágen BMP secreta en múltiples sombras, para asi distribuirlas entre un grupo de actores. Para recuperar la imágen secreta original, es necesario reunir una cantidad mínima establecida previamente de sombras. Cabe destacar que la implementación detecta si una de las sombras provistas es falsa, es decir detecta trampas. Las sombras están ocultas estenográficamente en otras imágenes.

## Instrucciones de compilación y ejecución

1. Clonar el repositorio.
2. Crear dentro del  repositorio el directorio _"bin/"_ para almacenar los binarios: `mkdir bin`
3. Compilar el proyecto: `make all`
4. Ejecutar el programa: `./bin/bmp_main archivo [d | r] k directorio`
   - `archivo`: Nombre de la imágen que se quiere ocultar o recuperar.
   - `d`: Para ocultar una imágen secreta en otras imágenes.
   - `r`: Para recuperar una imágen secreta a partir de otras imágenes.
   - `k`: Número que especifica la cantidad de imágenes necesarias para recuperar la imágen secreta.
   - `directorio`: Ruta del directorio donde se encuentran las imágenes para recuperar la imágen secreta o donde se quiere guardar las imágenes con el secreto ocultado.

## Ejemplo de uso (Asumiendo que en los directorios indicados hay 8 imágenes)

1. Distribuir la imagen “Albert.bmp” según esquema (4,8) ocultando las sombras en las imágenes del directorio “covers/280x440”:
 - ./bin/bmp_main d Albert.bmp 4 covers/280x440/
2. Recuperar la imagen “secreto.bmp”, en un esquema (4,8) buscando imágenes en el directorio “shares/280x440/”
 - ./bin/bmp_main r secreto.bmp 4 shares/280x440/ 