CPP      = g++
CXXFLAGS = #-DDEBUG__  #-DDEBUG___  #-std=c++11 -O3 #-DYYDEBUG=1  

SRC_DIR     = src/
BIN_DIR     = bin/
LIB_DIR     = lib/
OBJS_DIR    = obj/
INCLUDE_DIR = include/
INCLUDE     = $(addprefix -I, $(INCLUDE_DIR))

LIB = $(LIB_DIR)/robonlib.a

SRCS_FILES    = stl_parser.cpp \
          stl_scanner.cpp \
          stl_driver.cpp \
          tools.cpp \
          stl_atom.cpp \
          transducer.cpp \
          interval_transducer.cpp \
          interval.cpp \
          update_transducer.cpp \
          robustness.cpp \
          signal.cpp \
          signal_expr.cpp 

HEADRS_FILES = FlexLexer.h \
		interval.h \
		location.hh \
		position.hh \
		robustness.h \
		signal.h \
		signal_expr \
		stack.hh \
		stl_driver.h \
		stl_parser.h \
		stl_scanner.h \
		tools.h \
		transducer.h 


SRCS   = $(addprefix $(SRC_DIR), $(SRCS_FILES))
HEADRS   = $(addprefix $(INCLUDE_DIR), $(HEADRS_FILES))

OBJS_FILES  = $(patsubst %.cpp,%.o,$(SRCS_FILES))
OBJS = $(addprefix $(OBJS_DIR), $(OBJS_FILES))

all: lib

lib: $(LIB)

$(LIB): $(OBJS)
	ar crfvs $(LIB) $(OBJS)  

obj/%.o : src/%.cpp 
	$(CPP) -c $(CXXFLAGS) $(INCLUDE) $< -o $@

clean:  
	rm -f $(OBJS) $(LIB)

echo:
	@echo $(HEADRS)
	@echo $(SRCS)
	@echo $(INCLUDE) 
	@echo $(OBJS)


