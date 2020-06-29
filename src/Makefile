nmea: main.c nmea.c
	gcc -Wall main.c nmea.c -o nmea

clean:
	rm *.o nmea

nmea.o: nmea.c nmea.h
	gcc -c nmea.c

main.o: main.c
	gcc -c main.c