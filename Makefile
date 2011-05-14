TOP = .
CFLAGS := -Wall -g `pkg-config --cflags --libs gtk+-2.0` 

t2g_gui : t2g_gui.c
	gcc -o t2g_gui $(CFLAGS) t2g_gui.c
