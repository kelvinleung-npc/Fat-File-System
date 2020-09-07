CFLAGS = -Wall -g -Og 
LDFLAGS =  

DINFOOBJ = diskinfo.o 
DLISTOBJ = disklist.o 
DGETOBJ = diskget.o 
DPUTOBJ = diskput.o 

all: diskinfo disklist diskget diskput 

diskinfo: $(DINFOOBJ)
	$(CC) $(CFLAGS) -o diskinfo $(DINFOOBJ) $(LDFLAGS) -lreadline -ltermcap

disklist: $(DLISTOBJ)
	$(CC) $(CFLAGS) -o disklist $(DLISTOBJ) $(LDFLAGS) -lreadline -ltermcap

diskget: $(DGETOBJ)
	$(CC) $(CFLAGS) -o diskget $(DGETOBJ) $(LDFLAGS) -lreadline -ltermcap

diskput: $(DPUTOBJ)
	$(CC) $(CFLAGS) -o diskput $(DPUTOBJ) $(LDFLAGS) -lreadline -ltermcap

clean: 
	rm -rf $(DINFOOBJ) diskinfo
	rm -rf $(DLISTOBJ) disklist
	rm -rf $(DGETOBJ) diskget
	rm -rf $(DPUTOBJ) diskput