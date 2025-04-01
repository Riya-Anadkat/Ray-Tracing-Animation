ifndef config
  config=debug
endif
export config

PROJECTS := A4

.PHONY: all clean help $(PROJECTS)

all: $(PROJECTS)

A4: 
	@echo "==== Building A4 ($(config)) ===="
	@${MAKE} --no-print-directory -C build -f Makefile

clean:
	@${MAKE} --no-print-directory -C build -f Makefile clean