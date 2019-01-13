TARGET   = timer

TARGET_ARCH=riscv32

CC       = riscv32-unknown-elf-gcc

# compiling flags here
CFLAGS = -Wall -I. -O0 -static --specs=nosys.specs

LINKER   = riscv32-unknown-linux-gnu-gcc
# linking flags here
LDFLAGS   = -I. --entry main -L/opt/riscv/riscv32-unknown-elf/lib/ -T ld_script.ld
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
#	$(LINKER) $(OBJECTS) $(LDFLAGS) $(LIBS) $(LIBDIR) -o $@
	riscv32-unknown-linux-gnu-objdump -d $< > dump
	objcopy -Oihex $< $(TARGET).hex
#	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@echo "Compiling "$<" ..."
#	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
	@echo "Done!"

#$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.S
#	@echo "Assembling "$<" ..."
#       $(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
#	$(CC) $(CFLAGS) $(INCDIR) $< -o $@
#	@echo "Done!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS) *.hex dump
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
