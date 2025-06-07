files = foo.c bar.c baz.c
# 为每个.c文件生成对应的.o文件目标
objects := $(foreach file,$(files),$(patsubst %.c, %.o, $(file)))

all:
	@echo Source files: $(files)
	@echo Object files: $(objects)