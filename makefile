.PHONY:all
all:
	@cd compile_server;\
	make;\
	cd -;\
	cd OJ_server;\
	make;\
	cd -;

.PHONY:output
output:
	@mkdir -p output/compile_server;\
	mkdir -p output/compile_server/temp;\
	mkdir -p output/OJ_server;\
	cp -rf OJ_server/conf output/OJ_server/;\
	cp -rf OJ_server/lib output/OJ_server/;\
	cp -rf OJ_server/include output/OJ_server/;\
	cp -rf OJ_server/wwwroot output/OJ_server/;\
	cp -rf OJ_server/template_html output/OJ_server/;\
	cp -rf OJ_server/oj_server output/OJ_server/;\
	cp -rf OJ_server/questions output/OJ_server/;\
	cp -rf compile_server/compile_server output/compile_server;\
	cp -rf compile_server/compile_server output/compile_server;\
	cp -rf compile_server/temp output/compile_server;


.PHONY:clean
clean:
	@cd compile_server;\
	make clean;\
	cd -;\
	cd OJ_server;\
	make clean;\
	cd - ;\
	rm -rf output;


