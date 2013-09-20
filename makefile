CC=cl
LK=link /NOLOGO /SUBSYSTEM:CONSOLE	/RELEASE 
CCFLAG=/Zi /c /nologo
LIBS=Ws2_32.lib
all:bin/server.exe bin/client.exe

bin/server.exe:server.obj netfile.obj process.obj url.obj file_sys.obj
    $(LK) /OUT:$@ server.obj netfile.obj process.obj url.obj file_sys.obj $(LIBS) shell32.lib Kernel32.lib Advapi32.lib

server.obj:server.c
    $(CC) $(CCFLAG) /Fo$@ server.c
	
bin/client.exe:client.obj
	$(LK) /OUT:$@ client.obj netfile.obj $(LIBS)

client.obj:client.c
    $(CC) $(CCFLAG) /Fo$@ client.c
	
netfile.obj:netfile.c
	$(CC) $(CCFLAG) /Fo$@ netfile.c
	
process.obj:process.c
	$(CC) $(CCFLAG) /Fo$@ process.c
	
url.obj:url.c
	$(CC) $(CCFLAG) /Fo$@ url.c
	
file_sys.obj:file_sys.c
	$(CC) $(CCFLAG) /Fo$@ file_sys.c
	
clean:
	del *.obj
	del *.pdb
	(cd bin&del *.exe)
	