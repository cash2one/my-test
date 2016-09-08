
ifdef ASM_INCLUDE
$(TARGET):$(OBJS) $(ASM_OBJS)
	@echo [AR] $@
	$(Q) $(AR) cr $@ $(OBJS) $(ASM_OBJS)
else
$(TARGET):$(OBJS)
	@echo [AR] $@
	$(Q) $(AR) cr $@ $(OBJS)
endif

%.pb-c.c: %.proto
	@echo [PROTOC-C] $@
	$(Q) $(PROTOC_C) --proto_path=$(@D) --c_out=$(@D) $<

%.lex.c: %.l
	@echo [LEX] $@
	$(Q) flex -t $^ > $@

%.tab.c: %.y
	@echo [YACC] $@
	$(Q) bison -d -v $^

$(BUILD_DIR)/%.o:%.s
	@echo [NASM] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(NASM) -f elf64 $< -o $@
	
$(BUILD_DIR)/%.d:%.s
	@echo [DEP] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed -n "H;$$ {g;s@\(.*\)\.o[ :]\(.*\)@$(BUILD_DIR)/$*.o $@: \$$\(wildcard\2\)@;p}" < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
$(BUILD_DIR)/%.o:%.c
	@echo [CC] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ $(CFLAGS) -c $<

$(BUILD_DIR)/%.d:%.c
	@echo [DEP] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed -n "H;$$ {g;s@\(.*\)\.o[ :]\(.*\)@$(BUILD_DIR)/$*.o $@: \$$\(wildcard\2\)@;p}" < $@.$$$$ > $@; \
	rm -f $@.$$$$

$(BUILD_DIR)/%.o:%.cpp
	@echo [CC] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -o $@ $(CFLAGS) -c $<

$(BUILD_DIR)/%.d:%.cpp
	@echo [DEP] $@
	$(Q) mkdir -p $(@D)
	$(Q) $(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed -n "H;$$ {g;s@\(.*\)\.o[ :]\(.*\)@$(BUILD_DIR)/$*.o $@: \$$\(wildcard\2\)@;p}" < $@.$$$$ > $@; \
	rm -f $@.$$$$

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPS)
endif

clean:
	-rm -rf $(BUILD_DIR)/* $(TARGET)

