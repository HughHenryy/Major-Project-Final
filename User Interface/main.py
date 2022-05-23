# This python code will display the visualization of the simplified radar chart and the state of each booth
import os.path
import time
import serial
import struct
import traceback
import pygame
import math
import sys
import random

MSG_HEADER_SIZE = 16

Booth1_angle_start = -10
Booth1_angle_end = 10

Booth2_angle_start = 990
Booth2_angle_end = 1010

Booth3_angle_start = 1990
Booth3_angle_end = 2010

boothStatus = [0, 0, 0]
limit = 2000

booth_image = pygame.image.load(os.path.join("Asset", "Booth_icon.png"))
booth_display = pygame.transform.scale(booth_image, (100, 100))


def read_packet(f):
    header_bytes = f.read(MSG_HEADER_SIZE)

    if len(header_bytes) < MSG_HEADER_SIZE:
        # must be out of messages
        return False

    header_data = struct.unpack(">H8sHHH", header_bytes)
    print("header sentinels: " + str(hex(header_data[0])) + ", " + str(hex(header_data[4])))

    message_type = header_data[1].split(b'\0', 1)[0]  # remove the null characters from the string
    print(message_type)
    print("message size: " + str(header_data[2]))

    if message_type == b"text":
        text_bytes = f.read(header_data[2])
        print("text message: " + str(text_bytes))

    elif message_type == b"DATA":
        bytes = f.read(header_data[2])
        data = struct.unpack(">hhLLH", bytes)
        print("DATA message: " + "Booth number: " + (str(int(data[1] / 1000 + 1)) + ", " + "Distance: " + str(
            data[2]) + ", " + "Original distance: " + str(data[3])))
        # Position  angle           # Lidar data                #None

        visualization(data)
    return True


def visualization(data):
    # Initialize pygame
    pygame.init()
    screen = pygame.display.set_mode((700, 400))
    pygame.display.set_caption("Main project G2")
    # pygame：0----->x
    #        |
    #        ↓
    #        y
    # Define a queue to collect data received by the serial port
    while True:
        ################################
        # Keep the window open until the user closes it#
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                sys.exit()
        #############################################################

        distance = data[2]  # data[1]=Detection range   this is the data. I don't know what lidar sensor provide
        # if it is in raw data, It should be change in to cm please tell me
        # if it is in cm , no change
        LIDAR_REF = data[3]

        # Background covering
        screen.fill((0, 0, 0))

        if distance < (LIDAR_REF - limit):  # Corresponding distance and warning text
            screen.blit(pygame.font.Font(None, 25).render(str(distance), 1, "red"), (130, 320))
            screen.blit(pygame.font.Font(None, 25).render(str(LIDAR_REF), 1, "green"), (290, 320))

        else:
            screen.blit(pygame.font.Font(None, 25).render(str(distance), 1, "green"), (130, 320))
            screen.blit(pygame.font.Font(None, 25).render(str(LIDAR_REF), 1, "green"), (290, 320))

        if Booth1_angle_start < data[1] < Booth1_angle_end:
            if distance < (LIDAR_REF - limit):
                boothStatus[0] = 1
            else:
                boothStatus[0] = 0
        elif Booth2_angle_start <= data[1] < Booth2_angle_end:
            if distance < (LIDAR_REF - limit):
                boothStatus[1] = 1
            else:
                boothStatus[1] = 0
        elif Booth3_angle_start < data[1] < Booth3_angle_end:
            if distance < (LIDAR_REF - limit):
                boothStatus[2] = 1
            else:
                boothStatus[2] = 0

        if boothStatus[0] == 1:
            screen.blit(pygame.font.Font(None, 25).render("OCCUPIED ", 1, (251, 10, 10)), (130, 360))
            pygame.draw.circle(screen, "red", (150, 250), 25)
        else:
            screen.blit(pygame.font.Font(None, 25).render("VACANCY", 1, (10, 10, 251)), (130, 360))
            pygame.draw.circle(screen, "green", (150, 250), 25)
        if boothStatus[1] == 1:
            screen.blit(pygame.font.Font(None, 25).render("OCCUPIED ", 1, (251, 10, 10)), (360, 360))
            pygame.draw.circle(screen, "red", (350, 250), 25)
        else:
            screen.blit(pygame.font.Font(None, 25).render("VACANCY", 1, (10, 10, 251)), (360, 360))
            pygame.draw.circle(screen, "green", (350, 250), 25)
        if boothStatus[2] == 1:
            screen.blit(pygame.font.Font(None, 25).render("OCCUPIED ", 1, (251, 10, 10)), (590, 360))
            pygame.draw.circle(screen, "red", (550, 250), 25)
        else:
            screen.blit(pygame.font.Font(None, 25).render("VACANCY", 1, (10, 10, 251)), (590, 360))
            pygame.draw.circle(screen, "green", (550, 250), 25)

        #########################################################################################################
        # Background for making radar maps
        #########################################################################################################
        # Draw bottom lines and text messages
        pygame.draw.line(screen, "green", (0, 300), (700, 300), 2)
        screen.blit(pygame.font.Font(None, 25).render("Distance:", 1, "green"), (50, 320))
        screen.blit(pygame.font.Font(None, 25).render("Base:", 1, "green"), (210, 320))
        screen.blit(pygame.font.Font(None, 25).render("Booth:", 1, "green"), (350, 320))
        screen.blit(
            pygame.font.Font(None, 25).render(str(int(data[1] / 1000 + 1)), 1, "green"),
            (450, 320))

        screen.blit(pygame.font.Font(None, 25).render("Booth 1: ", 1, (251, 114, 153)), (50, 360))
        screen.blit(pygame.font.Font(None, 25).render("Booth 2: ", 1, (251, 114, 153)), (280, 360))
        screen.blit(pygame.font.Font(None, 25).render("Booth 3: ", 1, (251, 114, 153)), (510, 360))

        screen.blit(booth_display, (100, 100))
        screen.blit(booth_display, (300, 100))
        screen.blit(booth_display, (500, 100))

        # Update the screen
        pygame.display.update()
        # Dragon board running speed
        pygame.time.Clock().tick(24 * 10 ^ 6)
        return True


def read_serial(com_port):
    serialPort = serial.Serial(port=com_port, baudrate=9600, bytesize=8, timeout=2, stopbits=serial.STOPBITS_ONE)
    serialString = ""  # Used to hold data coming over UART

    while True:

        # Wait until there is data waiting in the serial buffer
        if serialPort.in_waiting > 0:

            try:
                if not read_packet(serialPort):
                    break
            except Exception as e:
                # Logs the error appropriately.
                print(traceback.format_exc())
                break

        else:
            time.sleep(0.05)


# main program entry point
if __name__ == '__main__':
    # read_file('C:/Users/Stewart Worrall/Documents/data/test.hex')
    read_serial("COM8")
