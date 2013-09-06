#
#	GMK Makefile
#

# Paths
SOURCES_PATH=src
INCLUDE_PATHS=lib/zlib src/include
OBJECTS_PATH=objs
TARGET=bin/linux/libgmk.a

# Tools
CXX=g++
AR=ar

# Parse Parameters
SOURCES=$(foreach dir, $(SOURCES_PATH), $(wildcard $(dir)/*.cpp))
OBJECTS=$(SOURCES:.cpp=.o)
OBJLIST=$(addprefix $(OBJECTS_PATH)/,$(OBJECTS))
OBJDIRS=$(sort $(dir $(OBJLIST))) $(dir $(TARGET))

# Rules
all: $(TARGET)

$(TARGET): $(OBJLIST)
	@$(AR) rvs $(TARGET) $(OBJLIST) > /dev/null

$(OBJECTS_PATH)/%.o: %.cpp | $(OBJDIRS)
	@$(CXX) $(foreach dir, $(INCLUDE_PATHS), -I$(dir)) -c $< -o $@

$(OBJDIRS):
	@mkdir -p $@
	
clean:
	@find $(OBJECTS_PATH) -name '*.o' | xargs rm  -f > /dev/null
	@rm -f $(TARGET) > /dev/null
