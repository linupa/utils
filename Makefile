OS = $(shell uname -s | sed s/$1-.*//g)
MACHINE = $(shell uname -m)
BUILD = $(OS)_$(MACHINE)

SOURCES = fileio.cpp Comm.cpp timestamp.cpp default.cpp tinyxml.cpp tinyxmlparser.cpp tinyxmlerror.cpp tinystr.cpp
OBJECTS = $(patsubst %.cpp,$(BUILD)/%.o,$(SOURCES))
LIB = $(BUILD)/libutils.a


LIB_FLAGS = rcs

all: $(BUILD) $(LIB)
#	@echo $(BUILD)
#	@echo $(SOURCES)
#	@echo $(OBJECTS)

$(LIB): $(OBJECTS) 
	ar $(LIB_FLAGS) $@ $^


$(OBJECTS): $(BUILD)/%.o : %.cpp
	g++ -c -g $< -o $@

$(BUILD):
	mkdir $(BUILD)

clean:
	rm -f $(LIB) $(OBJECTS)
