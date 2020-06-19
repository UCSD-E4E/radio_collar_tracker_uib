nmea: main.c nmea.c strtok_e.c
	gcc main.c nmea.c strtok_e.c -o nmea

clean:
	rm *.o nmea

nmea.o: nmea.c nmea.h
	gcc -c nmea.c

main.o: main.c
	gcc -c main.c

strtok_e.o: strtok_e.c
	gcc -c strtok_e.c