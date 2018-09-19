TARGET   = trace3

TARGET_ARCH=riscv32

CC       = riscv32-unknown-linux-gnu-gcc
# compiling flags here
#CFLAGS   = -Wall -I. -O0 -nostartfiles  -march=rv32i -mabi=ilp32
CFLAGS = -Wall -I. -O0 -nostdlib -march=rv32i -mabi=ilp32

LINKER   = riscv32-unknown-linux-gnu-gcc
# linking flags here
LFLAGS   = -I.
LIBS   = $(EXTRA_LIBS)


# change these to proper directories where each file should be
SRCDIR   = ./
OBJDIR   = .
BINDIR   = ./
INCDIR = -I.
LIBDIR = -L.


SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
#	$(LINKER) $(OBJECTS) $(LFLAGS) $(LIBS) $(LIBDIR) -o $@
	riscv32-unknown-linux-gnu-objdump -d $< > dump
	objcopy -Oihex $< $(TARGET).hex
#	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo "Compiling "$<" ..."
#	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
	@echo "Done!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
