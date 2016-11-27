# -*- encoding:utf-8 -*-
import os
import os.path

"""
include makefile.mk

$(LUA_OBJ):%%.o:%%.c
	@echo Compiling C: $<
	$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o $(OBJDIR)$@

.PHONY: clean
clean:
	rm -f $(LUA_OBJ)
"""


dirs = os.listdir()
for dir in dirs:
	if os.path.isdir(dir):
		# os.execlp("D:\\WinAVR-20090313\\utils\\bin\\cp.exe", "Makefile", dir)
		# exec("copy Makefile "+dir)
		# os.system("copy Makefile "+dir)
		subdirs = os.listdir(dir)
		wf = open(dir+"\\makefile.mk",'w')
		count = 0;
		havecfile = False
		wf.write(dir + 'OBJ := ')

		for subdir in subdirs:
			if ".c" == subdir[-2:] :
				if os.path.isfile(dir + "\\" + subdir):
					if count > 3:
						count = 0
						wf.write("\\\n")
					wf.write(subdir[:-2] + '.o ')
					count+=1
					havecfile = True
		wf.write("\n\n")
		wf.write("OBJ += $("+dir + "OBJ)\n")
		wf.close()
		if havecfile:
			wf = open(dir+"\\Makefile",'w')
			wf.write("include makefile.mk\n\n")
			wf.write("INCLUDES:= -I ../Include\n")
			wf.write("OBJDIR:=../obj\n")
			wf.write("VPATH=$(OBJDIR)\n\n")
			wf.write("all:$(%s)\n\n"%(dir + "OBJ"))
			wf.write("$(%s):%%.o:%%.c\n"%(dir + "OBJ"))
			wf.write("\t@echo Compiling C: $<\n")
			wf.write("\t$(CC) -c $(CCFLAGS) $(INCLUDES) -D$(DEVICETYPE) $< -o ../obj/$@\n\n")
			wf.write(".PHONY: clean\nclean:\n\trm -f $(%s)"%(dir + "OBJ"))
			wf.close()
		else:
			os.system("rm -f "+dir+"\\makefile.mk")