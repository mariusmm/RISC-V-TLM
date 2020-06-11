TARGET   = rtti

TARGET_ARCH = riscv32

CC       = riscv32-unknown-elf-g++

# compiling flags here
CFLAGS = -Wall -I. -O0 -static -march=rv32imac -mabi=ilp32 --specs=nosys.specs

LINKER   = riscv32-unknown-elf-g++
# linking flags here
LDFLAGS   = -I. -static 
LIBS   = $(EXTRA_LIBS)


# change these to proper directories where each file should be
SRCDIR   = ./
OBJDIR   = .
BINDIR   = ./
INCDIR = -I.
LIBDIR = -L.


SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	$(LINKER) $(CFLAGS) $(LDFLAGS) $(LIBS) $(LIBDIR) $(OBJECTS) -o $@
	riscv32-unknown-elf-objdump -d $@ > dump
	riscv32-unknown-elf-objcopy -Oihex $@ $(TARGET).hex
	@echo "Linking complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@echo "Compiling "$<" ..."
	$(CC) $(CFLAGS) $(INCDIR) -c $< -o $@
	@echo "Done!"

.PHONY: clean
clean:
	@$(rm) $(OBJECTS) *.hex dump
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"
