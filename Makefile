CC=gcc

all: src/dwmbar.c
	$(CC) src/dwmbar.c -o dwmbar -lpthread -lX11

clean: dwmbar
	rm dwmbar

install: dwmbar
	sudo cp dwmbar /usr/local/bin

uninstall: /usr/local/bin/dwmbar
	sudo rm /usr/local/bin/dwmbar
