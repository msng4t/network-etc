SUBDIRS = \
	lib\
	PL\
	BDB\
	UDP\
	HTTP\
	dkv\

ifeq ($(PKG_DIR), )
	PKG_DIR = .
endif

.PHONY:all $(SUBDIRS)
all:$(SUBDIRS)
$(SUBDIRS):
	$(MAKE) -C $@;

clean:
	@for a in $(SUBDIRS); do cd $$a; $(MAKE) clean; cd ..; done

bin:
	@for a in $(SUBDIRS); do cd $$a; $(MAKE) package; cd ..; done

