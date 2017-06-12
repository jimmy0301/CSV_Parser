CFLAGS = -g -Wall


main: conf.o common.o header_info.o csv_data.o csv_sort.o
	gcc -o csv_main main.c conf.o common.o header_info.o csv_data.o validator.o csv_sort.o $(CFLAGS)
common.o: common.c common.h
	gcc -c common.c $(CFLAGS)
conf.o: conf.h conf.c err_code.h common.h common.o
	gcc -c conf.c $(CFLAGS)
header_info.o: header_info.c header_info.h err_code.h
	gcc -c header_info.c $(CFLAGS)
csv_data.o: csv_data.h common.h validator.o validator.h
	gcc -c csv_data.c $(CFLAGS)
validator.o: common.h header_info.h
	gcc -c validator.c $(CFLAGS)
csv_sort.o: common.h header_info.h csv_sort.h err_code.h csv_data.h
	gcc -c csv_sort.c $(CFLAGS)

.PHONY: clean

clean:
	rm -f conf.o csv_main common.o header_info.o csv_data.o csv_sort.o validator.o
