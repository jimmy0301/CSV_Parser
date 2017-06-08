CFLAGS = -g -Wall


main: conf.o common.o header_info.o csv_data.o
	gcc -o csv_main main.c conf.o common.o header_info.o csv_data.o $(CFLAGS)
common.o: common.c common.h
	gcc -c common.c $(CFLAGS)
conf.o: conf.h conf.c err_code.h common.h common.o
	gcc -c conf.c $(CFLAGS)
header_info.o: header_info.c header_info.h err_code.h
	gcc -c header_info.c $(CFLAGS)
csv_data.o: csv_data.h common.h
	gcc -c csv_data.c $(CFLAGS)


clean:
	rm -f conf.o csv_main common.o header_info.o csv_data.o
