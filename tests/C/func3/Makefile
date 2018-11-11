TARGET   = func3

TARGET_ARCH=riscv32

CC       = riscv32-unknown-linux-gnu-gcc
# compiling flags here
#CFLAGS = -Wall -I. -O0 -nostdlib -march=rv32i -mabi=ilp32 --entry main 
CFLAGS = -Wall -I. -O0 -static
#CFLAGS = -Wall -I. -O0 -Xlinker --gc-sections -lgcc -lc -static --specs=nano.specs


LINKER   = riscv32-unknown-linux-gnu-gcc
# linking flags here
LFLAGS   = -I. --entry main -L/opt/riscv/riscv32-unknown-elf/lib/
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
	@$(rm) $(OBJECTS) *.hex dump
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
