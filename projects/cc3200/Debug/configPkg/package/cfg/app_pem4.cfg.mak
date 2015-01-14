# invoke SourceDir generated makefile for app.pem4
app.pem4: .libraries,app.pem4
.libraries,app.pem4: package/cfg/app_pem4.xdl
	$(MAKE) -f H:\Users\Akram\Programs\GitHub\havroc\projects\cc3200/src/makefile.libs

clean::
	$(MAKE) -f H:\Users\Akram\Programs\GitHub\havroc\projects\cc3200/src/makefile.libs clean

