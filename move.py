import pygame
import serial
import threading
import pygame_chart as pyc

# Configurar conexión al monitor serial
ser = serial.Serial('/dev/ttyUSB0', 115200)  # Cambia '/dev/ttyUSB0' por tu puerto serial

# Inicializar Pygame
pygame.init()

# Configurar la pantalla
screen = pygame.display.set_mode((0, 0), pygame.FULLSCREEN)
# Figure instance on screen with position and size
figure = pyc.Figure(screen, 50, 50, 700, 500)
pygame.display.set_caption('Detección de Teclas y Lectura Serial')


# Variable para controlar el bucle principal
running = True


# Función para leer datos del monitor serial
def read_serial():
    while running:
        if ser.in_waiting > 0:  # Comprobar si hay datos disponibles
            data = ser.readline().decode('utf-8') # Leer y decodificar los datos
            print(data)


# Iniciar el hilo para lectura serial
serial_thread = threading.Thread(target=read_serial, daemon=True)
serial_thread.start()

# Bucle principal
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.KEYDOWN:
            #print(f'Tecla {pygame.key.name(event.key)} presionada')
            if pygame.key.name(event.key) == 'w':
                #print('Adelante')
                ser.write(b'f')
            elif pygame.key.name(event.key) == 'a':
                #print('Izquierda')
                ser.write(b'l')
            elif pygame.key.name(event.key) == 's':
                #print('Atrás')
                ser.write(b'b')
            elif pygame.key.name(event.key) == 'd':
                #print('Derecha')
                ser.write(b'r')
            elif pygame.key.name(event.key) == 'up':
                #print('Adelante')
                ser.write(b'z')
            elif pygame.key.name(event.key) == 'down':
                #print('Atrás')
                ser.write(b'x')
            elif pygame.key.name(event.key) == 'left':
                #print('Izquierda')
                ser.write(b'v')
            elif pygame.key.name(event.key) == 'right':
                #print('Derecha')
                ser.write(b'c')

        elif event.type == pygame.KEYUP:
            #print(f'Tecla {pygame.key.name(event.key)} liberada')
            ser.write(b's')
        # add a line chart. First argument "name" should be unique for every chart
    figure.line('Chart1', [1,2,3,4,6,20,24],[3,5,7,2,7,9,1])
    # draw figure with specified properties
    figure.draw()    
    pygame.display.update()

# Finalizar Pygame y cerrar conexión serial
pygame.quit()
ser.close()
